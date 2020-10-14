#include "nt_helpers.h"

#include <_lotuscrt.h>

#include <stdint.h>

#define PHNT_NO_INLINE_INIT_STRING // No strlen dependency in LotusCRT
#include <phnt_windows.h>
#include <phnt.h>

extern IMAGE_LOAD_CONFIG_DIRECTORY _load_config_used;
extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;

// Win 10 1809+
typedef struct _VM_INFORMATION_1809
{
	DWORD NumberOfOffsets;
	DWORD MustBeZero;
	PDWORD TargetsProcessed;
	PCFG_CALL_TARGET_INFO CallTargets;
	union _Section
	{
		HANDLE Section;
		DWORD64 _data;
	} Section;
	ULONG64 ExpectedFileOffset;
} VM_INFORMATION_1809, *PVM_INFORMATION_1809;

// Win 10 1507 to 1803
typedef struct _VM_INFORMATION_1507
{
	DWORD NumberOfOffsets;
	DWORD MustBeZero;
	PDWORD TargetsProcessed;
	PCFG_CALL_TARGET_INFO CallTargets;
} VM_INFORMATION_1507, *PVM_INFORMATION_1507;

typedef struct _VM_INFORMATION
{
	PVOID vmInfo;
	ULONG vmSize;
} VM_INFORMATION, *PVM_INFORMATION;

static VM_INFORMATION_1507 vmInfo1507;
static VM_INFORMATION_1809 vmInfo1809;

static EXCEPTION_RECORD hardFailRecord;
static CONTEXT hardFailContext;

static EXCEPTION_POINTERS hardFailException =
{
	&hardFailRecord,
	&hardFailContext
};

static void get_vm_information(
	PVM_INFORMATION vmInfo,
	DWORD numberOfOffsets,
	PDWORD targetsProcessed,
	PCFG_CALL_TARGET_INFO callTargets)
{
	RTL_OSVERSIONINFOEXW osVer;
	RtlGetVersion(&osVer);

	if(osVer.dwBuildNumber >= 17763)
	{
		vmInfo->vmInfo = &vmInfo1809;
		vmInfo->vmSize = sizeof(vmInfo1809);

		vmInfo1809.Section.Section = NULL;
		vmInfo1809.ExpectedFileOffset = 0;
	}
	else
	{
		vmInfo->vmInfo = &vmInfo1507;
		vmInfo->vmSize = sizeof(vmInfo1507);
	}

	PVM_INFORMATION_1507 punnedInfo = (PVM_INFORMATION_1507)vmInfo->vmInfo;
	punnedInfo->MustBeZero = 0;
	punnedInfo->NumberOfOffsets = numberOfOffsets;
	punnedInfo->TargetsProcessed = targetsProcessed;
	punnedInfo->CallTargets = callTargets;
}

uint32_t _Lotus_protect_functions(void **const __functions, uint32_t __count)
{
	RTL_OSVERSIONINFOEXW osVer;
	RtlGetVersion(&osVer);

	// Controlling CFG at runtime is not supported on Windows 8.1 and lower,
	// so we just lie that this function succeeded.
	if(osVer.dwMajorVersion < 10)
		return __count;

	// CFG is disabled on this image.
	if(_load_config_used.GuardCFFunctionTable == 0)
		return __count;

	// We need to dynamically load ZwSetInformationVirtualMemory, because this
	// function has been introduced since Windows 10.
	typedef NTSTATUS (NTAPI *SetInformationVirtualMemory_t)(
		HANDLE,
		VIRTUAL_MEMORY_INFORMATION_CLASS,
		ULONG_PTR,
		PMEMORY_RANGE_ENTRY,
		PVOID,
		ULONG);

	ANSI_STRING functionName;
	RtlInitAnsiString(&functionName, "ZwSetInformationVirtualMemory");

	PLIST_ENTRY const moduleList = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
	PLIST_ENTRY const ntDLLEntry = moduleList->Flink->Flink;
	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
	HANDLE const ntDLL = ((PLDR_DATA_TABLE_ENTRY)ntDLLEntry)->DllBase;
	__LOTUSCRT_RESTORE_CLANG_WARNING()

	SetInformationVirtualMemory_t SetInformationVirtualMemory;
	NTSTATUS status = LdrGetProcedureAddress(
		ntDLL,
		&functionName,
		0,
		(void*)&SetInformationVirtualMemory);

	if(NT_ERROR(status))
		return 0;

	uint32_t protectedFuncs = 0;
	for(uint32_t i = 0; i < __count; ++i)
	{
		// To avoid allocating on the heap we will have to do multiple
		// ZwSetInformationVirtualMemory calls. At the moment of writing this,
		// allocation isn't even implemented yet. But we don't want to allocate
		// in here anyway, as that would open up attacks on CFG.

		MEMORY_IMAGE_INFORMATION memInfo;
		status = ZwQueryVirtualMemory(
			NtCurrentProcess(),
			__functions[i],
			MemoryImageInformation,
			&memInfo,
			sizeof(memInfo),
			NULL);

		// Function ptr was invalid
		if(NT_ERROR(status))
			return protectedFuncs;

		CFG_CALL_TARGET_INFO callTarget = {0};
		// Disable this function as a valid CFG destination
		callTarget.Flags = 0;
		callTarget.Offset =
			(ULONG_PTR)(__functions[i]) - (ULONG_PTR)(memInfo.ImageBase);

		MEMORY_RANGE_ENTRY memRange = {0};
		memRange.NumberOfBytes = memInfo.SizeOfImage;
		memRange.VirtualAddress = memInfo.ImageBase;

		DWORD targetsProcessed;
		VM_INFORMATION vmInfo;
		get_vm_information(&vmInfo, 1, &targetsProcessed, &callTarget);

		status = SetInformationVirtualMemory(
			NtCurrentProcess(),
			VmCfgCallTargetInformation,
			1,
			&memRange,
			vmInfo.vmInfo,
			vmInfo.vmSize);

		if(NT_ERROR(status))
			return protectedFuncs;

		++protectedFuncs;
	}

	return protectedFuncs;
}

__LOTUSCRT_NORETURN void _Lotus_raise_hard_error(
	uint32_t __errorCode,
	bool __useStackCookie,
	uintptr_t __stackCookie)
{
	volatile UINT_PTR cookie[2];

#ifdef __LOTUSCRT_PLATFORM_WIN32
	BYTE volatile dummyStack[(sizeof(CONTEXT) + sizeof(EXCEPTION_RECORD))];
	__LOTUSCRT_UNUSED(dummyStack);

#ifndef __LOTUSCRT_COMPILER_MSVC
	__asm__(
		// Capture general registers
		"movl %%eax, %0\n"
		"\tmovl %%ecx, %1\n"
		"\tmovl %%edx, %2\n"
		"\tmovl %%ebx, %3\n"
		"\tmovl %%esi, %4\n"
		"\tmovl %%edi, %5\n"
		"\tmovw %%ss, %6\n"
		"\tmovw %%cs, %7\n"
		"\tmovw %%ds, %8\n"
		"\tmovw %%es, %9\n"
		"\tmovw %%fs, %10\n"
		"\tmovw %%gs, %11\n"
		"\tpushfd\n"
		"\tpop %12\n"

		// Capture caller stack registers
		"\tmovl (%%ebp), %%eax\n"
		"\tmovl %%eax, %13\n"
		"\tmovl 4(%%ebp), %%eax\n"
		"\tmovl %%eax, %14\n"
		"\tlea 8(%%ebp), %%eax\n"
		"\tmovl %%eax, %15\n"

		// Reference dummy stack
		"\tmovl %16, %%eax"
		: "=m"(hardFailContext.Eax),
		"=m"(hardFailContext.Ecx),
		"=m"(hardFailContext.Edx),
		"=m"(hardFailContext.Ebx),
		"=m"(hardFailContext.Esi),
		"=m"(hardFailContext.Edi),
		"=m"(hardFailContext.SegSs),
		"=m"(hardFailContext.SegCs),
		"=m"(hardFailContext.SegDs),
		"=m"(hardFailContext.SegEs),
		"=m"(hardFailContext.SegFs),
		"=m"(hardFailContext.SegGs),
		"=m"(hardFailContext.EFlags),
		"=m"(hardFailContext.Ebp),
		"=m"(hardFailContext.Eip),
		"=m"(hardFailContext.Esp)
		: "m"(dummyStack) : "eax");
#else
	__asm
	{
		// Capture general registers
		mov dword ptr[hardFailContext.Eax], eax
		mov dword ptr[hardFailContext.Ecx], ecx
		mov dword ptr[hardFailContext.Edx], edx
		mov dword ptr[hardFailContext.Ebx], ebx
		mov dword ptr[hardFailContext.Esi], esi
		mov dword ptr[hardFailContext.Edi], edi
		mov word ptr[hardFailContext.SegSs], ss
		mov word ptr[hardFailContext.SegCs], cs
		mov word ptr[hardFailContext.SegDs], ds
		mov word ptr[hardFailContext.SegEs], es
		mov word ptr[hardFailContext.SegFs], fs
		mov word ptr[hardFailContext.SegGs], gs
		pushfd
		pop [hardFailContext.EFlags]

		// Capture caller stack registers
		mov eax, [ebp]
		mov dword ptr[hardFailContext.Ebp], eax
		mov eax, [ebp + 4]
		mov dword ptr[hardFailContext.Eip], eax
		lea eax, [ebp + 8]
		mov dword ptr[hardFailContext.Esp], eax

		// Reference dummy stack
		mov eax, dword ptr dummyStack
	}
#endif // __LOTUSCRT_COMPILER_MSVC
#endif // __LOTUSCRT_PLATFORM_WIN32

#ifdef __LOTUSCRT_PLATFORM_WIN64
	ULONG64 controlPC, imgBase, establisherFrame;
	PRUNTIME_FUNCTION fctEntry;
	PVOID hndData;

	RtlCaptureContext(&hardFailContext);
	controlPC = hardFailContext.Rip;
	fctEntry = RtlLookupFunctionEntry(controlPC, &imgBase, NULL);
	if(fctEntry != NULL)
	{
		RtlVirtualUnwind(
			UNW_FLAG_NHANDLER,
			imgBase,
			controlPC,
			fctEntry,
			&hardFailContext,
			&hndData,
			&establisherFrame,
			NULL);
	}
	else
#endif // __LOTUSCRT_PLATFORM_WIN64
	{
#if defined(__LOTUSCRT_ARCH_X64)
		hardFailContext.Rip = (DWORD64)(_ReturnAddress());
		hardFailContext.Rsp = (DWORD64)(_AddressOfReturnAddress()) + 8;
#elif defined(__LOTUSCRT_ARCH_X86)
		hardFailContext.Eip = (DWORD)(_ReturnAddress());
		hardFailContext.Esp = (DWORD)(_AddressOfReturnAddress()) + 4;
#elif defined(__LOTUSCRT_PROCESSOR_ARM)
		hardFailContext.Pc = (DWORD)(_ReturnAddress());
		hardFailContext.Sp = (DWORD)(_AddressOfReturnAddress()) + 4;
#endif // __LOTUSCRT_ARCH_X64
	}

#if defined(__LOTUSCRT_ARCH_X64)
	hardFailRecord.ExceptionAddress = (PVOID)hardFailContext.Rip;
	hardFailContext.Rcx = __useStackCookie ? __stackCookie : hardFailContext.Rcx;
#elif defined(__LOTUSCRT_ARCH_X86)
	hardFailRecord.ExceptionAddress = (PVOID)hardFailContext.Eip;
	hardFailContext.Ecx = __useStackCookie ? __stackCookie : hardFailContext.Ecx;
#elif defined(__LOTUSCRT_PROCESSOR_ARM)
	hardFailRecord.ExceptionAddress = (PVOID)hardFailContext.Pc;
	__LOTUSCRT_UNUSED(__useStackCookie);
	__LOTUSCRT_UNUSED(__stackCookie);
#endif // __LOTUSCRT_ARCH_X64
	hardFailRecord.ExceptionCode = __errorCode;
	hardFailRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

	cookie[0] = __security_cookie;
	cookie[1] = __security_cookie_complement;

	RtlSetUnhandledExceptionFilter(NULL);
	RtlUnhandledExceptionFilter((EXCEPTION_POINTERS *)&hardFailException);

	ZwTerminateProcess(NtCurrentProcess(), (NTSTATUS)__errorCode);
	__LOTUSCRT_UNREACHABLE;
}

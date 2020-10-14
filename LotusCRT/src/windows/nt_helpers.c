#include "nt_helpers.h"

#include <stdint.h>

#include <phnt_windows.h>
#include <phnt.h>

extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;

static EXCEPTION_RECORD hardFailRecord;
static CONTEXT hardFailContext;

static EXCEPTION_POINTERS hardFailException =
{
	&hardFailRecord,
	&hardFailContext
};

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

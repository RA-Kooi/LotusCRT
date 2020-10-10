// Explicitly disable CFG and attempt to indirect call a function to trigger it

#include <stdlib.h>

#include <phnt_windows.h>
#include <phnt.h>

void Foo(void);

#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(suppress))
#endif
__LOTUSCRT_FORCE_NOINLINE __LOTUSCRT_NORETURN void Foo(void)
{
	_Exit(1);
}

typedef struct _VM_INFORMATION
{
	DWORD numberOfOffsets;
	DWORD mustBeZero;
	PDWORD output;
	PCFG_CALL_TARGET_INFO callTargets;
	DWORD64 unknown1;
	DWORD64 unknown2;
} VM_INFORMATION, *PVM_INFORMATION;

int main(void)
{
#ifndef __LOTUSCRT_COMPILER_MSVC_ONLY
	PLIST_ENTRY const head = &NtCurrentPeb()->Ldr->InMemoryOrderModuleList;

	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
	PLDR_DATA_TABLE_ENTRY const entry = CONTAINING_RECORD(
		head->Flink,
		LDR_DATA_TABLE_ENTRY,
		InMemoryOrderLinks);
	__LOTUSCRT_RESTORE_CLANG_WARNING()

	CFG_CALL_TARGET_INFO callTargetInfo = { 0 };
	callTargetInfo.Flags = 0; // CFG call invalid
	void* base = NtCurrentPeb()->ImageBaseAddress;
	callTargetInfo.Offset = (ULONG_PTR)(Foo) - (ULONG_PTR)(base);

	MEMORY_RANGE_ENTRY memRange = { 0 };
	memRange.NumberOfBytes = entry->SizeOfImage;
	memRange.VirtualAddress = entry->DllBase;

	VM_INFORMATION vmInfo = { 0 };
	vmInfo.numberOfOffsets = 1;
	vmInfo.mustBeZero = 0;
	DWORD output = 0;
	vmInfo.output = &output;
	vmInfo.callTargets = &callTargetInfo;
	vmInfo.unknown1 = 0;
	vmInfo.unknown2 = 0;

	NTSTATUS status = ZwSetInformationVirtualMemory(
		NtCurrentProcess(),
		VmCfgCallTargetInformation,
		1,
		&memRange,
		&vmInfo,
		sizeof(vmInfo));

	if(!NT_SUCCESS(status))
		return 2;
#endif

	typedef void (*func_t)(void);
	volatile func_t func = &Foo; // Volatile so the indirection doesn't get
	// removed during release builds.

	func();

	return -1;
}

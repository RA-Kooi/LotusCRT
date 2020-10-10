#include "context_validation.h"

#include <stddef.h>

#include <phnt_windows.h>
#include <phnt.h>

#include "guard_support.h"

#if defined(__LOTUSCRT_ARCH_X64)
	#define __GET_STACK_POINTER(context) ((context)->Rsp)
#elif defined(__LOTUSCRT_ARCH_X86)
	#define __GET_STACK_POINTER(context) ((context)->Esp)
#else
	#error "Unsupported MSVC architecture"
#endif

static EXCEPTION_RECORD __contextValidationException;
static EXCEPTION_POINTERS __contextExceptionPointers =
{
	&__contextValidationException,
	NULL
};

void _Lotus_validate_context_record(PCONTEXT const __contextRecord)
{
	if(!_guard_icall_checks_enforced())
		return;

	PNT_TIB const tib = (PNT_TIB)NtCurrentTeb();
	PVOID const stackPointer = (PVOID)__GET_STACK_POINTER(__contextRecord);

	if((stackPointer < tib->StackLimit) || (stackPointer > tib->StackBase))
	{
		__contextValidationException.ExceptionAddress =
			(PVOID)__GET_STACK_POINTER(__contextRecord);

		__contextValidationException.ExceptionCode =
			(DWORD)STATUS_STACK_BUFFER_OVERRUN;

		__contextValidationException.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
		__contextExceptionPointers.ContextRecord = __contextRecord;

		RtlSetUnhandledExceptionFilter(NULL);
		RtlUnhandledExceptionFilter(&__contextExceptionPointers);

		ZwTerminateProcess(NtCurrentProcess(), STATUS_STACK_BUFFER_OVERRUN);
	}
}

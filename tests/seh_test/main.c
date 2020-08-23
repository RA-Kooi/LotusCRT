#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

static int Filter(
	DWORD const __exceptionCode,
	PEXCEPTION_POINTERS const __exceptionPointers)
{
	if((NTSTATUS)__exceptionCode == EXCEPTION_ACCESS_VIOLATION)
		__debugbreak();

	if(!((NTSTATUS)__exceptionPointers->ExceptionRecord->ExceptionFlags
	   & EXCEPTION_NONCONTINUABLE_EXCEPTION))
		return EXCEPTION_EXECUTE_HANDLER;

	PCONTEXT context = __exceptionPointers->ContextRecord;
#ifdef __LOTUSCRT_PLATFORM_WIN64
    context->Rip++;
#else
    context->Eip++;
#endif

	return EXCEPTION_CONTINUE_EXECUTION;
}

int main()
{
	__try
	{
		int *const a = (int *const)0x0BADFEED;
		*a = 7;
	}
	__except(Filter(_exception_code(), _exception_info()))
	{
		return 0;
	}

	return 1;
}

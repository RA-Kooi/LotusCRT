#include "windows/seh_filters.h"

#include <_lotuscrt.h>

ULONG NTAPI _Lotus_SEH_exe_filter(
	PEXCEPTION_POINTERS const __exceptionPointers)
{
	// TODO: Implement signal/raise
	// TODO: Get Per thread signal handler

	__LOTUSCRT_UNUSED(__exceptionPointers);

	return EXCEPTION_CONTINUE_SEARCH;
}

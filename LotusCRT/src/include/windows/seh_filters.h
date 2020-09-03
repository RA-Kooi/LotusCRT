#ifndef __LOTUSCRT_SEH_FILTERS_H__
#define __LOTUSCRT_SEH_FILTERS_H__

#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

__LOTUSCRT_BEGIN_C_HEADER

ULONG NTAPI _Lotus_SEH_exe_filter(
	PEXCEPTION_POINTERS const __exceptionPointers);

__LOTUSCRT_END_C_HEADER

#endif // __LOTUSCRT_SEH_FILTERS_H__

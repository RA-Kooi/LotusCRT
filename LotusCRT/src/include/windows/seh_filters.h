#ifndef __LOTUSCRT_SEH_FILTERS_H__
#define __LOTUSCRT_SEH_FILTERS_H__

#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

#ifdef __cplusplus
extern "C"
{
#endif

ULONG NTAPI _Lotus_SEH_exe_filter(
	PEXCEPTION_POINTERS const __exceptionPointers);

#ifdef __cplusplus
}
#endif

#endif // __LOTUSCRT_SEH_FILTERS_H__

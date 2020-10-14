#include "gs_support.h"

#include <_lotuscrt.h>

#include <stdint.h>
#include <stdbool.h>

#include <phnt_windows.h>
#include <phnt.h>

#include "nt_helpers.h"

#ifdef __LOTUSCRT_PLATFORM_WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DDFA232ll
#else
#define DEFAULT_SECURITY_COOKIE 0xBB40E64E
#endif

__LOTUSCRT_ALIGNAS(__LOTUSCRT_PACKING) typedef union
{
	UINT64 ft_scalar;
	FILETIME ft_struct;
} FT;

extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;

__LOTUSCRT_SELECTANY UINT_PTR __security_cookie = DEFAULT_SECURITY_COOKIE;
__LOTUSCRT_SELECTANY UINT_PTR __security_cookie_complement =
	(UINT_PTR)~(DEFAULT_SECURITY_COOKIE);

void __LOTUSCRT_CDECL __security_init_cookie(void)
{
	UINT_PTR cookie;
	FT systime = {0,};
	LARGE_INTEGER perfctr;

	if(__security_cookie != DEFAULT_SECURITY_COOKIE)
	{
		__security_cookie_complement = ~__security_cookie;
		return;
	}

	ZwQuerySystemTime((PLARGE_INTEGER)&systime.ft_scalar);
#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie = systime.ft_scalar;
#else
	cookie = systime.ft_struct.dwLowDateTime;
	cookie ^= systime.ft_struct.dwHighDateTime;
#endif
	cookie ^= (UINT_PTR)NtCurrentThreadId();
	cookie ^= (UINT_PTR)NtCurrentProcessId();

	cookie ^= (UINT_PTR)&cookie;

#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie ^= ((UINT_PTR)NtGetTickCount64()) << 56;
#else
	cookie ^= (UINT_PTR)NtGetTickCount64();
#endif

	ZwQueryPerformanceCounter(&perfctr, NULL);
#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie ^= (UINT_PTR)perfctr.QuadPart;
	cookie ^= (UINT_PTR)(((UINT64)perfctr.LowPart) << 32);
#else
	cookie ^= (UINT_PTR)perfctr.LowPart;
	cookie ^= (UINT_PTR)perfctr.HighPart;
#endif

#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie &= 0x0000FFFFFFFFFFFFll;
#endif

	if(cookie == DEFAULT_SECURITY_COOKIE)
		cookie = DEFAULT_SECURITY_COOKIE + 1;
#ifdef __LOTUSCRT_PLATFORM_WIN32
	else if((cookie & 0xFFFF0000) == 0)
		cookie |= (cookie | 0x4711) << 16;
#endif

	__security_cookie = cookie;
	__security_cookie_complement = ~__security_cookie;
}

__LOTUSCRT_NORETURN void __LOTUSCRT_CDECL __report_gsfailure(ULONG_PTR);

__LOTUSCRT_NORETURN void __LOTUSCRT_CDECL __report_gsfailure(
	ULONG_PTR __stackCookie)
{
	_Lotus_raise_hard_error(
		(uint32_t)STATUS_STACK_BUFFER_OVERRUN,
		true,
		__stackCookie);
}

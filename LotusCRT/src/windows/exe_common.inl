#include <_lotuscrt.h>

// Windows SDK uses wchar functions in C++, but doesn't include the header
#include <wchar.h>
#include <phnt_windows.h>
#include <phnt.h>

#include "crt_init.h"
#include "gs_support.h"
#include "nt_helpers.h"
#include "seh_filters.h"
#include "vc_sections.h"


extern "C" int __LOTUSCRT_CDECL main(int argc, char **argv, char **envp);
extern "C" int __LOTUSCRT_CDECL wmain(int argc, wchar_t **argv, wchar_t **envp);

static __LOTUSCRT_FORCE_NOINLINE int _Lotus_common_main_seh()
{
	if(!_LotusCRT_init())
		_Lotus_raise_hard_error(uint32_t(STATUS_INTERNAL_ERROR), false, 0);

	// Instead of using SEH here, I opt for the unhandled exception filter.
	// This is better as clang and GCC do not properly support SEH.
	// Clang will at least eat the SEH syntax, but GCC won't.
	// Less ifdef hell overall.
	RtlSetUnhandledExceptionFilter(_Lotus_SEH_exe_filter);

	if(_Lotus_do_global_constructors(__xi_a, __xi_z) != 0)
		return 255;

#ifdef __LOTUS_CRT_MAIN__
	return main(0, nullptr, nullptr);
#elif defined(__LOTUS_CRT_WMAIN__)
	return wmain(0, nullptr, nullptr);
#endif
}

static inline __LOTUSCRT_FORCE_INLINE int _Lotus_common_main()
{
	__security_init_cookie();

	return _Lotus_common_main_seh();
}

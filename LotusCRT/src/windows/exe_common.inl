#include <_lotuscrt.h>

#include <stdint.h>

// Windows SDK uses wchar functions in C++, but doesn't include the header
#include <wchar.h>
#include <phnt_windows.h>
#include <phnt.h>

#include "crt_init.h"
#include "gs_support.h"
#include "nt_helpers.h"
#include "seh_filters.h"
#include "vc_sections.h"

#ifdef __LOTUSCRT_PLATFORM_WIN64
extern "C" EXCEPTION_DISPOSITION __C_specific_handler(
	PEXCEPTION_RECORD __exceptionRecord,
	void *__establisherFrame,
	PCONTEXT __contextRecord,
	PDISPATCHER_CONTEXT __dispatcherContext);
#else
extern "C" EXCEPTION_DISPOSITION
#ifndef __LOTUSCRT_AS_DLL
_except_handler4(
#else
_except_handler4_common(
	PUINT_PTR __cookie,
	PCOOKIE_CHECK __cookieCheckFunc
#endif
	PEXCEPTION_RECORD __exceptionRecord,
	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
	PCONTEXT __contextRecord,
	PVOID __dispatcherContext);
#endif

extern "C" int __LOTUSCRT_CDECL main(int argc, char **argv, char **envp);
extern "C" int __LOTUSCRT_CDECL wmain(int argc, wchar_t **argv, wchar_t **envp);

static __LOTUSCRT_FORCE_NOINLINE int _Lotus_common_main_seh()
{
#ifndef __LOTUSCRT_COMPILER_MSVC_ONLY
	constexpr uint32_t const functionCount = 1;
	void *toProtect[functionCount] =
	{
#ifdef __LOTUSCRT_PLATFORM_WIN64
		reinterpret_cast<void*>(&__C_specific_handler),
#else
	#ifndef __LOTUSCRT_AS_DLL
		reinterpret_cast<void*>(&_except_handler4),
	#else
		reinterpret_cast<void*>(&_except_handler4_common),
	#endif // __LOTUSCRT_AS_DLL
#endif
	};

	uint32_t const protectedFuncs = _Lotus_protect_functions(
		reinterpret_cast<void**>(&toProtect),
		functionCount);

	if(protectedFuncs != functionCount)
		_Lotus_raise_hard_error(uint32_t(STATUS_INVALID_USER_BUFFER), false, 0);
#endif

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

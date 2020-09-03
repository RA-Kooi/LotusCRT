#ifndef __LOTUSCRT_STDDEF_H__
#define __LOTUSCRT_STDDEF_H__

#include <_lotuscrt.h>

__LOTUSCRT_SYSTEM_HEADER

#if defined(__LOTUSCRT_COMPILER_GNULIKE) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
#include_next <stddef.h>
#elif defined(__LOTUSCRT_COMPILER_MSVC)

__LOTUSCRT_BEGIN_C_HEADER

// wchar_t
#ifndef __cplusplus
	// If MSVC /Zc:wchar_t is active, don't define wchar_t
	#if !((defined(_WCHAR_T_DEFINED) && _WCHAR_T_DEFINED == 1) \
		  || defined(_NATIVE_WCHAR_T_DEFINED))
		typedef unsigned short wchar_t;
	#endif
#endif // __cplusplus

// size_t
#ifdef __LOTUSCRT_ARCH_X86
	typedef unsigned __int32 size_t;
#elif defined(__LOTUSCRT_ARCH_X64)
	typedef unsigned __int64 size_t;
#else
	#error "Unsupported MSVC platform"
#endif

#ifdef __cplusplus
	#define NULL nullptr
#else
	#define NULL ((void*)0)
#endif

__LOTUSCRT_END_C_HEADER

#else
#error "Unsupported compiler"
#endif

#endif // __LOTUSCRT_STDDEF_H__

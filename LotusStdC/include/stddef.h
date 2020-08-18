#ifndef __LOTUSCRT_STDDEF_H__
#define __LOTUSCRT_STDDEF_H__

#include <_lotuscrt.h>

__LOTUSCRT_SYSTEM_HEADER

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// wchar_t
#ifndef __cplusplus
	#ifdef __LOTUSCRT_PLATFORM_WINDOWS
		// If MSVC /Zc:wchar_t is active, don't define wchar_t
		#if !((defined(_WCHAR_T_DEFINED) && _WCHAR_T_DEFINED == 1) \
			  || defined(_NATIVE_WCHAR_T_DEFINED))
			typedef unsigned short wchar_t;
		#endif
	#else
		typedef unsigned int wchar_t;
	#endif
#endif // __cplusplus

// size_t
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
		|| defined(__LOTUSCRT_COMPILER_GNULIKE)
	typedef __SIZE_TYPE__ size_t;
#elif defined(__LOTUSCRT_COMPILER_MSVC)
	#ifdef __LOTUSCRT_ARCH_X86
		typedef unsigned __int32 size_t;
	#elif __LOTUSCRT_ARCH_X64
		typedef unsigned __int64 size_t;
	#else
		#error "Unsupported MSVC platform"
	#endif
#else
	#error "Unsupported platform/compiler"
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LOTUSCRT_STDDEF_H__

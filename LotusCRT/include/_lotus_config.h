#ifndef __LOTUSCRT_CONFIG_H__
#define __LOTUSCRT_CONFIG_H__

#include <_lotus_compiler.h>

__LOTUSCRT_SYSTEM_HEADER

#define __LOTUSCRT_PACKING 8

// C language version convenience macro
#ifdef __STDC_VERSION__
	#if __STDC_VERSION__ == 201112L
		#define __LOTUSCRT_C_VERSION 11
	#elif __STDC_VERSION__ == 199901L
		#define __LOTUSCRT_C_VERSION 99
	#elif __STDC_VERSION__ == 199409L
		#define __LOTUSCRT_C_VERSION 94
	#endif
#else // __STDC_VERSION__
	// Assume C90
	#define __LOTUSCRT_C_VERSION 90
#endif // __STDC_VERSION__

#if defined(_DLL) || defined(__LOTUSCRT_AS_DLL)
	#ifdef _DLL
		#ifdef __LOTUSCRT_PLATFORM_WINDOWS
			#define __LOTUSCRT_API __declspec(dllimport)
			#define __LOTUSCRT_LOCAL
		#else // ^ Linking DLL on windows | v Linking SO on linux
			#define __LOTUSCRT_API
			#define __LOTUSCRT_LOCAL
		#endif // __LOTUSCRT_PLATFORM_WINDOWS
	#else
		#ifdef __LOTUSCRT_PLATFORM_WINDOWS
			#define __LOTUSCRT_API __declspec(dllexport)
			#define __LOTUSCRT_LOCAL
		#elif (defined(__LOTUSCRT_COMPILER_GNULIKE) \
			   && __LOTUSCRT_COMPILER_GNULIKE_VERSION >= 400)
			// GCC/clang on Linux
			#define __LOTUSCRT_API __attribute__((visibility("default")))
			#define __LOTUSCRT_LOCAL __attribute__((visibility("hidden")))
		#else
			#define __LOTUSCRT_API
			#define __LOTUSCRT_LOCAL
		#endif // __LOTUSCRT_PLATFORM_WINDOWS
	#endif // _DLL
#else
	#define __LOTUSCRT_API
	#define __LOTUSCRT_LOCAL
#endif // _DLL || __LOTUSCRT_AS_DLL

#ifdef __cplusplus
#define __LOTUSCRT_BEGIN_C_HEADER extern "C" {
#define __LOTUSCRT_END_C_HEADER }
#else
#define __LOTUSCRT_BEGIN_C_HEADER
#define __LOTUSCRT_END_C_HEADER
#endif

#endif //__LOTUSCRT_CONFIG_H__

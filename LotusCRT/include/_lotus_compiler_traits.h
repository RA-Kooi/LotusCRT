#ifndef __LOTUSCRT_COMPILER_TRAITS_H__
#define __LOTUSCRT_COMPILER_TRAITS_H__

#include <_lotus_compiler.h>
#include <_lotus_config.h>
#include <_lotus_platform.h>

__LOTUSCRT_SYSTEM_HEADER

// __LOTUSCRT_DISABLE_VC_WARNING
#ifdef __LOTUSCRT_COMPILER_MSVC
	#define __LOTUSCRT_DISABLE_VC_WARNING(w) \
		__pragma(warning(push)) \
		__pragma(warning(disable:w))
#else
	#define __LOTUSCRT_DISABLE_VC_WARNING(w)
#endif

// __LOTUSCRT_RESTORE_VC_WARNING
#ifdef __LOTUSCRT_COMPILER_MSVC
	#define __LOTUSCRT_RESTORE_VC_WARNING() __pragma(warning(pop))
#else
	#define __LOTUSCRT_RESTORE_VC_WARNING()
#endif

// __LOTUSCRT_DISABLE_CLANG_WARNING
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_CLANG)
	#define __LOTUSCRT_CLANG_WARNING_HELP0(x) #x
	#define __LOTUSCRT_CLANG_WARNING_HELP1(x) __LOTUSCRT_CLANG_WARNING_HELP0(clang diagnostic ignored x)
	#define __LOTUSCRT_CLANG_WARNING_HELP2(x) __LOTUSCRT_CLANG_WARNING_HELP1(#x)

	#define __LOTUSCRT_DISABLE_CLANG_WARNING(w) \
		_Pragma("clang diagnostic push") \
		_Pragma(__LOTUSCRT_CLANG_WARNING_HELP2(w))
#else
	#define __LOTUSCRT_DISABLE_CLANG_WARNING(w)
#endif

// __LOTUSCRT_RESTORE_CLANG_WARNING
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_CLANG)
	#define __LOTUSCRT_RESTORE_CLANG_WARNING() _Pragma("clang diagnostic pop")
#else
	#define __LOTUSCRT_RESTORE_CLANG_WARNING()
#endif

// __LOTUSCRT_NORETURN
#if __LOTUSCRT_C_VERSION >= 11 && !defined(__LOTUSCRT_COMPILER_MSVC)
	#define __LOTUSCRT_NORETURN _Noreturn
#elif !defined(__LOTUSCRT_COMPILER_MSVC)
	#define __LOTUSCRT_NORETURN __attribute__(noreturn)
#else
	#define __LOTUSCRT_NORETURN __declspec(noreturn)
#endif

// __LOTUSCRT_ALIGNAS
#if __LOTUSCRT_C_VERSION >= 11 && !defined(__LOTUSCRT_COMPILER_MSVC)
	#define __LOTUSCRT_ALIGNAS(x) _Alignas(x)
#elif !defined(__LOTUSCRT_COMPILER_MSVC)
	#define __LOTUSCRT_ALIGNAS __attribute__(aligned(x))
#else
	#define __LOTUSCRT_ALIGNAS(x) __declspec(align(x))
#endif

// __LOTUSCRT_FORCE_INLINE
#ifdef __LOTUSCRT_COMPILER_MSVC
	#define __LOTUSCRT_FORCE_INLINE __forceinline
#elif defined(__LOTUSCRT_COMPILER_GNULIKE)
	#define __LOTUSCRT_FORCE_INLINE __attribute__((always_inline))
#else
	#define __LOTUSCRT_FORCE_INLINE
#endif

// __LOTUSCRT_FORCE_NOINLINE
#ifdef __LOTUSCRT_COMPILER_MSVC
	#define __LOTUSCRT_FORCE_NOINLINE __declspec(noinline)
#elif defined(__LOTUSCRT_COMPILER_GNULIKE)
	#define __LOTUSCRT_FORCE_NOINLINE __attribute__((noinline))
#else
	#define __LOTUSCRT_FORCE_NOINLINE
#endif

// __LOTUSCRT_UNUSED
#define __LOTUSCRT_UNUSED(x) (void)x

// __LOTUSCRT_SELECTANY
#ifdef __LOTUSCRT_PLATFORM_WINDOWS
	#ifdef __LOTUSCRT_COMPILER_MSVC
		#define __LOTUSCRT_SELECTANY __declspec(selectany)
	#elif defined(__LOTUSCRT_COMPILER_GNULIKE)
		#define __LOTUSCRT_SELECTANY __attribute__((selectany))
	#else
		#define __LOTUSCRT_SELECTANY
	#endif
#endif

// __LOTUSCRT_UNREACHABLE
#if defined(__LOTUSCRT_COMPILER_MSVC)
	#if !defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
		#define __LOTUSCRT_UNREACHABLE __assume(0)
	#else
		#define __LOTUSCRT_UNREACHABLE __builtin_unreachable()
	#endif
#elif defined(__LOTUSCRT_COMPILER_GNULIKE)
	#define __LOTUSCRT_UNREACHABLE __builtin_unreachable()
#else
	#define __LOTUSCRT_UNREACHABLE
#endif

// __LOTUSCRT_MSVC_INTRINSIC
#if defined(__LOTUSCRT_COMPILER_MSVC)
	#ifdef __LOTUSCRT_COMPILER_MSVC_CLANG
		#define __LOTUSCRT_MSVC_INTRINSIC_HELPER0(x) #x
		#define __LOTUSCRT_MSVC_INTRINSIC_HELPER1(x) \
			__LOTUSCRT_MSVC_INTRINSIC_HELPER0(intrinsic(x))

		#define __LOTUSCRT_MSVC_INTRINSIC(x) \
			_Pragma(__LOTUSCRT_MSVC_INTRINSIC_HELPER1(x))
	#else
		#define __LOTUSCRT_MSVC_INTRINSIC(x) __pragma intrinsic(x)
	#endif
#else
	#define __LOTUSCRT_MSVC_INTRINSIC(x)
#endif

// __LOTUSCRT_MSVC_FUNCTION
#if defined(__LOTUSCRT_COMPILER_MSVC) \
	&& !defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
	#define __LOTUSCRT_MSVC_FUNCTION(x) __pragma function(x)
#else
	#define __LOTUSCRT_MSVC_FUNCTION(x)
#endif

// __LOTUSCRT_NO_BUILTIN
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_CLANG)
	#define __LOTUSCRT_NO_BUILTIN(x) __attribute__((no_builtin(#x)))
#else
	#define __LOTUSCRT_NO_BUILTIN(x)
#endif

// __LOTUSCRT_ENABLE_ARCH
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_GNULIKE)
	#define __LOTUSCRT_ENABLE_ARCH(x) __attribute__((target(#x)))
#else
	#define __LOTUSCRT_ENABLE_ARCH(x)
#endif

// __LOTUSCRT_FALLTHROUGH
#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_GNULIKE)
	#define __LOTUSCRT_FALLTHROUGH __attribute__((fallthrough))
#else
	#define __LOTUSCRT_FALLTHROUGH
#endif

#endif // __LOTUSCRT_COMPILER_TRAITS_H__

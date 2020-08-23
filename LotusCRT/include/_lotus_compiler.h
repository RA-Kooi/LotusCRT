#ifndef __LOTUSCRT_COMPILER_H__
#define __LOTUSCRT_COMPILER_H__

// Compiler abstraction and attributes
#if defined(__GNUC__) && !defined(__clang__)
	#define __LOTUSCRT_COMPILER_GNU
	#define __LOTUSCRT_COMPILER_GNULIKE
	#define __LOTUSCRT_COMPILER_VERSION (__GNUC__ * 100) \
		+ (__GNUC_MINOR__ * 10) \
		+ (__GNUC_PATCHLEVEL__)
	#define __LOTUSCRT_COMPILER_GNULIKE_VERSION __LOTUSCRT_COMPILER_VERSION

	#define __LOTUSCRT_CDECL_SUPPORTED

	#define __LOTUSCRT_ADDRESSOF(x) __builtin_addressof(x)
#elif defined(__clang__) \
	&& !defined(__apple_build_version__) \
	&& !defined(_MSC_VER)
	#define __LOTUSCRT_COMPILER_LLVM_CLANG
	#define __LOTUSCRT_COMPILER_GNULIKE
	#define __LOTUSCRT_COMPILER_VERSION (__clang_major__ * 100) \
		+ (__clang_minor__ * 10) \
		+ (__clang_patchlevel__)
	#define __LOTUSCRT_COMPILER_GNULIKE_VERSION (__GNUC__ * 100) \
		+ (__GNUC_MINOR__ * 10) \
		+ (__GNUC_PATCHLEVEL__)

	#define __LOTUSCRT_CDECL_SUPPORTED

	#define __LOTUSCRT_ADDRESSOF(x) __builtin_addressof(x)
#elif defined(__clang__) && defined(__apple_build_version__)
	#error "Apple is not supported."
#elif defined(_MSC_VER)
	#define __LOTUSCRT_COMPILER_MSVC
	#define __LOTUSCRT_COMPILER_VERSION _MSC_VER

	#ifdef __clang__
		#define __LOTUSCRT_COMPILER_MSVC_CLANG
		#define __LOTUSCRT_COMPILER_MSVC_CLANG_VERSION \
			(__clang_major__ * 100) \
			+ (__clang_minor__ * 10) \
			+ (__clang_patchlevel__)
	#else
		#define __LOTUSCRT_COMPILER_MSVC_ONLY
	#endif

	#define __LOTUSCRT_CDECL_SUPPORTED

	#define __LOTUSCRT_ADDRESSOF(x) __builtin_addressof(x)
#else // ^ Supported compilers v Unsupported compilers
	#error "Unknown compiler, please add it if you wish to use it."
#endif // Compiler abstraction

// Check support for system header annotation
#ifndef _CRTBLD
	#if defined(__LOTUSCRT_COMPILER_GNULIKE)
		#define __LOTUSCRT_SYSTEM_HEADER _Pragma("GCC system_header")
	#elif defined(__LOTUSCRT_COMPILER_MSVC)
		#ifdef __LOTUSCRT_COMPILER_MSVC_CLANG
			#define __LOTUSCRT_SYSTEM_HEADER _Pragma("clang system_header")
		#else
			#define __LOTUSCRT_SYSTEM_HEADER __pragma(system_header)
		#endif // __LOTUSCRT_COMPILER_MSVC_CLANG
	#else // ^ Compilers that support system header annotation | v No support
		#define __LOTUSCRT_SYSTEM_HEADER
	#endif // Support for system header annotation
#else
	#define __LOTUSCRT_SYSTEM_HEADER
#endif

// Check for cdeclare support
#ifdef __LOTUSCRT_CDECL_SUPPORTED
	#define __LOTUSCRT_CDECL __cdecl
#else
	#warning "Compiler doesn't seem to support __cdecl"
	#define __LOTUSCRT_CDECL
#endif // __LOTUSCRT_CDECL_SUPPORTED

#endif // __LOTUSCRT_COMPILER_H__

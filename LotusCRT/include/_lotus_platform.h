#ifndef __LOTUSCRT_PLATFORM_H__
#define __LOTUSCRT_PLATFORM_H__

#if defined(__x86_64__) || defined(_M_X64)
	#define __LOTUSCRT_PROCESSOR_INTEL_AMD
	#define __LOTUSCRT_ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
	#define __LOTUSCRT_PROCESSOR_INTEL_AMD
	#define __LOTUSCRT_ARCH_X86
#elif defined(__arm__)
	#error "ARM is not yet supported."
	#define __LOTUSCRT_PROCESSOR_ARM
	#define __LOTUSCRT_ARCH_ARM32
#elif defined(__aarch64__) || defined(__AARCH64)
	#error "ARM is not yet supported."
	#define __LOTUSCRT_PROCESSOR_ARM
	#define __LOTUSCRT_ARCH_ARN64
#elif defined(__powerpc__) || defined(__POWERPC__)
	#error "PowerPC is not yet supported."
	#define __LOTUSCRT_PROCESSOR_POWERPC
	#define __LOTUSCRT_ARCH_POWERPC32
#elif defined(__powerpc64__) || defined(__POWERPC64__)
	#error "PowerPC is not yet supported."
	#define __LOTUSCRT_PROCESSOR_POWERPC
	#define __LOTUSCRT_ARCH_POWERPC64
#else
	#warning "Unknown architecture"
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
	#define __LOTUSCRT_PLATFORM_MINGW
#endif

#if defined(__ANDROID__)
	#error "Android is not yet supported."
	#define __LOTUSCRT_PLATFORM_ANDROID
#endif

#if defined(_WIN32)
	#define __LOTUSCRT_PLATFORM_WINDOWS
	#if defined(_WIN64)
		#define __LOTUSCRT_PLATFORM_WIN64
	#endif
#elif defined(__linux__)
	#define __LOTUSCRT_PLATFORM_LINUX
#elif defined(__bsdi__)
	#error "BSD is not yet supported"
	#define __LOTUSCRT_PLATFORM_BSD
#else
	#error "Platform not yet supported"
#endif

#endif // __LOTUSCRT_PLATFORM_H__

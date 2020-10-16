#include <string.h>

#include "simd_support.h"

#ifdef __LOTUSCRT_COMPILER_MSVC
	// Allow use of SIMD intrinsics without generating SIMD in the whole CRT
	#ifndef __SSE2__
		#define __SSE2__
	#endif
	#ifndef __SSE4_2__
		#define __SSE4_2__
	#endif
	#ifndef __AVX__
		#define __AVX__
	#endif
	#ifndef __AVX2__
		#define __AVX2__
	#endif
	#ifndef __AVX512F__
		#define __AVX512F__
	#endif
	#define __MM_MALLOC_H
#endif
#include <immintrin.h>

__LOTUSCRT_NO_BUILTIN(memset)
static void _Lotus_memset_no_accel(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	unsigned char value = (unsigned char)__value;

	while(__count > 0)
	{
		switch(__count)
		{
			default:
			case 8: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 7: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 6: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 5: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 4: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 3: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 2: *dest++ = value;
			__LOTUSCRT_FALLTHROUGH;
			case 1: *dest++ = value;
		}

		__count -= __count >= 8 ? 8 : __count % 8;
	}
}

#if !defined(__LOTUSCRT_COMPILER_MSVC) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
__LOTUSCRT_ENABLE_ARCH(mmx)
static void _Lotus_memset_MMX(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	for(size_t i = 0; i < __count; i += 8)
	{
		__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
		*(__m64*)(dest + i) = _mm_set1_pi8((char)__value);
		__LOTUSCRT_RESTORE_CLANG_WARNING()
	}
}
#endif

__LOTUSCRT_ENABLE_ARCH(sse2)
static void _Lotus_memset_SSE(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	for(size_t i = 0; i < __count; i += 16)
	{
		__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
		_mm_storeu_si128((__m128i*)(dest + i), _mm_set1_epi8((char)__value));
		__LOTUSCRT_RESTORE_CLANG_WARNING()
	}
}

__LOTUSCRT_ENABLE_ARCH(avx)
static void _Lotus_memset_AVX(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	for(size_t i = 0; i < __count; i += 32)
	{
		__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
		_mm256_storeu_si256(
			(__m256i*)(dest + i),
			_mm256_set1_epi8((char)__value));
		__LOTUSCRT_RESTORE_CLANG_WARNING()
	}
}

__LOTUSCRT_ENABLE_ARCH(avx512f)
static void _Lotus_memset_AVX512(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	for(size_t i = 0; i < __count; i += 32)
	{
		__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
		_mm512_storeu_si512(
			(__m512i*)(dest + i),
			_mm512_set1_epi8((char)__value));
		__LOTUSCRT_RESTORE_CLANG_WARNING()
	}
}

__LOTUSCRT_MSVC_FUNCTION(memset)
__LOTUSCRT_API void * __LOTUSCRT_CDECL memset(
	void *__dest,
	int __value,
	size_t __count)
{
	size_t left;

	if(__count == 0)
		return __dest;

	// MMX is not supported, just do it the old fashioned way.
	if(__LOTUSCRT_UNLIKELY(!_Lotus_cpuflag_supported(__LOTUSCRT_CPUFLAG_MMX)))
	{
		_Lotus_memset_no_accel(__dest, __value, __count);
		return __dest;
	}

	if(__count < 8)
	{
		_Lotus_memset_no_accel(__dest, __value, __count);
		return __dest;
	}

	// MMX is available here
	if(__count < 16
	   || __LOTUSCRT_UNLIKELY(
			!_Lotus_cpuflag_supported(__LOTUSCRT_CPUFLAG_SSE2)))
	{
#if !defined(__LOTUSCRT_COMPILER_MSVC) \
		|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
		left = __count % 8;
		_Lotus_memset_MMX(__dest, __value, __count - left);
		memset((unsigned char*)__dest + __count - left, __value, left);
#else
		_Lotus_memset_no_accel(__dest, __value, __count);
#endif

		return __dest;
	}

	if(__count < 32 || !_Lotus_cpuflag_supported(__LOTUSCRT_CPUFLAG_AVX))
	{
		left = __count % 16;
		_Lotus_memset_SSE(__dest, __value, __count - left);
		memset((unsigned char*)__dest + __count - left, __value, left);
		return __dest;
	}

	if(__count < 64 || !_Lotus_cpuflag_supported(__LOTUSCRT_CPUFLAG_AVX512F))
	{
		left = __count % 32;
		_Lotus_memset_AVX(__dest, __value, __count - left);
		memset((unsigned char*)__dest + __count - left, __value, left);
	}

	left = __count % 64;
	_Lotus_memset_AVX512(__dest, __value, __count - left);
	memset((unsigned char*)__dest + __count - left, __value, left);

	return __dest;
}

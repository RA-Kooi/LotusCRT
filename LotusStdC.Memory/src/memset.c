#include <string.h>

#ifdef __LOTUSCRT_COMPILER_MSVC
	// Allow use of AVX intrinsics without generating AVX in the whole CRT
	#define __AVX__
	#define __MM_MALLOC_H
#endif
#include <immintrin.h>

__LOTUSCRT_NO_BUILTIN(memset)
static void _Lotus_memset_small(void *__dest, int __value, size_t __count)
{
	unsigned char *dest = __dest;
	unsigned char value = (unsigned char)__value;

	switch(__count)
	{
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
}

#if !defined(__LOTUSCRT_COMPILER_MSVC) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
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

// TODO: CPUID check
__LOTUSCRT_MSVC_FUNCTION(memset)
__LOTUSCRT_API void * __LOTUSCRT_CDECL memset(
	void *__dest,
	int __value,
	size_t __count)
{
	size_t left;

	if(__count == 0)
		return __dest;

	if(__count < 8)
	{
		_Lotus_memset_small(__dest, __value, __count);
		return __dest;
	}

	if(__count < 16)
	{
#if !defined(__LOTUSCRT_COMPILER_MSVC) \
		|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
		left = __count % 8;
		_Lotus_memset_MMX(__dest, __value, __count - left);
		_Lotus_memset_small(
			(unsigned char*)__dest + __count - left,
			__value,
			left);
#else
		_Lotus_memset_small(__dest, __value, 8);
		_Lotus_memset_small((unsigned char*)__dest + 8, __value, __count - 8);
#endif

		return __dest;
	}

	if(__count < 32)
	{
		left = __count % 16;
		_Lotus_memset_SSE(__dest, __value, __count - left);
		memset((unsigned char*)__dest + __count - left, __value, left);
		return __dest;
	}

	left = __count % 32;
	_Lotus_memset_AVX(__dest, __value, __count - left);
	memset((unsigned char*)__dest + __count - left, __value, left);

	return __dest;
}

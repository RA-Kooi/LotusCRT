#include "simd_support.h"

#include <stdint.h>

#if defined(__LOTUSCRT_COMPILER_MSVC) && defined(__LOTUSCRT_ARCH_X64)
	#define __MM_MALLOC_H
#endif
#include <intrin.h>
#include <immintrin.h>

static uint32_t __Lotus_cpuflags;

bool _Lotus_cpuflag_supported(uint32_t __flags)
{
	return (bool)(__Lotus_cpuflags & __flags);
}

void _Lotus_init_simd(void)
{
	__Lotus_cpuflags = 0;

	int abcd[4] = {0, 0, 0, 0};

	// Get the highest supported level the CPU supports.
	__cpuid(abcd, 0);

	// If this is true, the CPU supports no further querying, we can also
	// deduce that to mean no form of SIMD is supported.
	if(abcd[0] == 0)
		return;

	// Query most SIMD feature flags
	__cpuid(abcd, 1);

	// Check for MMX support
	if(!(abcd[3] & (1 << 23)))
		return;

	__Lotus_cpuflags |= __LOTUSCRT_CPUFLAG_MMX;

	// Check for SSE2 support
	if(!(abcd[3] & (1 << 26)))
		return;

	__Lotus_cpuflags |= __LOTUSCRT_CPUFLAG_SSE2;

	// Check for XSAVE (required for checking AVX support)
	if(!(abcd[2] & (1 << 27)))
		return;

	// Check if AVX is enabled in the OS
	if((_xgetbv(0) & 6) != 6)
		return;

	// Check if AVX is supported
	if(!(abcd[2] & (1 << 28)))
		return;

	__Lotus_cpuflags |= __LOTUSCRT_CPUFLAG_AVX;
}

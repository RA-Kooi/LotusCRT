#ifndef __LOTUSCRT_SIMD_SUPPORT_H__
#define __LOTUSCRT_SIMD_SUPPORT_H__

#include <_lotuscrt.h>

#include <stdbool.h>
#include <stdint.h>

__LOTUSCRT_BEGIN_C_HEADER

#define __LOTUSCRT_CPUFLAG_MMX  1 << 0
#define __LOTUSCRT_CPUFLAG_SSE2 1 << 1
#define __LOTUSCRT_CPUFLAG_AVX  1 << 2

bool _Lotus_cpuflag_supported(uint32_t __flags);

void _Lotus_init_simd(void);

__LOTUSCRT_END_C_HEADER

#endif // __LOTUSCRT_SIMD_SUPPORT_H__

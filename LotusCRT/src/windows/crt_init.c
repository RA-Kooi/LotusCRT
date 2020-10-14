#include "crt_init.h"

#include "simd_support.h"

bool _LotusCRT_init(void)
{
	_Lotus_init_simd();

	return true;
}

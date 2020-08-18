#include "vc_sections.h"

#include <stddef.h>

__LOTUSCRT_SECTION(.CRT$XIA) _PIFV __xi_a[] = { NULL };
__LOTUSCRT_SECTION(.CRT$XIZ) _PIFV __xi_z[] = { NULL };

#ifdef __LOTUSCRT_COMPILER_MSVC
#pragma comment(linker, "/merge:.CRT=.rdata")
#endif

int _Lotus_do_global_constructors(_PIFV *__begin, _PIFV *__end)
{
	int ret = 0;

	while(__begin < __end && ret == 0)
	{
		if(*__begin != NULL)
			ret = (**__begin)();

		++__begin;
	}

	return ret;
}

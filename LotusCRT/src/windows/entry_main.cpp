#include <_lotus_compiler.h>

#define __LOTUS_CRT_MAIN__
#include "exe_common.inl"

#if !defined(__LOTUSCRT_COMPILER_MSVC) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
extern "C" int mainCRTStartup();
#endif

extern "C" int mainCRTStartup()
{
	return _Lotus_common_main();
}

#define __LOTUS_CRT_WMAIN__
#include "exe_common.inl"

#if !defined(__LOTUSCRT_COMPILER_MSVC) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
extern "C" int wmainCRTStartup();
#endif

extern "C" int wmainCRTStartup()
{
	return _Lotus_common_main();
}

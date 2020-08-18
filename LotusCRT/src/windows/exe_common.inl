#include <_lotuscrt.h>

#include "windows/gs_support.h"
#include "windows/vc_sections.h"

extern "C" int __LOTUSCRT_CDECL main(int argc, char **argv, char **envp);
extern "C" int __LOTUSCRT_CDECL wmain(int argc, wchar_t **argv, wchar_t **envp);

static __LOTUSCRT_FORCE_NOINLINE int _Lotus_common_main_seh()
{
	if(_Lotus_do_global_constructors(__xi_a, __xi_z) != 0)
		return 255;

#ifdef __LOTUS_CRT_MAIN__
	return main(0, nullptr, nullptr);
#elif defined(__LOTUS_CRT_WMAIN__)
	return wmain(0, nullptr, nullptr);
#endif
}

static inline __LOTUSCRT_FORCE_INLINE int _Lotus_common_main()
{
	__security_init_cookie();

	return _Lotus_common_main_seh();
}

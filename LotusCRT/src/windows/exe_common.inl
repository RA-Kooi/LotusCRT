#include <_lotuscrt.h>

extern "C" int __LOTUSCRT_CDECL main(int argc, char **argv, char **envp);
extern "C" int __LOTUSCRT_CDECL wmain(int argc, wchar_t **argv, wchar_t **envp);

static __LOTUSCRT_FORCE_NOINLINE int _Lotus_common_main_seh()
{
#ifdef __LOTUS_CRT_MAIN__
	return main(0, nullptr, nullptr);
#elif defined(__LOTUS_CRT_WMAIN__)
	return wmain(0, nullptr, nullptr);
#endif
}

static inline __LOTUSCRT_FORCE_INLINE int _Lotus_common_main()
{
	return _Lotus_common_main_seh();
}

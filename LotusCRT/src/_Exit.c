#include <stdlib.h>

#ifdef __LOTUSCRT_PLATFORM_WINDOWS
#include <phnt_windows.h>
#include <phnt.h>
#endif

__LOTUSCRT_NORETURN void __LOTUSCRT_CDECL _Exit(int __exit_code)
{
	#ifdef __LOTUSCRT_PLATFORM_WINDOWS
	ZwTerminateProcess(NtCurrentProcess(), __exit_code);
	#endif
	__LOTUSCRT_UNREACHABLE;
}

#include <stdlib.h>

void use(char const volatile *c);
void use(char const volatile *c)
{
	__LOTUSCRT_UNUSED(c);
}

int main(void)
{
	volatile char buf[16];
	for(int i = 0; i < 20; ++i)
		buf[i] = 'a';

	use(buf);

	// Make sure buf isn't optimized away in release mode.
	// We obviously need it to be here to here to trigger /GS.
#if defined(__LOTUSCRT_COMPILER_GNULIKE) \
	|| defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
	__asm__ volatile("" : : "r,m"(buf) : "memory");
#elif defined(__LOTUSCRT_COMPILER_MSVC_ONLY)
	_ReadWriteBarrier();
#endif

	return 0;
}

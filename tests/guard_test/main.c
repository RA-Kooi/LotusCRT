// Explicitly disable CFG and attempt to indirect call a function to trigger it

#include <stdlib.h>

#include <nt_helpers.h>

void Foo(void);

#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(suppress))
#endif
__LOTUSCRT_FORCE_NOINLINE __LOTUSCRT_NORETURN void Foo(void)
{
	_Exit(1);
}

int main(void)
{
#ifndef __LOTUSCRT_COMPILER_MSVC_ONLY
	void *protectedFunc = (void*)&Foo;
	if(_Lotus_protect_functions(&protectedFunc, 1) != 1)
		return 2;
#endif

	typedef void (*func_t)(void);
	volatile func_t func = &Foo; // Volatile so the indirection doesn't get
	// removed during release builds.

	func();

	return -1;
}

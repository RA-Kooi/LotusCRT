#include "gs_support.h"

#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

#ifdef __LOTUSCRT_PLATFORM_WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DDFA232ll
#else
#define DEFAULT_SECURITY_COOKIE 0xBB40E64E
#endif

__LOTUSCRT_ALIGNAS(__LOTUSCRT_PACKING) typedef union
{
	UINT64 ft_scalar;
	FILETIME ft_struct;
} FT;

static EXCEPTION_RECORD GS_ExceptionRecord;
static CONTEXT GS_ContextRecord;

static EXCEPTION_POINTERS GS_ExceptionPointers =
{
	&GS_ExceptionRecord,
	&GS_ContextRecord
};

extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;

__LOTUSCRT_SELECTANY UINT_PTR __security_cookie = DEFAULT_SECURITY_COOKIE;
__LOTUSCRT_SELECTANY UINT_PTR __security_cookie_complement =
	(UINT_PTR)~(DEFAULT_SECURITY_COOKIE);

void __LOTUSCRT_CDECL __security_init_cookie(void)
{
	UINT_PTR cookie;
	FT systime = {0,};
	LARGE_INTEGER perfctr;

	if(__security_cookie != DEFAULT_SECURITY_COOKIE)
	{
		__security_cookie_complement = ~__security_cookie;
		return;
	}

	ZwQuerySystemTime((PLARGE_INTEGER)&systime.ft_scalar);
#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie = systime.ft_scalar;
#else
	cookie = systime.ft_struct.dwLowDateTime;
	cookie ^= systime.ft_struct.dwHighDateTime;
#endif
	cookie ^= (UINT_PTR)NtCurrentThreadId();
	cookie ^= (UINT_PTR)NtCurrentProcessId();

	cookie ^= (UINT_PTR)&cookie;

#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie ^= ((UINT_PTR)NtGetTickCount64()) << 56;
#else
	cookie ^= (UINT_PTR)NtGetTickCount64();
#endif

	ZwQueryPerformanceCounter(&perfctr, NULL);
#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie ^= (UINT_PTR)perfctr.QuadPart;
	cookie ^= (UINT_PTR)(((UINT64)perfctr.LowPart) << 32);
#else
	cookie ^= (UINT_PTR)perfctr.LowPart;
	cookie ^= (UINT_PTR)perfctr.HighPart;
#endif

#ifdef __LOTUSCRT_PLATFORM_WIN64
	cookie &= 0x0000FFFFFFFFFFFFll;
#endif

	if(cookie == DEFAULT_SECURITY_COOKIE)
		cookie = DEFAULT_SECURITY_COOKIE + 1;
#ifdef __LOTUSCRT_PLATFORM_WIN32
	else if((cookie & 0xFFFF0000) == 0)
		cookie |= (cookie | 0x4711) << 16;
#endif

	__security_cookie = cookie;
	__security_cookie_complement = ~__security_cookie;
}

/* wrap msvc intrinsics onto gcc builtins */
#if defined(__LOTUSCRT_COMPILER_GNULIKE) || defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
#undef  _ReturnAddress
#undef  _AddressOfReturnAddress
#define _ReturnAddress()		__builtin_return_address(0)
#define _AddressOfReturnAddress()	__builtin_frame_address (0)
#endif // __LOTUSCRT_COMPILER_GNULIKE

__LOTUSCRT_NORETURN void __LOTUSCRT_CDECL __report_gsfailure(ULONG_PTR);

__LOTUSCRT_NORETURN void __LOTUSCRT_CDECL __report_gsfailure(
	ULONG_PTR __stackCookie)
{
	volatile UINT_PTR cookie[2];
	(void)cookie;

#ifdef __LOTUSCRT_PLATFORM_WIN64
	ULONG64 controlPC, imgBase, establisherFrame;
	PRUNTIME_FUNCTION fctEntry;
	PVOID hndData;

	RtlCaptureContext(&GS_ContextRecord);
	controlPC = GS_ContextRecord.Rip;
	fctEntry = RtlLookupFunctionEntry(controlPC, &imgBase, NULL);
	if(fctEntry != NULL)
	{
		RtlVirtualUnwind(
			UNW_FLAG_NHANDLER,
			imgBase,
			controlPC,
			fctEntry,
			&GS_ContextRecord,
			&hndData,
			&establisherFrame,
			NULL);
	}
	else
#endif // __LOTUSCRT_PLATFORM_WIN64
	{
#if defined(__LOTUSCRT_ARCH_X64)
		GS_ContextRecord.Rip = (DWORD64)(_ReturnAddress());
		GS_ContextRecord.Rsp = (DWORD64)(_AddressOfReturnAddress()) + 8;
#elif defined(__LOTUSCRT_ARCH_X86)
		GS_ContextRecord.Eip = (DWORD)(_ReturnAddress());
		GS_ContextRecord.Esp = (DWORD)(_AddressOfReturnAddress()) + 4;
#elif defined(__LOTUSCRT_PROCESSOR_ARM)
		GS_ContextRecord.Pc = (DWORD)(_ReturnAddress());
		GS_ContextRecord.Sp = (DWORD)(_AddressOfReturnAddress()) + 4;
#endif // __LOTUSCRT_ARCH_X64
	}

#if defined(__LOTUSCRT_ARCH_X64)
	GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Rip;
	GS_ContextRecord.Rcx = __stackCookie;
#elif defined(__LOTUSCRT_ARCH_X86)
	GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Eip;
	GS_ContextRecord.Ecx = __stackCookie;
#elif defined(__LOTUSCRT_PROCESSOR_ARM)
	GS_ExceptionRecord.ExceptionAddress = (PVOID)GS_ContextRecord.Pc;
	UNUSED_PARAM(__stackCookie);
#endif // __LOTUSCRT_ARCH_X64
	GS_ExceptionRecord.ExceptionCode = (DWORD)STATUS_STACK_BUFFER_OVERRUN;
	GS_ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

	cookie[0] = __security_cookie;
	cookie[1] = __security_cookie_complement;

	RtlSetUnhandledExceptionFilter(NULL);
	RtlUnhandledExceptionFilter((EXCEPTION_POINTERS *)&GS_ExceptionPointers);

	ZwTerminateProcess(NtCurrentProcess(), STATUS_STACK_BUFFER_OVERRUN);
	__LOTUSCRT_UNREACHABLE;
}

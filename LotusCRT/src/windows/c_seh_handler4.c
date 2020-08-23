#include <_lotuscrt.h>

#include <stdint.h>

#include <phnt_windows.h>
#include <phnt.h>

#ifdef __LOTUSCRT_COMPILER_MSVC
	#define __MM_MALLOC_H
#endif
#include <xmmintrin.h>

static DWORD _Lotus_filter_SSE2_FPE(DWORD const __exceptionCode);

typedef void (__fastcall *PCOOKIE_CHECK)(UINT_PTR);
extern void __fastcall __security_check_cookie(uintptr_t __stackCookie);

typedef LONG (__cdecl *PEXCEPTION_FILTER_X86)(void); // __except filter
typedef void (__cdecl *PEXCEPTION_HANDLER_X86)(void); // __except handler
typedef void (__fastcall *PTERMINATION_HANDLER_X86)(BOOL); // __finally

extern LONG __fastcall _Lotus_SEH_call_filter_func(
	PEXCEPTION_FILTER_X86 __filterFunc,
	PCHAR __framePointer);

extern void __LOTUSCRT_NORETURN __fastcall _Lotus_SEH_transfer_to_handler(
	PEXCEPTION_HANDLER_X86 __handlerAddress,
	PCHAR __framePointer);

extern void __fastcall _Lotus_SEH_global_unwind(
	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
	PEXCEPTION_RECORD __exceptionRecord);

extern void __fastcall _Lotus_SEH_local_unwind(
	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
	ULONG __targetLevel,
	PCHAR __framePointer,
	PUINT_PTR __cookie);

extern UINT_PTR __security_cookie;

typedef struct _EH4_SCOPETABLE_RECORD
{
	ULONG enclosingLevel;
	PEXCEPTION_FILTER_X86 filterFunc;
	union
	{
		PEXCEPTION_HANDLER_X86 handlerAddress;
		PTERMINATION_HANDLER_X86 finallyFunc;
	} u;
} EH4_SCOPETABLE_RECORD, *PEH4_SCOPETABLE_RECORD;

typedef struct _EH4_SCOPETABLE
{
	ULONG GSCookieOffset;
	ULONG GSCookieXOROffset;
	ULONG EHCookieOffset;
	ULONG EHCookieXOROffset;
	EH4_SCOPETABLE_RECORD scopeRecord[1];
} EH4_SCOPETABLE, *PEH4_SCOPETABLE;

typedef struct _EH4_EXCEPTION_REGISTRATION_RECORD
{
	PVOID savedESP;
	PEXCEPTION_POINTERS exceptionPointers;
	EXCEPTION_REGISTRATION_RECORD subRecord;
	UINT_PTR encodedScopeTable;
	ULONG tryLevel;
} EH4_EXCEPTION_REGISTRATION_RECORD, *PEH4_EXCEPTION_REGISTRATION_RECORD;

static void _Lotus_validate_cookies(
#ifdef __LOTUSCRT_AS_DLL
	PCOOKIE_CHECK __cookieCheckFunc,
#endif
	PEH4_SCOPETABLE __scopeTable,
	PCHAR __framePointer);

#define TOPMOST_TRY_LEVEL ((ULONG)-2)

EXCEPTION_DISPOSITION
#ifndef __LOTUSCRT_AS_DLL
_except_handler4(
#else
_except_handler4_common(
	PUINT_PTR __cookie,
	PCOOKIE_CHECK __cookieCheckFunc
#endif
	PEXCEPTION_RECORD __exceptionRecord,
	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
	PCONTEXT __contextRecord,
	PVOID __dispatcherContext);

#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(suppress))
#endif
EXCEPTION_DISPOSITION
#ifndef __LOTUSCRT_AS_DLL
_except_handler4(
#else
_except_handler4_common(
	PUINT_PTR __cookie,
	PCOOKIE_CHECK __cookieCheckFunc
#endif
	PEXCEPTION_RECORD __exceptionRecord,
	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
	PCONTEXT __contextRecord,
	PVOID __dispatcherContext)
{
	__LOTUSCRT_UNUSED(__dispatcherContext);

	EXCEPTION_DISPOSITION disposition = ExceptionContinueSearch;

#ifndef __LOTUSCRT_AS_DLL
	__exceptionRecord->ExceptionCode = _Lotus_filter_SSE2_FPE(
		__exceptionRecord->ExceptionCode);
#endif

	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
	PEH4_EXCEPTION_REGISTRATION_RECORD const registrationNode =
		(PEH4_EXCEPTION_REGISTRATION_RECORD)(
			(PCHAR)__establisherFrame
			- FIELD_OFFSET(EH4_EXCEPTION_REGISTRATION_RECORD, subRecord));
	__LOTUSCRT_RESTORE_CLANG_WARNING()

	PCHAR const framePointer = (PCHAR)(registrationNode + 1);

#ifdef __LOTUSCRT_AS_DLL
	PEH4_SCOPETABLE const scopeTable = (PEH4_SCOPETABLE)(
		registrationNode->encodedScopeTable ^ __cookie);
#else
	PEH4_SCOPETABLE const scopeTable = (PEH4_SCOPETABLE)(
		registrationNode->encodedScopeTable ^ __security_cookie);
#endif

	_Lotus_validate_cookies(
#ifdef __LOTUSCRT_AS_DLL
		__cookieCheckFunc,
#endif
		scopeTable,
		framePointer);

	// TODO: validate context record

	BOOLEAN revalidate = FALSE;
	if(IS_DISPATCHING(__exceptionRecord->ExceptionFlags))
	{
		EXCEPTION_POINTERS exceptionPointers =
			{__exceptionRecord, __contextRecord};
		registrationNode->exceptionPointers = &exceptionPointers;

		ULONG enclosingLevel;
		for(ULONG tryLevel = registrationNode->tryLevel;
			tryLevel != TOPMOST_TRY_LEVEL;
			tryLevel = enclosingLevel)
		{
			PEH4_SCOPETABLE_RECORD const scopeTableRecord =
				&scopeTable->scopeRecord[tryLevel];

			PEXCEPTION_FILTER_X86 const filterFunc =
				scopeTableRecord->filterFunc;

			enclosingLevel = scopeTableRecord->enclosingLevel;

			if(filterFunc)
			{
				LONG const filterResult = _Lotus_SEH_call_filter_func(
					filterFunc,
					framePointer);

				revalidate = TRUE;

				if(filterResult < 0)
				{
					disposition = ExceptionContinueExecution;
					break;
				}
				else if(filterResult > 0)
				{
					// TODO: Call C++ destructors

					_Lotus_SEH_global_unwind(
						&registrationNode->subRecord,
						__exceptionRecord);

					if(registrationNode->tryLevel != tryLevel)
					{
						_Lotus_SEH_local_unwind(
							&registrationNode->subRecord,
							tryLevel,
							framePointer,
					#ifdef __LOTUSCRT_AS_DLL
							__cookie);
					#else
							&__security_cookie);
					#endif
					}

					registrationNode->tryLevel = enclosingLevel;

					_Lotus_validate_cookies(
				#ifdef __LOTUSCRT_AS_DLL
						__cookie,
				#endif
						scopeTable,
						framePointer);

					_Lotus_SEH_transfer_to_handler(
						scopeTableRecord->u.handlerAddress,
						framePointer);
				}
			}
		}
	}
	else
	{
		if(registrationNode->tryLevel != TOPMOST_TRY_LEVEL)
		{
			_Lotus_SEH_local_unwind(
				&registrationNode->subRecord,
				TOPMOST_TRY_LEVEL,
				framePointer,
			#ifdef __LOTUSCRT_AS_DLL
				__cookie);
			#else
				&__security_cookie);
			#endif

			revalidate = TRUE;
		}
	}

	if(revalidate)
	{
		_Lotus_validate_cookies(
#ifdef __LOTUSCRT_AS_DLL
			__cookie,
#endif
			scopeTable,
			framePointer);
	}

	return disposition;
}

static DWORD _Lotus_filter_SSE2_FPE(DWORD const __exceptionCode)
{
	// TODO: Implement CPUID check
	//if not SSE2 available
	// return __exceptionCode

	if(__exceptionCode != (DWORD)STATUS_FLOAT_MULTIPLE_FAULTS
	   && __exceptionCode != (DWORD)STATUS_FLOAT_MULTIPLE_TRAPS)
		return __exceptionCode;

	typedef unsigned int uint;

	uint const invalidOperationBit  = 0x0001;
	uint const denormalOperationBit = 0x0002;
	uint const divideByZeroBit      = 0x0004;
	uint const overflowBit          = 0x0008;
	uint const underflowBit         = 0x0010;
	uint const precisionBit         = 0x0020;
	uint const exceptionMask        = 0x003f;

	uint const invalidOperationDisableBit = 0x0080;
	uint const denomalOperationDisableBit = 0x0100;
	uint const divideByZeroDisableBit     = 0x0200;
	uint const overflowDisableBit         = 0x0400;
	uint const underflowDisableBit        = 0x0800;
	uint const precisionDisableBit        = 0x1000;

	uint const invalidOperation  = invalidOperationDisableBit | invalidOperationBit;
	uint const denormalOperation = denomalOperationDisableBit | denormalOperationBit;
	uint const divideByZero      = divideByZeroDisableBit     | divideByZeroBit;
	uint const overflow          = overflowDisableBit         | overflowBit;
	uint const underflow         = underflowDisableBit        | underflowBit;
	uint const precision         = precisionDisableBit        | precisionBit;

	uint const maskedStatus = (uint)_mm_getcsr() ^ exceptionMask;

	if(!(maskedStatus & invalidOperation))
		return (DWORD)EXCEPTION_FLT_INVALID_OPERATION;

	if(!(maskedStatus & divideByZero))
		return (DWORD)EXCEPTION_FLT_DIVIDE_BY_ZERO;

	if(!(maskedStatus & denormalOperation))
		return (DWORD)EXCEPTION_FLT_INVALID_OPERATION;

	if(!(maskedStatus & overflow))
		return (DWORD)EXCEPTION_FLT_OVERFLOW;

	if(!(maskedStatus & underflow))
		return (DWORD)EXCEPTION_FLT_UNDERFLOW;

	if(!(maskedStatus & precision))
		return (DWORD)EXCEPTION_FLT_INEXACT_RESULT;

	return __exceptionCode;
}

#define NO_GS_COOKIE ((ULONG)-2)

static
#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(ignore))
#endif
void _Lotus_validate_cookies(
#ifdef __LOTUSCRT_AS_DLL
	PCOOKIE_CHECK __cookieCheckFunc,
#endif
	PEH4_SCOPETABLE __scopeTable,
	PCHAR __framePointer)
{
	if(__scopeTable->GSCookieOffset != NO_GS_COOKIE)
	{
		__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
		UINT_PTR GSCookie = *(PUINT_PTR)(__framePointer + __scopeTable->GSCookieOffset);
		__LOTUSCRT_RESTORE_CLANG_WARNING()
		GSCookie ^= (UINT_PTR)(__framePointer + __scopeTable->GSCookieXOROffset);

#ifdef __LOTUSCRT_AS_DLL
		// TODO:Do guard check call on __cookieCheckFunc
		(*__cookieCheckFunc)(GSCookie);
#else
		__security_check_cookie(GSCookie);
#endif
	}

	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wcast-align)
	UINT_PTR EHCookie = *(PUINT_PTR)(__framePointer + __scopeTable->EHCookieOffset);
	__LOTUSCRT_RESTORE_CLANG_WARNING()
	EHCookie ^= (UINT_PTR)(__framePointer + __scopeTable->EHCookieXOROffset);

#ifdef __LOTUSCRT_AS_DLL
	// TODO:Do guard check call on __cookieCheckFunc
	(*__cookieCheckFunc)(EHCookie);
#else
	__security_check_cookie(EHCookie);
#endif
}

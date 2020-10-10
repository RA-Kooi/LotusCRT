#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

#include "context_validation.h"

// C specific SEH exception handler
// x64 only
// See c_seh_handler4.c for x86

// A function that seems to notify the debugger or something, idk
extern void _NLG_Notify(
	void *__offsetDestination,
	void *__offsetFramePointer,
	ULONG __code);

extern void __NLG_Return2(void);

#define DISABLE_SHRINK_WRAPPING() NOTHING
#define ENABLE_SHRINK_WRAPPING()  NOTHING

extern EXCEPTION_DISPOSITION __C_specific_handler(
	PEXCEPTION_RECORD __exceptionRecord,
	void *__establisherFrame,
	PCONTEXT __contextRecord,
	PDISPATCHER_CONTEXT __dispatcherContext);

#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(ignore))
__declspec(guard(suppress))
// TODO: Setup supression from the process loader using NTAPI.
#endif
#ifdef __LOTUSCRT_COMPILER_MSVC
#pragma comment(linker, "/GUARDSYM:__C_specific_handler,S")
#endif
EXCEPTION_DISPOSITION __C_specific_handler(
	PEXCEPTION_RECORD __exceptionRecord,
	void *__establisherFrame,
	PCONTEXT __contextRecord,
	PDISPATCHER_CONTEXT __dispatcherContext)
{
	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wextra-semi-stmt)
	DISABLE_SHRINK_WRAPPING();
	__LOTUSCRT_RESTORE_CLANG_WARNING()

#ifdef __LOTUSCRT_PLATFORM_WIN64
	_Lotus_validate_context_record(__contextRecord);
#endif

	ULONG_PTR const imageBase = __dispatcherContext->ImageBase;
	ULONG_PTR const controlPC = __dispatcherContext->ControlPc - imageBase;

	PSCOPE_TABLE const scopeTable =
		(PSCOPE_TABLE)__dispatcherContext->HandlerData;

	DWORD const exceptionFlags = __exceptionRecord->ExceptionFlags;

	if(IS_DISPATCHING(exceptionFlags))
	{
		// An exception is being actively dispatched. Call the exception
		// filter(s) and start the unwinding process.

		EXCEPTION_POINTERS exceptionPointers =
			{ __exceptionRecord, __contextRecord };

		// Scan the scope table for exception filters.
		for(ULONG index = __dispatcherContext->ScopeIndex;
			index < scopeTable->Count;
			++index)
		{
			DWORD const beginAddress =
				scopeTable->ScopeRecord[index].BeginAddress;

			DWORD const endAddress =
				scopeTable->ScopeRecord[index].EndAddress;

			DWORD const jumpTarget = scopeTable->ScopeRecord[index].JumpTarget;

			DWORD const handlerAddress =
				scopeTable->ScopeRecord[index].HandlerAddress;

			// Check if the filter is in our current scope level.
			if((controlPC >= beginAddress)
			   && (controlPC < endAddress)
			   && (jumpTarget != 0))
			{
				LONG value;

				// If the handler address is one we execute the handler.
				// I'm not sure what case would actually cause this to be one.
				if(handlerAddress == 1)
					value = EXCEPTION_EXECUTE_HANDLER;
				else
				{
					// We have a filter, calculate its address and execute it.

					PEXCEPTION_FILTER const exceptionFilter =
						(PEXCEPTION_FILTER)(handlerAddress + imageBase);

					// Execute exception Filter
					value = (exceptionFilter)(
						&exceptionPointers,
						__establisherFrame);
				}

				if(value < EXCEPTION_CONTINUE_SEARCH)
					return ExceptionContinueExecution;
				else if(value > EXCEPTION_CONTINUE_SEARCH)
				{
					// The filter returned EXCEPTION_EXECUTE_HANDLER.
					// Calculate the address of the funclet and use unwinding
					// magic to execute it.

					// TODO: Execute C++ destructors

					ULONG_PTR const handler = imageBase + jumpTarget;

					// Notify the debugger that we're about to unwind the stack.
					_NLG_Notify((void*)handler, __establisherFrame, 1);

					RtlUnwindEx(
						__establisherFrame,
						(void*)handler,
						__exceptionRecord,
						(void*)((ULONG_PTR)__exceptionRecord->ExceptionCode),
						(PCONTEXT)__dispatcherContext->ContextRecord,
						__dispatcherContext->HistoryTable);

					// Executes a ret instruction. We can't do a normal return
					// as that would attempt to clean up the stack. But we
					// can't do that because the current stack is not our stack
					// anymore cause we unwound the stack.
					__NLG_Return2();
				}
			}
		}
	}
	else
	{
		// An exception is being unwound, but we haven't found an appropriate
		// handler yet. Scan the scope table and call the __finally routines
		// for this scope.

		ULONG_PTR const targetPc = __dispatcherContext->TargetIp - imageBase;

		for(
			ULONG index = __dispatcherContext->ScopeIndex;
			index < scopeTable->Count;
			++index)
		{
			DWORD const beginAddress =
				scopeTable->ScopeRecord[index].BeginAddress;

			DWORD const endAddress =
				scopeTable->ScopeRecord[index].EndAddress;

			DWORD const jumpTarget = scopeTable->ScopeRecord[index].JumpTarget;

			DWORD const handlerAddress =
				scopeTable->ScopeRecord[index].HandlerAddress;

			if((controlPC >= beginAddress) && (controlPC <endAddress))
			{
				if(IS_TARGET_UNWIND(exceptionFlags))
				{
					// If the targetPc is within the same scope as the controlPc
					// then this is an uplevel goto, or longjmp back into a try
					// scope. Stop looking for a __finally block.

					ULONG targetIndex = 0;

					for(; targetIndex < scopeTable->Count; ++targetIndex)
					{
						DWORD const beginTargetAddress =
							scopeTable->ScopeRecord[targetIndex].BeginAddress;

						DWORD const endTargetAddress =
							scopeTable->ScopeRecord[targetIndex].EndAddress;

						DWORD const targetJumpTarget =
							scopeTable->ScopeRecord[targetIndex].JumpTarget;

						DWORD const targetHandlerAddress =
							scopeTable->ScopeRecord[targetIndex].HandlerAddress;

						if((targetPc >= beginTargetAddress)
						   && (targetPc < endTargetAddress)
						   && (targetJumpTarget == jumpTarget)
						   && (targetHandlerAddress == handlerAddress))
						{
							break;
						}
					}

					if(targetIndex != scopeTable->Count)
						break;
				}

				if(jumpTarget != 0)
				{
					// If the jump target describes an exception filter and
					// the exception handler is the target of the unwind then
					// stop looking for __finally blocks.
					if((targetPc == jumpTarget)
					   && IS_TARGET_UNWIND(exceptionFlags))
						break;
				}
				else
				{
					// We have found a __finally handler, execute it.

					__dispatcherContext->ScopeIndex = index + 1;

					PTERMINATION_HANDLER const terminationHandler =
						(PTERMINATION_HANDLER)(handlerAddress + imageBase);

					// Execute __finally handler
					(terminationHandler)(TRUE, __establisherFrame);
				}
			}
		}
	}

	__LOTUSCRT_DISABLE_CLANG_WARNING(-Wextra-semi-stmt)
	ENABLE_SHRINK_WRAPPING();
	__LOTUSCRT_RESTORE_CLANG_WARNING()

	// We may have executed a filter or __finally handler, but not the actual
	// exception handler. Inform the kernel to re-execute this function with
	// new scope information and try again to find a handler.
	return ExceptionContinueSearch;
}

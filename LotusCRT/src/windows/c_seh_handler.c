#include <_lotuscrt.h>

#include <phnt_windows.h>
#include <phnt.h>

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

__LOTUSCRT_DISABLE_CLANG_WARNING(-Wextra-semi-stmt)
#ifdef __LOTUSCRT_COMPILER_MSVC_ONLY
__declspec(guard(ignore))
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
	DISABLE_SHRINK_WRAPPING();

#ifdef __LOTUSCRT_PLATFORM_WIN64
	// Validate __contextRecord
#endif

	ULONG_PTR const imageBase = __dispatcherContext->ImageBase;
	ULONG_PTR const controlPC = __dispatcherContext->ControlPc - imageBase;

	PSCOPE_TABLE const scopeTable =
		(PSCOPE_TABLE)__dispatcherContext->HandlerData;

	DWORD const exceptionFlags = __exceptionRecord->ExceptionFlags;

	if(IS_DISPATCHING(exceptionFlags))
	{
		EXCEPTION_POINTERS exceptionPointers =
			{ __exceptionRecord, __contextRecord };

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

			if((controlPC >= beginAddress)
			   && (controlPC < endAddress)
			   && (jumpTarget != 0))
			{
				LONG value;

				if(handlerAddress == 1)
					value = EXCEPTION_EXECUTE_HANDLER;
				else
				{
					PEXCEPTION_FILTER const exceptionFilter =
						(PEXCEPTION_FILTER)(handlerAddress + imageBase);

					// Execute exception Filter
					value = (exceptionFilter)(
						&exceptionPointers,
						__establisherFrame);
				}

				if(value < 0)
					return ExceptionContinueExecution;
				else if(value > 0)
				{
					// TODO: Execute C++ destructors

					ULONG_PTR const handler = imageBase + jumpTarget;

					_NLG_Notify((void*)handler, __establisherFrame, 1);

					RtlUnwindEx(
						__establisherFrame,
						(void*)handler,
						__exceptionRecord,
						(void*)((ULONG_PTR)__exceptionRecord->ExceptionCode),
						(PCONTEXT)__dispatcherContext->ContextRecord,
						__dispatcherContext->HistoryTable);

					__NLG_Return2();
				}
			}
		}
	}
	else
	{
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
					if((targetPc == jumpTarget)
					   && IS_TARGET_UNWIND(exceptionFlags))
						break;
				}
				else
				{
					__dispatcherContext->ScopeIndex = index + 1;

					PTERMINATION_HANDLER const terminationHandler =
						(PTERMINATION_HANDLER)(handlerAddress + imageBase);

					// Execute termination handler
					(terminationHandler)(TRUE, __establisherFrame);
				}
			}
		}
	}

	ENABLE_SHRINK_WRAPPING();

	return ExceptionContinueSearch;
}
__LOTUSCRT_RESTORE_CLANG_WARNING()

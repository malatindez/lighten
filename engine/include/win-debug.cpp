#include "win-debug.hpp"
static std::function<void(std::string_view)> OutputDebugStringCallback;
LONG NTAPI VexHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    PEXCEPTION_RECORD ExceptionRecord = ExceptionInfo->ExceptionRecord;

    switch (ExceptionRecord->ExceptionCode)
    {
        case DBG_PRINTEXCEPTION_WIDE_C:
        case DBG_PRINTEXCEPTION_C:

            if (ExceptionRecord->NumberParameters >= 2)
            {
                bool call_free = false;
                ULONG len = (ULONG)ExceptionRecord->ExceptionInformation[0];

                union
                {
                    ULONG_PTR up;
                    PCWSTR pwz;
                    PCSTR psz;
                };

                up = ExceptionRecord->ExceptionInformation[1];

                if (ExceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_WIDE_C)
                {
                    if (ULONG n = WideCharToMultiByte(CP_UTF8, 0, pwz, len, nullptr, 0, nullptr, nullptr))
                    {
                        PSTR sz = (PSTR)_malloca(n * sizeof(CHAR));

                        if (len = WideCharToMultiByte(CP_UTF8, 0, pwz, len, sz, n, nullptr, nullptr);
                            len)
                        {
                            psz = sz;
                            call_free = true;
                        }
                        else
                        {
                            _freea((void *)(psz));
                        }
                    }
                }
                if (call_free)
                {
                    _freea((void *)(psz));
                }

                OutputDebugStringCallback(psz);
            }
            return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

namespace engine::debug
{
    void RedirectOutputDebugString(std::function<void(std::string_view)> const &callback)
    {
        OutputDebugStringCallback = callback;
        AddVectoredExceptionHandler(TRUE, VexHandler);
    }
}
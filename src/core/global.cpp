#include <base/macro.hpp>
#include "global.hpp"

static HANDLE WINAPI original_GetProcessHeap()
{
    static auto volatile p_GetProcessHeap =
        reinterpret_cast<decltype(&::GetProcessHeap)>(
            ::GetProcAddress(
            ::GetModuleHandleW(L"kernel32.dll"), "GetProcessHeap"));

    if (p_GetProcessHeap)
    {
        return p_GetProcessHeap();
    }
    else
    {
        ::SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
}

EXTERN_C HANDLE WINAPI compile_time_hook_GetProcessHeap()
{
    return global_info.heap;
}

#if defined(_M_IX86)
#pragma warning(suppress:4483)
extern "C" __declspec(selectany) void const* const __identifier("_imp__GetProcessHeap@703") = reinterpret_cast<void const*>(::compile_time_hook_GetProcessHeap);
#pragma comment(linker, "/include:__imp__GetProcessHeap@703")
#else
extern "C" __declspec(selectany) void const* const __imp_GetProcessHeap = reinterpret_cast<void const*>(::compile_time_hook_GetProcessHeap);
#pragma comment(linker, "/include:__imp_GetProcessHeap")
#endif

static void get_global_info_from_pipe() noexcept
{
    const HANDLE pipe = ::CreateFileA(GLOBAL_PIPENAME, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    DWORD timeout = 1000;
    ::SetNamedPipeHandleState(pipe, nullptr, nullptr, &timeout);

    DWORD lpNumberOfBytesRead;
    BOOL result = ::ReadFile(pipe, &global_info, sizeof(global_info), &lpNumberOfBytesRead, nullptr);
    if (!result || lpNumberOfBytesRead != sizeof(global_info))
    {
        // TODO: error handle
    }
    ::CloseHandle(pipe);
}

void initialize() noexcept
{
#ifdef PROJECT_DEBUG
    while (!::IsDebuggerPresent())
    {
        ::Sleep(100);
    }
#endif

    // get_global_info_from_pipe();
    global_info.code_page = ::GetACP();
    global_info.heap = ::original_GetProcessHeap();
}

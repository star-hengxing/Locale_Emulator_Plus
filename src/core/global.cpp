#include "global.hpp"

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
    // get_global_info_from_pipe();
    global_info.code_page = ::GetACP();
    // ::GetProcessHeap();
}

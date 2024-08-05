#include <windows.h>
#include <detours.h>
#include "nls.h"


consteval auto GetDllName() noexcept
{
    if constexpr (sizeof(void*) == 8)
    {
        return "locale_emulator_plus64.dll";
    }
    else if constexpr (sizeof(void*) == 4)
    {
        return "locale_emulator_plus32.dll";
    }
}

int wmain(int argc, wchar_t** argv)
{
    if (argc != 2) { return 0; }

    ::PROCESS_INFORMATION pi{};
    ::STARTUPINFOW si{ .cb = sizeof(si) };

    const auto status = ::DetourCreateProcessWithDllExW(
        argv[1],
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &si,
        &pi,
        GetDllName(),
        nullptr);

    if (!status) { return -2; }

    ::NlsPatcher(NlsPatcherSysVer::Nls_Windows10_x32, pi.hProcess, 0x3A4);

    ::MessageBoxW(0, 0, 0, 0);
    ::ResumeThread(pi.hThread);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
}

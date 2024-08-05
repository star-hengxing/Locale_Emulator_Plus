#include <windows.h>
#include <detours.h>
#include <LEP/Utils/NlsPatcher.h>


auto wmain(int argc, wchar_t** argv) -> int
{
    if (argc != 2) { return 0; }

    ::PROCESS_INFORMATION pi{};
    ::STARTUPINFOW si{ .cb = sizeof(si) };

    const auto status = ::DetourCreateProcessWithDllExW
    (
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
        "lep_core.dll",
        nullptr
    );

    if (!status) { return -2; }

    LEP::Utils::NlsPatcher::Install(pi.hProcess, LEP::Utils::NlsPatcher::SysVer::Nls_Windows10_x32, 0x3A4);

    ::MessageBoxW(0, 0, 0, 0);

    ::ResumeThread(pi.hThread);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
}

#include <LEP/Core/Win32uHook.h>
#include <LEP/Utils/TextConvert.h>
#include <LEP/Utils/Hooker.h>
#include <Windows.h>


static auto __stdcall NtUserMessageCall_Hook(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, DWORD xpfnProc, BOOL Ansi) -> LRESULT
{
    if (!Ansi) { return LEP::Utils::Hooker<::NtUserMessageCall_Hook>::FnRaw(hWnd, Message, wParam, lParam, xParam, xpfnProc, Ansi); }

    // proc send ansi data
    {
        std::unique_ptr<wchar_t[]> buffer;

        switch (Message)
        {
        case WM_SETTEXT:
            buffer = LEP::Utils::MBCSToWide(reinterpret_cast<const char*>(lParam));
            lParam = reinterpret_cast<LPARAM>(buffer.get());
            Ansi = FALSE;
            break;
        }

        return LEP::Utils::Hooker<::NtUserMessageCall_Hook>::FnRaw(hWnd, Message, wParam, lParam, xParam, xpfnProc, Ansi);
    }

    // proc get ansi data
    {

    }
}

namespace LEP::Core::Win32u
{
    auto InstallHook() -> void
    {
        const auto win32u_handle = ::GetModuleHandleW(L"win32u.dll");
        if (win32u_handle == NULL) { return; }

        const auto fn_NtUserMessageCall_va = reinterpret_cast<std::size_t>(::GetProcAddress(win32u_handle, "NtUserMessageCall"));
        LEP::Utils::Detour::CommitBeg();
        {
            LEP::Utils::Hooker<::NtUserMessageCall_Hook>::Attach(fn_NtUserMessageCall_va);
        }
        LEP::Utils::Detour::CommitEnd();
    }
}

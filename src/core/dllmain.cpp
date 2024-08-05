#include <windows.h>
#include <detours.h>
#include <core/single_hooker.hpp>

#include <memory>
#include <string_view>


static auto MBCSToWide(const std::string_view msPath) -> std::unique_ptr<wchar_t[]>
{
    const auto buffer_max_chars = (msPath.size() * sizeof(char) + 1) * 2;
    auto buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
    const auto char_count_real = static_cast<std::size_t>(::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(buffer_max_chars)));
    buffer[char_count_real] = {};
    return std::unique_ptr<wchar_t[]>{ std::move(buffer) };
}


static auto __stdcall NtUserMessageCall_Hook(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ULONG_PTR xParam, DWORD xpfnProc, BOOL Ansi) -> LRESULT
{
    if (!Ansi) { return SingleHooker<::NtUserMessageCall_Hook>::FnRaw(hWnd, Message, wParam, lParam, xParam, xpfnProc, Ansi); }

    // proc send ansi data
    {
        std::unique_ptr<wchar_t[]> buffer;

        switch (Message)
        {
        case WM_SETTEXT:
            Ansi = FALSE;
            buffer = MBCSToWide(reinterpret_cast<const char*>(lParam));
            lParam = reinterpret_cast<LPARAM>(buffer.get());
            break;
        }

        return SingleHooker<::NtUserMessageCall_Hook>::FnRaw(hWnd, Message, wParam, lParam, xParam, xpfnProc, Ansi);
    }

    // proc get ansi data
    {

    }
}

static auto StartHook(HMODULE /* hDllBase */) -> void
{
    const auto win32u_handle = ::GetModuleHandleW(L"win32u.dll");
    if (win32u_handle == NULL) { return; }

    const auto fn_NtUserMessageCall_va = reinterpret_cast<std::size_t>(::GetProcAddress(win32u_handle, "NtUserMessageCall"));

    SingleHookerAUX::CommitBeg();
    {
        SingleHooker<::NtUserMessageCall_Hook>::Register(fn_NtUserMessageCall_va);
    }
    SingleHookerAUX::CommitEnd();
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::DetourRestoreAfterWith();
        ::StartHook(hModule);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

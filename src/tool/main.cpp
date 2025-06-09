#include <string>
#include <string_view>

#include <windows.h>
#include <shlwapi.h>
#include <sddl.h>

#include <detours.h>

#include <base/basic_type.hpp>

consteval auto get_dll_name() noexcept
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

constexpr auto DLL_NAME{
#ifndef LOCALE_EMULATOR_PLUS_DLL_NAME
    get_dll_name()
#else
    LOCALE_EMULATOR_PLUS_DLL_NAME
#endif
};

struct fixed_buffer
{
    usize size = 0;
    wchar_t data[MAX_PATH];
};

static fixed_buffer string2wstring(const std::string_view string) noexcept
{
    usize size = ::MultiByteToWideChar(CP_ACP, 0, string.data(), string.size(), nullptr, 0);
    if (size == 0)
    {
        return {};
    }

    fixed_buffer buffer;
    buffer.size = size;
    if (::MultiByteToWideChar(CP_ACP, 0, string.data(), string.size(), buffer.data, size) == 0)
    {
        return {};
    }
    buffer.data[size] = L'\0';
    return buffer;
}

static std::wstring get_sid() noexcept
{
    HANDLE token;
    DWORD dwSize = 0;
    PTOKEN_USER ptu = nullptr;

    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
    {
        return {};
    }

    ::GetTokenInformation(token, TokenUser, nullptr, 0, &dwSize);
    if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        ::CloseHandle(token);
        return {};
    }

    std::wstring sid;
    ptu = (PTOKEN_USER)::LocalAlloc(LPTR, dwSize);
    if (::GetTokenInformation(token, TokenUser, ptu, dwSize, &dwSize))
    {
        LPWSTR sidString = nullptr;
        if (::ConvertSidToStringSidW(ptu->User.Sid, &sidString))
        {
            sid = sidString;
            ::LocalFree(sidString);
        }
    }

    ::LocalFree(ptu);
    ::CloseHandle(token);
    return sid;
}

static HKEY create_key(const wchar_t* name) noexcept
{
    std::wstring key = get_sid();
    if (key.empty())
    {
        return nullptr;
    }

    HKEY key_handle;
    key += L"\\SOFTWARE\\Classes\\*\\shell\\";
    key += name;
    LSTATUS result = ::RegCreateKeyExW(
        HKEY_USERS,
        key.c_str(),
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        nullptr,
        &key_handle,
        nullptr);

    return (result == ERROR_SUCCESS ? key_handle : nullptr);
}

static bool remove_key(const wchar_t* name) noexcept
{
    std::wstring key = get_sid();
    if (key.empty())
    {
        return false;
    }

    key += L"\\SOFTWARE\\Classes\\*\\shell\\";
    key += name;
    return ::SHDeleteKeyW(HKEY_USERS, key.data()) == ERROR_SUCCESS;
}

static bool reg_install() noexcept
{
    LSTATUS result = 0;
    HKEY le_plus = ::create_key(L"Locale Emulator Plus");
    if (!le_plus)
    {
        return false;
    }

    HKEY le_plus_cmd = ::create_key(L"Locale Emulator Plus\\command");
    if (!le_plus_cmd)
    {
        ::RegCloseKey(le_plus);
        return false;
    }

    constexpr std::wstring_view app_name = L"Locale Emulator Plus";
    result = ::RegSetValueExW(le_plus, nullptr, 0, REG_SZ,
                              reinterpret_cast<const BYTE*>(app_name.data()),
                              (app_name.size() + 1) * sizeof(wchar_t));

    wchar_t program_path[MAX_PATH];
    ::GetModuleFileNameW(nullptr, program_path, MAX_PATH);
    std::wstring cmd = LR"(")";
    cmd += program_path;
    cmd += LR"(" "%1")";
    result = ::RegSetValueExW(le_plus_cmd, nullptr, 0, REG_SZ,
                              reinterpret_cast<const BYTE*>(cmd.c_str()),
                              (cmd.size() + 1) * sizeof(wchar_t));

    result = ::RegCloseKey(le_plus);
    result = ::RegCloseKey(le_plus_cmd);

    return result == ERROR_SUCCESS;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        return 0;
    }

    const std::string_view argv2 = argv[1];
    if (argv2 == "install")
    {
        if (::reg_install())
            return 0;
        else
            return -1;
    }
    else if (argv2 == "uninstall")
    {
        if (remove_key(L"Locale Emulator Plus"))
            return 0;
        else
            return -2;
    }

    TCHAR absolute_dll_path[MAX_PATH];
    ::GetModuleFileNameA(nullptr, absolute_dll_path, MAX_PATH);
    ::PathRemoveFileSpecA(absolute_dll_path);
    ::PathAppendA(absolute_dll_path, DLL_NAME);

    auto const [size, program_path] = ::string2wstring(argv2);
    if (size == 0)
        return -1;

    ::PROCESS_INFORMATION process_info{};
    ::STARTUPINFOW startup_info{.cb = sizeof(startup_info)};

    // wchar_t cur_dir[MAX_PATH];
    // std::memcpy(cur_dir, program_path, size * sizeof(wchar_t) + 1);
    // ::PathRemoveFileSpecW(cur_dir);

    BOOL result = ::DetourCreateProcessWithDllExW(
        program_path, nullptr, nullptr,
        nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
        &startup_info, &process_info, absolute_dll_path, nullptr);

    if (result == FALSE)
    {
        return -3;
    }
}

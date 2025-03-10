#include <string_view>

#include <windows.h>
#include <shlwapi.h>

#include <detours.h>
#ifdef PROJECT_DEBUG
#include <libipc/ipc.h>
#endif

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

constexpr auto DLL_NAME
{
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

    fixed_buffer buffer{size};
    if (::MultiByteToWideChar(CP_ACP, 0, string.data(), string.size(), buffer.data, size) == 0)
    {
        return {};
    }
    buffer.data[size] = L'\0';
    return buffer;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return 0;
    }

    TCHAR absolute_dll_path[MAX_PATH];
    ::GetModuleFileNameA(nullptr, absolute_dll_path, MAX_PATH);
    ::PathRemoveFileSpecA(absolute_dll_path);
    ::PathAppendA(absolute_dll_path, DLL_NAME);

    auto const [size, program_path] = ::string2wstring(argv[1]);
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
        return -2;
    }

#ifdef PROJECT_DEBUG
    ipc::route cc{"my-ipc-route", ipc::receiver};
    while (true)
    {
        auto buf = cc.recv();
        auto str = static_cast<char*>(buf.data());
        if (str == nullptr || str[0] == '\0')
            continue;

        std::printf("%s\n", str);
    }
#endif
}

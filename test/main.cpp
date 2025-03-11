#include <string>
#include <string_view>

#include <Windows.h>
#include <boost/ut.hpp>

#include <windows.h>
#include <string>

constexpr UINT CP_SHIFT_JIS = 932;

static std::string utf8_to_shiftjis(const std::string_view utf8_str)
{
    if (utf8_str.empty())
    {
        return {};
    }
    // utf-8 -> utf-16
    int utf16_len = ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), -1, nullptr, 0);
    std::wstring utf16_str(utf16_len, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), -1, utf16_str.data(), utf16_len);

    // utf-16 -> shift-jis
    int sjis_len = ::WideCharToMultiByte(CP_SHIFT_JIS, 0, utf16_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string shift_jis_str(sjis_len, 0);
    ::WideCharToMultiByte(CP_SHIFT_JIS, 0, utf16_str.c_str(), -1, shift_jis_str.data(), sjis_len, nullptr, nullptr);
    shift_jis_str.pop_back();
    return shift_jis_str;
}

static std::wstring ascii_to_utf16(const std::string_view shift_jis_str, UINT code_page)
{
    if (shift_jis_str.empty())
    {
        return {};
    }

    int wide_len = ::MultiByteToWideChar(code_page, 0, shift_jis_str.data(), -1, nullptr, 0);
    std::wstring wstr(wide_len, 0);
    ::MultiByteToWideChar(code_page, 0, shift_jis_str.data(), -1, wstr.data(), wide_len);
    wstr.pop_back();
    return wstr;
}

extern "C" VOID CALLBACK DetourFinishHelperProcess(
    _In_ HWND,
    _In_ HINSTANCE,
    _In_ LPSTR,
    _In_ INT
);

auto import_dll = ::DetourFinishHelperProcess;

int main(int argc, char** argv)
{
    using namespace boost::ut;

    "GetACP hook"_test = [] {
        auto const code_page = ::GetACP();
        auto const shift_jis_str = utf8_to_shiftjis("ハローワールド");
        bool is_eq = (ascii_to_utf16(shift_jis_str, code_page) == std::wstring{L"ハローワールド"});
        expect(is_eq);
    };

    // Don't optimize
    if (!import_dll)
    {
        return -1;
    }
}

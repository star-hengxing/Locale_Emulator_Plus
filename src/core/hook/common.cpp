#include "core/global.hpp"
#include "common.hpp"

string<wchar_t> char2wide(const char* src, UINT code_page) noexcept
{
    if (!src)
    {
        return {};
    }

    auto const src_size = ::lstrlenA(src);
    auto const dst_size = ::MultiByteToWideChar(code_page, 0, src, src_size, nullptr, 0);
    auto const dst_capacity = (dst_size + 1) * sizeof(wchar_t);
    auto const dst = reinterpret_cast<wchar_t*>(std::malloc(dst_capacity));

    [[maybe_unused]] auto const result = ::MultiByteToWideChar(code_page, 0, src, src_size, dst, dst_size);

    dst[dst_size] = L'\0';
    return {dst};
}

string<char> wide2char(const wchar_t* src, UINT code_page) noexcept
{
    if (!src)
    {
        return {};
    }

    auto const src_size = ::lstrlenW(src);
    auto const dst_size = ::WideCharToMultiByte(code_page, 0, src, src_size, nullptr, 0, nullptr, nullptr);
    auto const dst_capacity = (dst_size + 1) * sizeof(char);
    auto const dst = reinterpret_cast<char*>(std::malloc(dst_capacity));

    [[maybe_unused]] auto const result = ::WideCharToMultiByte(code_page, 0, src, src_size, dst, dst_size, nullptr, nullptr);

    dst[dst_size] = '\0';
    return {dst};
}

string<wchar_t> shiftjis2gbk(const wchar_t* const src) noexcept
{
    return char2wide(wide2char(src, (UINT)Encoding::gbk), (UINT)Encoding::shift_jis);
}

#include "global.hpp"
#include "table.hpp"
#include "base/macro.hpp"

NAMESPACE_BEGIN()

template <typename char_type>
struct string
{
public:
    string(char_type* ptr = nullptr) :
        ptr(ptr)
    {}

    ~string()
    {
        if (ptr)
        {
            std::free(ptr);
            ptr = nullptr;
        }
    }

    operator const char_type*() noexcept
    {
        return ptr;
    }

public:
    char_type* ptr = nullptr;
};

string<wchar_t> char2wide(const char* src, UINT code_page = CP_ACP) noexcept
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

string<char> wide2char(const wchar_t* src, UINT code_page = CP_ACP) noexcept
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

NAMESPACE_END()

NAMESPACE_BEGIN(hook)

template <typename T>
auto get_original_function_ptr(hook_function_help index) noexcept
{
    // TODO: c++23 std::to_underlying
    return reinterpret_cast<T>(HOOK_LIST[static_cast<usize>(index)].src);
}

int WINAPI MessageBoxA(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType)
{
    return ::MessageBoxW(hWnd, char2wide(lpText), char2wide(lpCaption), uType);
}

BOOL WINAPI SetWindowTextA(
    _In_ HWND hWnd,
    _In_opt_ LPCSTR lpString)
{
    auto const original = get_original_function_ptr<decltype(&::SetWindowTextW)>(hook_function_help::SetWindowTextW);
    return original(hWnd, char2wide(lpString));
}

BOOL WINAPI SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString)
{
    auto const original = get_original_function_ptr<decltype(&::SetWindowTextW)>(hook_function_help::SetWindowTextW);
    return original(hWnd, char2wide(wide2char(lpString, (UINT)Encoding::gbk), (UINT)Encoding::shift_jis));
}

NAMESPACE_END(hook)

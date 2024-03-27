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

string<wchar_t> shiftjis2gbk(const wchar_t* const src) noexcept
{
    return char2wide(wide2char(src, (UINT)Encoding::gbk), (UINT)Encoding::shift_jis);
}

NAMESPACE_END()

NAMESPACE_BEGIN(hook)

template <hook_function_help fn_name>
struct hook_struct_help;

#define REGISTER(fn_name)                                \
    template <>                                          \
    struct hook_struct_help<hook_function_help::fn_name> \
    {                                                    \
        using fn_type = decltype(&::fn_name);            \
    };

template <hook_function_help T>
auto get_original_function_ptr() noexcept
{
    // TODO: c++23 std::to_underlying
    return reinterpret_cast<hook_struct_help<T>::fn_type>(HOOK_LIST[static_cast<usize>(T)].src);
}

REGISTER(MessageBoxA)
REGISTER(MessageBoxW)
REGISTER(SetWindowTextA)
REGISTER(SetWindowTextW)
REGISTER(RegisterClassExA)
REGISTER(RegisterClassExW)

int WINAPI MessageBoxA(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType)
{
    auto const original = get_original_function_ptr<hook_function_help::MessageBoxW>();
    return original(hWnd, char2wide(lpText, global_info.code_page), char2wide(lpCaption, global_info.code_page), uType);
}

int WINAPI MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType)
{
    auto const original = get_original_function_ptr<hook_function_help::MessageBoxW>();
    return original(hWnd, shiftjis2gbk(lpText), shiftjis2gbk(lpCaption), uType);
}

BOOL WINAPI SetWindowTextA(
    _In_ HWND hWnd,
    _In_opt_ LPCSTR lpString)
{
    auto const original = get_original_function_ptr<hook_function_help::SetWindowTextW>();
    return original(hWnd, char2wide(lpString));
}

BOOL WINAPI SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString)
{
    auto const original = get_original_function_ptr<hook_function_help::SetWindowTextW>();
    return original(hWnd, shiftjis2gbk(lpString));
}

ATOM WINAPI RegisterClassExA(_In_ CONST WNDCLASSEXA* src)
{
    auto const original = get_original_function_ptr<hook_function_help::RegisterClassExW>();
    auto const class_name = char2wide(src->lpszClassName);
    auto const menu_name = char2wide(src->lpszMenuName);

    auto dst = *reinterpret_cast<const WNDCLASSEXW*>(src);
    dst.lpszClassName = class_name.ptr;
    dst.lpszMenuName = menu_name.ptr;
    return original(&dst);
}

ATOM WINAPI RegisterClassExW(_In_ CONST WNDCLASSEXW* src)
{
    auto const original = get_original_function_ptr<hook_function_help::RegisterClassExW>();
    auto const class_name = shiftjis2gbk(src->lpszClassName);
    auto const menu_name = shiftjis2gbk(src->lpszMenuName);

    WNDCLASSEXW dst = *src;
    dst.lpszClassName = class_name.ptr;
    dst.lpszMenuName = menu_name.ptr;
    return original(&dst);
}

DWORD WINAPI GetModuleFileNameA(
    _In_opt_ HMODULE hModule,
    _Out_writes_to_(nSize, ((return < nSize) ? (return +1) : nSize)) LPSTR lpFilename,
    _In_ DWORD nSize)
{
    wchar_t filename[MAX_PATH];
    auto const ret = ::GetModuleFileNameW(hModule, filename, nSize);
    ::WideCharToMultiByte(global_info.code_page, 0, filename, MAX_PATH, lpFilename, MAX_PATH, nullptr, nullptr);
    return ret;
}

NAMESPACE_END(hook)

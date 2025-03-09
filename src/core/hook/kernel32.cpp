#include <core/global.hpp>
#include "common.hpp"
#include "table.hpp"

NAMESPACE_BEGIN(hook)

int WINAPI MessageBoxA(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType)
{
    auto const original = get_original_function_ptr(MessageBoxW);
    return original(hWnd, char2wide(lpText, global_info.code_page), char2wide(lpCaption, global_info.code_page), uType);
}

int WINAPI MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType)
{
    auto const original = get_original_function_ptr(MessageBoxW);
    return original(hWnd, shiftjis2gbk(lpText), shiftjis2gbk(lpCaption), uType);
}

ATOM WINAPI RegisterClassExA(_In_ CONST WNDCLASSEXA* src)
{
    auto const original = get_original_function_ptr(RegisterClassExA);
    return original(src);
    // auto const original = get_original_function_ptr(RegisterClassExW);
    // auto const class_name = char2wide(src->lpszClassName);
    // auto const menu_name = char2wide(src->lpszMenuName);

    // auto dst = *reinterpret_cast<const WNDCLASSEXW*>(src);
    // dst.lpszClassName = class_name.ptr;
    // dst.lpszMenuName = menu_name.ptr;
    // return original(&dst);
}

ATOM WINAPI RegisterClassExW(_In_ CONST WNDCLASSEXW* src)
{
    auto const original = get_original_function_ptr(RegisterClassExW);
    return original(src);
    // auto const class_name = shiftjis2gbk(src->lpszClassName);
    // auto const menu_name = shiftjis2gbk(src->lpszMenuName);

    // WNDCLASSEXW dst = *src;
    // dst.lpszClassName = class_name.ptr;
    // dst.lpszMenuName = menu_name.ptr;
    // return original(&dst);
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

HANDLE WINAPI CreateFileW(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
    )
{
    auto const original = get_original_function_ptr(CreateFileW);
    return original(shiftjis2gbk(lpFileName), dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

}

NAMESPACE_END(hook)

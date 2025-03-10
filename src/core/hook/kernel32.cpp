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

HWND WINAPI CreateWindowExW(
    _In_ DWORD dwExStyle,
    _In_opt_ LPCWSTR lpClassName,
    _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam)
{
    return get_original_function_ptr(CreateWindowExW)(
        dwExStyle,
        lpClassName,
        shiftjis2gbk(lpWindowName),
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );
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

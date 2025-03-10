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
    return ::MessageBoxW(hWnd, char2wide(lpText, global_info.code_page), char2wide(lpCaption, global_info.code_page), uType);
}

UINT WINAPI GetACP()
{
    return global_info.code_page;
}

UINT WINAPI GetOEMCP()
{
    return global_info.code_page;
}

NAMESPACE_END(hook)

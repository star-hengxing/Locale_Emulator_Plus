#include <core/global.hpp>
#include "common.hpp"
#include "table.hpp"

NAMESPACE_BEGIN(hook)

BOOL WINAPI SetWindowTextA(
    _In_ HWND hWnd,
    _In_opt_ LPCSTR lpString)
{
    return ::SetWindowTextW(hWnd, char2wide(lpString, global_info.code_page));
}

NAMESPACE_END(hook)

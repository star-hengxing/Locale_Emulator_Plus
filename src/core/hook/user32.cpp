#include <core/global.hpp>
#include "common.hpp"
#include "table.hpp"

NAMESPACE_BEGIN(hook)

BOOL WINAPI SetWindowTextA(
    _In_ HWND hWnd,
    _In_opt_ LPCSTR lpString)
{
    auto const original = get_original_function_ptr(SetWindowTextW);
    return original(hWnd, char2wide(lpString));
}

BOOL WINAPI SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString)
{
    auto const original = get_original_function_ptr(SetWindowTextW);
    return original(hWnd, shiftjis2gbk(lpString));
}

NAMESPACE_END(hook)

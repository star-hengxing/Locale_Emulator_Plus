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

BOOL WINAPI InsertMenuItemA(
    _In_ HMENU hmenu,
    _In_ UINT item,
    _In_ BOOL fByPosition,
    _In_ LPCMENUITEMINFOA lpmi)
{
    if (!lpmi || !lpmi->dwTypeData)
    {
        return get_original_function_ptr(InsertMenuItemA)(hmenu, item, fByPosition, lpmi);
    }

    auto info = *reinterpret_cast<const MENUITEMINFOW*>(lpmi);
    auto const str = char2wide(lpmi->dwTypeData, global_info.code_page);
    info.dwTypeData = str.ptr;
    return ::InsertMenuItemW(hmenu, item, fByPosition, &info);
}

NAMESPACE_END(hook)

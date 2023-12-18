#include "table.hpp"

NAMESPACE_BEGIN(hook)

template <typename T>
auto get_source_function_ptr(hook_function_help func) noexcept
{
    return (T)(HOOK_LIST[(int)func].src);
}

int WINAPI MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType)
{
    auto const type = ::MessageBoxW;
    auto const func = get_source_function_ptr<decltype(type)>(hook_function_help::MessageBoxW);
    return func(hWnd, L"hook", lpCaption, uType);
}

BOOL WINAPI SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString)
{
    auto const type = ::SetWindowTextW;
    auto const func = get_source_function_ptr<decltype(type)>(hook_function_help::SetWindowTextW);
    return func(hWnd, L"hook");
}

NAMESPACE_END(hook)

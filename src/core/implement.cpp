#include "table.hpp"

NAMESPACE_BEGIN(hook)

template <typename T>
auto get_source_function_ptr(hook_function_help index) noexcept
{
    // TODO: c++23 std::to_underlying
    return reinterpret_cast<T>(HOOK_LIST[static_cast<usize>(index)].src);
}

int WINAPI MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType)
{
    auto const func = get_source_function_ptr<decltype(&::MessageBoxW)>(hook_function_help::MessageBoxW);
    return func(hWnd, L"hook", lpCaption, uType);
}

BOOL WINAPI SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString)
{
    auto const func = get_source_function_ptr<decltype(&::SetWindowTextW)>(hook_function_help::SetWindowTextW);
    return func(hWnd, L"hook");
}

NAMESPACE_END(hook)

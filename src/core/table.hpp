#pragma once

#include <iterator>

#include <windows.h>

#include "base/base.hpp"

struct hook_function
{
    void* src = nullptr;
    void* dst = nullptr;
};

NAMESPACE_BEGIN(hook)

int
WINAPI
MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
    _In_ UINT uType);

BOOL
WINAPI
SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString);

NAMESPACE_END(hook)

enum class hook_function_help
{
    MessageBoxW = 0,
    SetWindowTextW,
    MAX,
};

inline hook_function HOOK_LIST[]
{
    {::MessageBoxW, ::hook::MessageBoxW},
    {::SetWindowTextW, ::hook::SetWindowTextW},
};

static_assert(static_cast<usize>(hook_function_help::MAX) == std::size(HOOK_LIST));

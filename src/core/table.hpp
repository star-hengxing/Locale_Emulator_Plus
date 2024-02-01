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
MessageBoxA(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);

BOOL
WINAPI
SetWindowTextA(
    _In_ HWND hWnd,
    _In_opt_ LPCSTR lpString);

BOOL
WINAPI
SetWindowTextW(
    _In_ HWND hWnd,
    _In_opt_ LPCWSTR lpString);

int
WINAPI
MultiByteToWideChar(
    _In_ UINT CodePage,
    _In_ DWORD dwFlags,
    _In_NLS_string_(cbMultiByte) LPCCH lpMultiByteStr,
    _In_ int cbMultiByte,
    _Out_writes_to_opt_(cchWideChar,return) LPWSTR lpWideCharStr,
    _In_ int cchWideChar
    );

NAMESPACE_END(hook)

enum class hook_function_help
{
    MessageBoxA = 0,
    SetWindowTextA,
    SetWindowTextW,
    MAX,
};

inline hook_function HOOK_LIST[]
{
    {::MessageBoxA, ::hook::MessageBoxA},
    {::SetWindowTextA, ::hook::SetWindowTextA},
    {::SetWindowTextW, ::hook::SetWindowTextW},
};

static_assert(static_cast<usize>(hook_function_help::MAX) == std::size(HOOK_LIST));

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

int
WINAPI
MessageBoxW(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCWSTR lpText,
    _In_opt_ LPCWSTR lpCaption,
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
    MessageBoxW,
    SetWindowTextA,
    SetWindowTextW,
    MAX,
};

#define HOOK_LIST_HELP(fn_name) {::fn_name, ::hook::fn_name}

inline hook_function HOOK_LIST[]
{
    HOOK_LIST_HELP(MessageBoxA),
    HOOK_LIST_HELP(MessageBoxW),
    HOOK_LIST_HELP(SetWindowTextA),
    HOOK_LIST_HELP(SetWindowTextW),
};

static_assert(static_cast<usize>(hook_function_help::MAX) == std::size(HOOK_LIST));

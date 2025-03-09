#pragma once

#include "base/basic_type.hpp"
#include <base/base.hpp>

#define get_original_function_ptr(fn) reinterpret_cast<decltype(&::fn)>(HOOK_LIST[static_cast<usize>(hook_function_help::fn)].src)

enum class hook_function_help
{
    MessageBoxA = 0,
    MessageBoxW,
    SetWindowTextA,
    SetWindowTextW,
    CreateFileW,
    RegisterClassExA,
    RegisterClassExW,
    GetModuleFileNameA,
    MAX,
};

struct hook_function
{
    void* src = nullptr;
    void* dst = nullptr;
};

extern hook_function HOOK_LIST[];
extern const usize HOOK_LIST_SIZE;

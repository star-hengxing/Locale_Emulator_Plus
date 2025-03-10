#include <iterator>

#include "table.hpp"

#include "kernel32.cpp"
#include "user32.cpp"

#define HOOK_LIST_HELP(fn_name) {reinterpret_cast<void*>(::fn_name), reinterpret_cast<void*>(::hook::fn_name)}

hook_function HOOK_LIST[]
{
    HOOK_LIST_HELP(MessageBoxA),
    HOOK_LIST_HELP(SetWindowTextA),
    HOOK_LIST_HELP(GetACP),
    HOOK_LIST_HELP(GetOEMCP),
};

const usize HOOK_LIST_SIZE = std::size(HOOK_LIST);

static_assert(static_cast<usize>(hook_function_help::MAX) == HOOK_LIST_SIZE);

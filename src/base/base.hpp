#pragma once

#include "macro.hpp"
#include "basic_type.hpp"

NAMESPACE_BEGIN(global)

inline constexpr auto is_debug
{
#ifdef LIBRARY_DEBUG
    true
#else
    false
#endif
};

inline constexpr auto is_release = !is_debug;

#if __cplusplus <= 202202L

[[noreturn]] inline auto unreachable()
{
// https://en.cppreference.com/w/cpp/utility/unreachable
#ifdef __GNUC__ // GCC, Clang, ICC
    __builtin_unreachable();
#else
#ifdef _MSC_VER // MSVC
    __assume(false);
#endif
#endif
}

#endif

NAMESPACE_END(global)

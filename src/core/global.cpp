#include "global.hpp"

void initialize() noexcept
{
    global_info.code_page = ::GetACP();
    // ::GetProcessHeap();
}

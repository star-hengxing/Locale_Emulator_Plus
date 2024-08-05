#pragma once
#include <cstddef>
#include <windows.h>


enum class NlsPatcherSysVer
{
    Nls_Windows8_x32,
    Nls_Windows8_1_x32,
    Nls_Windows10_x32,
    Nls_Windows11_x32
};

auto NlsPatcher(const NlsPatcherSysVer eSysVer, const HANDLE hProcess, const std::size_t nCodePage) -> bool;
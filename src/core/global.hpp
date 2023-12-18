#pragma once

#include <windows.h>

enum class Encoding : UINT
{
    shift_jis = 932,
    gbk = 936,
    big5 = 950,
};

struct locale_info
{
    UINT code_page;
};

inline locale_info global_info;

void initialize() noexcept;

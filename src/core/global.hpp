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
    void* heap = nullptr;
};

constexpr auto GLOBAL_PIPENAME = R"(\\.\pipe\Locale Emulator Plus)";

inline locale_info global_info;

void initialize() noexcept;

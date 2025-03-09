#pragma once

#include <cstdlib>

#include <Windows.h>

template <typename char_type>
struct string
{
public:
    string(char_type* ptr = nullptr) :
        ptr(ptr)
    {}

    ~string()
    {
        if (ptr)
        {
            std::free(ptr);
            ptr = nullptr;
        }
    }

    operator const char_type*() noexcept
    {
        return ptr;
    }

public:
    char_type* ptr = nullptr;
};

string<wchar_t> char2wide(const char* src, UINT code_page = CP_ACP) noexcept;

string<char> wide2char(const wchar_t* src, UINT code_page = CP_ACP) noexcept;

string<wchar_t> shiftjis2gbk(const wchar_t* const src) noexcept;

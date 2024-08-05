#pragma once
#include <cstddef>
#include <windows.h>


namespace LEP::Utils
{
    class NlsPatcher
    {
    public:
        enum class SysVer
        {
            Nls_Windows8_x32,
            Nls_Windows8_1_x32,
            Nls_Windows10_x32,
            Nls_Windows11_x32
        };

    private:
        SysVer m_eSysVer;
        HANDLE m_hProcess;
        std::size_t m_nCodePage;

    public:
        NlsPatcher() {};
        NlsPatcher(const HANDLE hProcess, const SysVer eSysVer, const std::size_t nCodePage) : m_hProcess{ hProcess }, m_eSysVer{ eSysVer }, m_nCodePage{ nCodePage } {};

    public:
        auto Install() const -> bool;

    public:
        static auto Install(const HANDLE hProcess, const SysVer eSysVer, const std::size_t nCodePage) -> bool;
    };
} // namespace name



#pragma once
#include <cstddef>
#include <utility>
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
        SysVer m_eSysVer{};
        HANDLE m_hProcess{};
        std::size_t m_nCodePage{};

    public:
        NlsPatcher() {};
        NlsPatcher(const HANDLE hProcess, const SysVer eSysVer, const std::size_t nCodePage) : m_hProcess{ hProcess }, m_eSysVer{ eSysVer }, m_nCodePage{ nCodePage } {};

    public:
        auto Install() -> bool;

    public:
        static auto SaveInfoViaEnv(const std::size_t nHookVA, const std::size_t nOldProtect) -> void;
        static auto LoadInfoViaEnv() -> std::pair<std::size_t, std::size_t>;
        static auto RestoreProtection() -> void;

    public:
        static auto Install(const HANDLE hProcess, const SysVer eSysVer, const std::size_t nCodePage) -> bool;
    };
} // namespace name



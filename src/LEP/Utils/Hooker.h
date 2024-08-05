#pragma once
#include <cstddef>
#include <LEP/Utils/Detour.h>


namespace LEP::Utils
{
    template<auto FnDetour>
    class Hooker
    {
    public:
        using FnType_t = decltype(FnDetour);

    public:
        inline static FnType_t FnRaw;

    public:
        static auto Attach(Hooker::FnType_t pRawFunc) -> void
        {
            Hooker::FnRaw = pRawFunc;
            Detour::Attach(&FnRaw, FnDetour);
        }

        static auto Attach(const std::size_t nRawFuncVA) -> void
        {
            Hooker::Attach(reinterpret_cast<Hooker::FnType_t>(nRawFuncVA));
        }

        static auto Attach(const std::size_t nImageBase, const std::size_t nRva) -> void
        {
            Hooker::Attach(reinterpret_cast<Hooker::FnType_t>(nImageBase + nRva));
        }

        static auto Commit(Hooker::FnType_t pRawFunc) -> void
        {
            Detour::CommitBeg();
            Hooker::Attach(pRawFunc);
            Detour::CommitEnd();
        }

        static auto Commit(const std::size_t nImageBase, const std::size_t nRva) -> void
        {
            Hooker::Commit(reinterpret_cast<Hooker::FnType_t>(nImageBase + nRva));
        }
    };
} // namespace LEP::Utils




#pragma once
#include <windows.h>
#include <detours.h>


class SingleHookerAUX
{
public:
    static auto CommitBeg() -> void
    {
        ::DetourTransactionBegin();
        ::DetourUpdateThread(GetCurrentThread());
    }

    static auto CommitEnd() -> void
    {
        ::DetourTransactionCommit();
    }

    static auto Register(void* ppFunc, void* pDetour) -> void
    {
        ::DetourAttach((PVOID*)ppFunc, pDetour);
    }
};

template<auto FnDetour>
class SingleHooker
{
public:
    using FnType_t = decltype(FnDetour);

public:
    inline static FnType_t FnRaw;

public:
    static auto Register(SingleHooker::FnType_t pRawFunc) -> void
    {
        SingleHooker::FnRaw = pRawFunc;
        SingleHookerAUX::Register(&FnRaw, FnDetour);
    }

    static auto Register(std::size_t nRawFuncVA) -> void
    {
        SingleHooker::Register(reinterpret_cast<SingleHooker::FnType_t>(nRawFuncVA));
    }

    static auto Register(size_t nImageBase, size_t nRva) -> void
    {
        SingleHooker::Register(reinterpret_cast<SingleHooker::FnType_t>(nImageBase + nRva));
    }

    static auto Commit(SingleHooker::FnType_t pRawFunc) -> void
    {
        SingleHookerAUX::CommitBeg();
        SingleHooker::Register(pRawFunc);
        SingleHookerAUX::CommitEnd();
    }

    static auto Commit(size_t nImageBase, size_t nRva) -> void
    {
        SingleHooker::Commit(reinterpret_cast<SingleHooker::FnType_t>(nImageBase + nRva));
    }
};

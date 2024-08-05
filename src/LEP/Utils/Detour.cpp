#include <LEP/Utils/Detour.h>
#include <windows.h>
#include <detours.h>


namespace LEP::Utils
{
    auto Detour::CommitBeg() -> void
    {
        ::DetourTransactionBegin();
        ::DetourUpdateThread(GetCurrentThread());
    }

    auto Detour::CommitEnd() -> void
    {
        ::DetourTransactionCommit();
    }

    auto Detour::Attach(void* ppFunc, void* pDetour) -> void
    {
        ::DetourAttach((PVOID*)ppFunc, pDetour);
    }

    auto Detour::AfterWith() -> void
    {
        ::DetourRestoreAfterWith();
    }

} // namespace LEP::Utils

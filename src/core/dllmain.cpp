#include <span>

#include <windows.h>
#include <detours.h>

#include "hook/table.hpp"
#include "global.hpp"

static void attach() noexcept
{
    for (auto& api : std::span{HOOK_LIST, HOOK_LIST_SIZE})
    {
        ::DetourAttach(
            std::addressof(reinterpret_cast<PVOID&>(api.src)),
            reinterpret_cast<PVOID>(api.dst));
    }
}

static void detach() noexcept
{
    for (auto& api : std::span{HOOK_LIST, HOOK_LIST_SIZE})
    {
        ::DetourDetach(
            std::addressof(reinterpret_cast<PVOID&>(api.src)),
            reinterpret_cast<PVOID>(api.dst));
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (::DetourIsHelperProcess())
        return TRUE;

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        ::initialize();

        ::DetourRestoreAfterWith();

        ::DetourTransactionBegin();
        ::DetourUpdateThread(::GetCurrentThread());

        ::attach();

        ::DetourTransactionCommit();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        ::DetourTransactionBegin();
        ::DetourUpdateThread(GetCurrentThread());

        ::detach();

        ::DetourTransactionCommit();
    }
    return TRUE;
}

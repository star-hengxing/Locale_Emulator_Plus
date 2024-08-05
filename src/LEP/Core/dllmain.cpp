#include <Windows.h>
#include <LEP/Utils/Hooker.h>
#include <LEP/Core/Win32uHook.h>


static auto StartHook(HMODULE /* hDllBase */) -> void
{
    LEP::Utils::Detour::AfterWith();
    LEP::Core::Win32u::InstallHook();
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::StartHook(hModule);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" VOID __declspec(dllexport) LEP_EXPORT_FUNC() {}
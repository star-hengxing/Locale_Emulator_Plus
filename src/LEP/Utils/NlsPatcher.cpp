#include <LEP/Utils/NlsPatcher.h>
#include <cstdint>
#include <array>
#include <string>
#include <optional>


// virtual address helper
namespace LEP::Utils::VA
{
    static auto Alloc(const HANDLE hProcess, const std::size_t nBytes) -> std::size_t
    {
        return reinterpret_cast<std::size_t>(::VirtualAllocEx(hProcess, NULL, nBytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    }

    static auto SetProtect(const HANDLE hProcess, const std::size_t nVA, const std::size_t nBytes, const std::size_t nProtect) -> std::size_t
    {
        DWORD old;
        const auto status = ::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(nVA), static_cast<SIZE_T>(nBytes), static_cast<DWORD>(nProtect), &old);
        return (status != FALSE) ? static_cast<std::size_t>(old) : 0;
    }

    static auto Write(const HANDLE hProcess, const std::size_t nVA, const void* pData, const std::size_t nBytes) -> bool
    {
        SIZE_T written;
        const auto status = ::WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(nVA), pData, static_cast<SIZE_T>(nBytes), &written);
        if (!status) { return false; }
        return written == static_cast<SIZE_T>(nBytes);
    }

    static auto WriteAsmJmp(const HANDLE hProcess, const std::size_t nSrcVA, const std::size_t nDestVA) -> bool
    {
        std::array<std::uint8_t, 5> jmp_instr{ 0xE9, 0x00, 0x00, 0x00, 0x00 };
        const auto jmp_offset = reinterpret_cast<std::uint32_t*>(jmp_instr.data() + 1);
        jmp_offset[0] = static_cast<std::uint32_t>(nDestVA - nSrcVA - 5);
        return VA::Write(hProcess, nSrcVA, jmp_instr.data(), jmp_instr.size());
    }
} // namespace LEP::Utils::VA

namespace LEP::Utils
{
    // support for windows8 8.1 10 x32
    static auto NlsPatch_V1_x32(const HANDLE hProcess, const std::size_t nCodePage) -> std::optional<std::size_t>
    {
        auto [fn_NtGetNlsSectionPtr_va, fn_RtlInitNlsTables_va] = []() -> std::pair<std::size_t, std::size_t> {
            const auto ntdll_handle = ::GetModuleHandleW(L"ntdll.dll");
            if (ntdll_handle == NULL) { return std::pair{0, 0}; }
            const auto fn_va_0 = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "NtGetNlsSectionPtr"));
            const auto fn_va_1 = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "RtlInitNlsTables"));
            return std::pair{fn_va_0, fn_va_1};
        }();

        if ((fn_NtGetNlsSectionPtr_va == 0) || (fn_RtlInitNlsTables_va == 0)) { std::nullopt; }

        /*
        ** _PEB* peb_ptr = __readfsdword(0x30);
        ** SIZE_T code_page_data_size;
        ** NtGetNlsSectionPtr(0xB, 0x3A4, 0, &peb_ptr->AnsiCodePageData, &code_page_data_size);
        ** peb_ptr->OemCodePageData = peb_ptr->AnsiCodePageData
        */

        std::array<std::uint8_t, 72> shell_code
        {
            0x60,                                // pushad
            0x9C,                                // pushfd
            0x64,0x8B,0x35, 0x30,0x00,0x00,0x00, // mov esi,dword ptr fs:[30]          // _PEB* peb_ptr = __readfsdword(0x30);
            0x8D,0x4E,0x58,                      // lea ecx,dword ptr ds:[esi+58]      // 
            0x8D,0x56,0x5C,                      // lea edx,dword ptr ds:[esi+5C]      // 
            0x51,                                // push ecx                           // 
            0x52,                                // push edx                           // 
            0x6A,00,                             // push 0                             // SIZE_T code_page_data_size;
            0x8D,0x04,0x24,                      // lea eax,dword ptr ss:[esp]         // NtGetNlsSectionPtr(0xB, 0x3A4, 0, &peb_ptr->AnsiCodePageData, &code_page_data_size);
            0x50,                                // push eax                           // 
            0x51,                                // push ecx                           // 
            0x6A,0x00,                           // push 0x0                           // 
            0x68, 0x00,0x00,0x00,0x00,           // push {CodePage}                    // 
            0x6A, 0x0B,                          // push 0xB                           // 
            0xE8, 0x00,0x00,0x00,0x00,           // call ntdll.NtGetNlsSectionPtr      // 
            0x58,                                // pop eax                            
            0x5A,                                // pop edx                            
            0x59,                                // pop ecx                            
            0x8B,0x01,                           // mov eax, dword ptr ds:[ecx]        // peb_ptr->OemCodePageData = peb_ptr->AnsiCodePageData
            0x89,0x02,                           // mov dword ptr ds:[edx], eax        // 
            0xB8,0x00,0x00,0x00,0x00,            // mov eax,{NtRtlInitNlsTables}       // recover NtRtlInitNlsTables
            0xC7,0x00,0x8B,0xFF,0x55,0x8B,       // mov dword ptr ds:[eax], 0x8B55FF8B //
            0xC6,0x40,0x04,0xEC,                 // mov byte ptr ds:[eax+0x4], 0xEC    //
            0x8B,0x02,                           // mov eax, dword ptr ds:[edx]        //
            0x89,0x44,0x24,0x28,                 // mov dword ptr ss:[esp+0x28], eax   // change RtlInitNlsTables param_0
            0x89,0x44,0x24,0x2C,                 // mov dword ptr ss:[esp+0x2C], eax   // change RtlInitNlsTables param_1
            0x9D,                                // popfd
            0x61                                 // popad
        };

        // alloc shellcode memory in target process
        const auto shell_code_mem_va = VA::Alloc(hProcess, 1024);
        if (shell_code_mem_va == 0) { return std::nullopt; }

        // config shellcode
        {
            const auto shellcode_codepage_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 27);
            shellcode_codepage_ptr[0] = static_cast<const std::uint32_t>(nCodePage);

            const auto shellcode_call_NtGetNlsSectionPtr_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 34);
            shellcode_call_NtGetNlsSectionPtr_ptr[0] = static_cast<const std::uint32_t>(fn_NtGetNlsSectionPtr_va - (shell_code_mem_va + 33) - 5);

            const auto shellcode_NtRtlInitNlsTables_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 46);
            shellcode_NtRtlInitNlsTables_ptr[0] = static_cast<const std::uint32_t>(fn_RtlInitNlsTables_va);
        }

        // write shellcode
        {
            // write shellcode to shell_code_mem
            const auto shellcode_write_status = VA::Write(hProcess, shell_code_mem_va, shell_code.data(), shell_code.size());
            if (shellcode_write_status == false) { return std::nullopt; }

            // modify RtlInitNlsTables va protection properties
            const auto old_protection = VA::SetProtect(hProcess, fn_RtlInitNlsTables_va, 5, PAGE_EXECUTE_READWRITE);
            if (old_protection == 0) { return std::nullopt; }

            // write jmp instr at the begin of NtRtlInitNlsTables
            const auto jmp_instr_write_status = VA::WriteAsmJmp(hProcess, fn_RtlInitNlsTables_va, shell_code_mem_va);
            if (jmp_instr_write_status == false) { return std::nullopt; }

            // write jmp instr at the end of shell_code_mem that jmp back begin of NtRtlInitNlsTables
            const auto jmp_back_instr_write_status = VA::WriteAsmJmp(hProcess, shell_code_mem_va + shell_code.size(), fn_RtlInitNlsTables_va);
            if (jmp_back_instr_write_status == false) { return std::nullopt; }

            return old_protection;
        }
    }

    static auto NlsPatch_V2_x32(const HANDLE hProcess, const std::size_t nCodePage) -> std::optional<std::size_t>
    {
        // find NtGetNlsSectionPtr and RtlInitNlsTables virtual address
        auto [fn_NtGetNlsSectionPtr_va, fn_RtlInitCodePageTable_va] = []() -> std::pair<std::size_t, std::size_t> {
            const auto ntdll_handle = ::GetModuleHandleW(L"ntdll.dll");
            if (ntdll_handle == NULL) { return std::pair{0, 0}; }
            const auto fn_va_0 = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "NtGetNlsSectionPtr"));
            const auto fn_va_1 = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "RtlInitCodePageTable"));
            return std::pair{fn_va_0, fn_va_1};
        }();

        if ((fn_NtGetNlsSectionPtr_va == 0) || (fn_RtlInitCodePageTable_va == 0)) { std::nullopt; }
        /*
        ** _PEB* peb_ptr = __readfsdword(0x30);
        ** SIZE_T code_page_data_size;
        ** NtGetNlsSectionPtr(0xB, 0x3A4, 0, &peb_ptr->AnsiCodePageData, &code_page_data_size);
        ** peb_ptr->OemCodePageData = peb_ptr->AnsiCodePageData
        */

        std::array<std::uint8_t, 94> shell_code
        {
            0x60,                                     // pushad
            0x9C,                                     // pushfd
            0xB8, 0x00,0x00,0x00,0x00,                // mov eax, { CodePage }
            0xBB, 0x00,0x00,0x00,0x00,                // mov ebx, { ntdll.NtGetNlsSectionPtr }
            0xB9, 0x00,0x00,0x00,0x00,                // mov ecx, { ntdll.RtlInitCodePageTable }
            0x64, 0x8B, 0x15, 0x30,0x00,0x00,0x00,    // mov edx,dword ptr fs:[30]
            0x0F, 0xB7, 0xB2, 0x28,0x02,0x00,0x00,    // movzx esi,word ptr ds:[edx+228] 
            0x39, 0xC6,                               // cmp esi,eax
            0x75, 0x0A,                               // jne 
            0xC7, 0x01, 0x8B,0xFF,0x55,0x8B,          // mov dword ptr ds:[ecx],8B55FF8B
            0xC6, 0x41, 0x04, 0xEC,                   // mov byte ptr ds:[ecx+4],EC
            0x66, 0x89, 0x82, 0x28,0x02,0x00,0x00,    // mov word ptr ds:[edx+228],ax
            0x66, 0x89, 0x82, 0x2A,0x02,0x00,0x00,    // mov word ptr ds:[edx+22A],ax
            0x53,                                     // push ebx
            0x6A, 0x00,                               // push 0
            0x6A, 0x00,                               // push 0
            0x8D, 0x14, 0x24,                         // lea edx,dword ptr ss:[esp]
            0x52,                                     // push edx
            0x8D, 0x54, 0x24, 0x08,                   // lea edx,dword ptr ss:[esp+8]
            0x52,                                     // push edx
            0x6A, 0x00,                               // push 0
            0x50,                                     // push eax
            0x6A, 0x0B,                               // push B
            0xFF, 0xD3,                               // call ebx
            0x58,                                     // pop eax
            0x5B,                                     // pop ebx
            0x59,                                     // pop ecx
            0x89, 0x5C, 0x24, 0x28,                   // mov dword ptr ss:[esp+28],ebx
            0x9D,                                     // popfd
            0x61,                                     // popad
            0x8B, 0xFF,                               // mov edi, edi
            0x55,                                     // push ebp
            0x8B, 0xEC                                // mov ebp, esp
        };

        // alloc shellcode memory in target process
        const auto shell_code_mem_va = VA::Alloc(hProcess, 1024);
        if (shell_code_mem_va == 0) { return std::nullopt; }

        // config shellcode
        {
            const auto shellcode_codepage_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 3);
            shellcode_codepage_ptr[0] = static_cast<const std::uint32_t>(nCodePage);

            const auto shellcode_call_NtGetNlsSectionPtr_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 8);
            shellcode_call_NtGetNlsSectionPtr_ptr[0] = static_cast<const std::uint32_t>(fn_NtGetNlsSectionPtr_va);

            const auto shellcode_RtlInitCodePageTable_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 13);
            shellcode_RtlInitCodePageTable_ptr[0] = static_cast<const std::uint32_t>(fn_RtlInitCodePageTable_va);
        }

        // write shellcode
        {
            // write shellcode to shell_code_mem
            const auto shellcode_write_status = VA::Write(hProcess, shell_code_mem_va, shell_code.data(), shell_code.size());
            if (shellcode_write_status == false) { return std::nullopt; }

            // modify RtlInitNlsTables va protection properties
            const auto old_protection = VA::SetProtect(hProcess, fn_RtlInitCodePageTable_va, 5, PAGE_EXECUTE_READWRITE);
            if (old_protection == 0) { return std::nullopt; }

            // write jmp instr at the begin of NtRtlInitNlsTables
            const auto jmp_instr_write_status = VA::WriteAsmJmp(hProcess, fn_RtlInitCodePageTable_va, shell_code_mem_va);
            if (jmp_instr_write_status == false) { return std::nullopt; }

            // write jmp instr at the end of shell_code_mem that jmp back begin of NtRtlInitNlsTables
            const auto jmp_back_instr_write_status = VA::WriteAsmJmp(hProcess, shell_code_mem_va + shell_code.size(), fn_RtlInitCodePageTable_va + 5);
            if (jmp_back_instr_write_status == false) { return std::nullopt; }

            return old_protection;
        }
    }
    }// namespace LEP::Utils

namespace LEP::Utils
{
    auto NlsPatcher::Install() -> bool
    {
        switch (m_eSysVer)
        {
        case SysVer::Nls_Windows8_x32:
        case SysVer::Nls_Windows8_1_x32:
        case SysVer::Nls_Windows10_x32:
        {
            const auto protection_opt = NlsPatch_V1_x32(m_hProcess, m_nCodePage);
            if (!protection_opt.has_value()) { return false; }
        }
        break;

        case SysVer::Nls_Windows11_x32:
        {
            const auto protection_opt = NlsPatch_V2_x32(m_hProcess, m_nCodePage);
            if (!protection_opt.has_value()) { return false; }
        }
        }

        return true;
    }

    auto NlsPatcher::SaveInfoViaEnv(const std::size_t nHookVA, const std::size_t nOldProtect) -> void
    {
#ifdef _WIN64
        LPCWSTR format = L"%ull";
#else
        LPCWSTR format = L"%ul";
#endif // _WIN64

        WCHAR buffer[64];
        ::wsprintfW(buffer, format, nHookVA);
        ::SetEnvironmentVariableW(L"LEP_NLS_PATCHER_HOOK_VA", buffer);

        ::wsprintfW(buffer, format, nOldProtect);
        ::SetEnvironmentVariableW(L"LEP_NLS_PATCHER_PROTECTION", buffer);

    }

    auto NlsPatcher::LoadInfoViaEnv() -> std::pair<std::size_t, std::size_t>
    {
#ifdef _WIN64
        LPCWSTR format = L"%ull";
#else
        LPCWSTR format = L"%ul";
#endif // _WIN64
        WCHAR buffer[64];

        ::GetEnvironmentVariableW(L"LEP_NLS_PATCHER_HOOK_VA", buffer, 64);
        std::size_t hook_va{};
        ::swscanf_s(buffer, format, &hook_va);

        GetEnvironmentVariableW(L"LEP_NLS_PATCHER_PROTECTION", buffer, 64);
        std::size_t old_protect{};
        ::swscanf_s(buffer, format, &old_protect);

        return std::pair{ hook_va, old_protect };
    }

    auto NlsPatcher::RestoreProtection() -> void
    {
        auto [hook_va, old_protect] = NlsPatcher::LoadInfoViaEnv();
        VA::SetProtect(::GetCurrentProcess(), hook_va, 5, old_protect);
    }

    auto NlsPatcher::Install(const HANDLE hProcess, const SysVer eSysVer, const std::size_t nCodePage) -> bool
    {
        NlsPatcher patcher{ hProcess,eSysVer,nCodePage };
        return patcher.Install();
    }
}// namespace LEP::Utils::VA



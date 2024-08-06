#include <LEP/Utils/NlsPatcher.h>
#include <LEP/Utils/VAHelper.h>
#include <LEP/Utils/SysInfo.h>
#include <array>
#include <cstdint>
#include <cstdio>


namespace LEP::Utils
{
	// for Windows 7 8 8.1 10  x32
	static auto ShellCodeV1_x32(uint8_t* pBuffer) -> std::size_t
	{
		/*
		** _PEB* peb_ptr = __readfsdword(0x30);
		** SIZE_T code_page_data_size;
		** NtGetNlsSectionPtr(0xB, 0x3A4, 0, &peb_ptr->AnsiCodePageData, &code_page_data_size);
		** peb_ptr->OemCodePageData = peb_ptr->AnsiCodePageData
		*/

		std::array<std::uint8_t, 104> shell_code
		{
			0x60,                                     // pushad
			0x9C,                                     // pushfd
			0xE8,0x10,0x00,0x00,0x00,                 // call lable_0

			// Data
			0x00,0x00,0x00,0x00,                      // info_struct.CodePage
			0x00,0x00,0x00,0x00,                      // info_struct.NtGetNlsSectionPtr
			0x00,0x00,0x00,0x00,                      // info_struct.RtlInitCodePageTable
			0x00,0x00,0x00,0x00,                      // info_struct.NlsMapVA

			// lable_0: test info_struct.NlsMapVA
			0x58,                                     // pop eax
			0x31,0xDB,                                // xor ebx,ebx
			0x8B,0x48,0x0C,							  // mov ecx,dword ptr ds:[eax+C]   // info_struct.NlsMapVA
			0x39,0xCB,								  // cmp ebx,ecx
			0x75,0x2F,								  // jne label_1

			0x50,									  // push eax                        // Save info_struct ptr
			0x8B,0x18,								  // mov ebx,dword ptr ds:[eax]      // load info_struct.CodePage

			// load nls map table														 //
			0x6A,0x00,								  // push 0							 //
			0x6A,0x00,								  // push 0							 //
			0x8D,0x0C,0x24,							  // lea ecx,dword ptr ss:[esp]		 //
			0x8D,0x54,0x24,0x04,					  // lea edx,dword ptr ss:[esp+4]	 //
			0x51,									  // push ecx						 // param_4: SectionSizePtr :
			0x52,									  // push edx						 // param_3: SectionPtrPtr  :
			0x6A,0x00,								  // push 0							 // param_2: Unknown        :
			0x53,									  // push ebx						 // param_1: CodePage       :
			0x6A,0x0B,								  // push B							 // param_0: NlsType        :
			0xFF,0x50,0x04,							  // call dword ptr ds:[eax+4]       // call info_struct.NtGetNlsSectionPtr
			0x5B,									  // pop ebx						 //
			0x59,									  // pop ecx						 //
			0x58,									  // pop eax						 // Restore info_struct ptr

			// patch PEB
			0x64,0x8B,0x35,0x30,0x00,0x00,0x00,		  // mov esi,dword ptr fs:[30]       // PEB
			0x8D,0x56,0x58,							  // lea edx,dword ptr ds:[esi+58]   // &PEB.AnsiCodePageData
			0x89,0x0A,								  // mov dword ptr ds:[edx],ecx      // PEB.AnsiCodePageData = nls_table_ptr
			0x89,0x4A,0x04,						      // mov dword ptr ds:[edx+4],ecx    // PEB.OemCodePageData = nls_table_ptr
			0x89,0x48,0x0C,							  // mov dword ptr ds:[eax+C],ecx    // info_struct.NlsMapVA = nls_table_ptr
			0xEB,0x0D,								  // jmp label_2

			// label_1: restore RtlInitCodePageTable header
			0x8B,0x58,0x08,							  // mov ebx,dword ptr ds:[eax+8]    // info_struct.RtlInitCodePageTable
			0xC7,0x03,0x8B,0xFF,0x55,0x8B,			  // mov dword ptr ds:[ebx],8B55FF8B // copy { {mov edi, edi}, {push ebp}, {mov ebp, esp} }
			0xC6,0x43,0x04,0xEC,					  // mov byte ptr ds:[ebx+4],EC      // 

			// label_2: modify RtlInitCodePageTable param0
			0x89,0x4C,0x24,0x28,					  // mov dword ptr ss:[esp+28],ecx
			0x9D,                                     // popfd
			0x61,                                     // popad
			0x8B, 0xFF,                               // mov edi, edi
			0x55,                                     // push ebp
			0x8B, 0xEC                                // mov ebp, esp
		};

		std::memcpy(pBuffer, shell_code.data(), shell_code.size());
		return shell_code.size();
	}

	// for Windows 11  x32
	static auto ShellCodeV2_x32(uint8_t* pBuffer) -> std::size_t
	{
		/*
		*  PEB* peb_ptr = __readfsdword(0x30);
		*  peb_ptr->ActiveCodePage = 0x3A4;
		*  peb_ptr->OemCodePage = 0x3A4;
		*  SIZE_T code_page_table_ptr;
		*  SIZE_T code_page_table_size;
		*  NtGetNlsSectionPtr(0xB, 0x3A4, 0, &code_page_table_ptr, &code_page_table_size);
		*  RtlInitCodePageTable(code_page_table_ptr, &GlobalRtlNlsState);
		*/
		std::array<std::uint8_t, 110> shell_code{
			0x60,                                     // pushad
			0x9C,                                     // pushfd
			0xE8,0x10,0x00,0x00,0x00,                 // call lable_0

			// Data
			0x00,0x00,0x00,0x00,                      // info_struct.CodePage
			0x00,0x00,0x00,0x00,                      // info_struct.NtGetNlsSectionPtr
			0x00,0x00,0x00,0x00,                      // info_struct.RtlInitCodePageTable
			0x00,0x00,0x00,0x00,                      // info_struct.NlsMapVA

			// lable_0: test info_struct.NlsMapVA
			0x58,                                     // pop eax
			0x31,0xDB,                                // xor ebx,ebx
			0x8B,0x48,0x0C,							  // mov ecx,dword ptr ds:[eax+C]   // info_struct.NlsMapVA
			0x39,0xCB,								  // cmp ebx,ecx
			0x75,0x35,								  // jne label_1

			// patch PEB
			0x50,									  // push eax                        // Save info_struct ptr
			0x8B,0x18,								  // mov ebx,dword ptr ds:[eax]      // info_struct.CodePage
			0x64,0x8B,0x15,0x30,0x00,0x00,0x00,		  // mov edx,dword ptr fs:[30]       // Get PEB
			0x66,0x89,0x9A,0x28,0x02,0x00,0x00,		  // mov word ptr ds:[edx+228],bx    // PEB.ActiveCodePage
			0x66,0x89,0x9A,0x2A,0x02,0x00,0x00,		  // mov word ptr ds:[edx+22A],bx    // PEB.OemCodePage

			// load nls map table														 //
			0x6A,0x00,								  // push 0							 //
			0x6A,0x00,								  // push 0							 //
			0x8D,0x0C,0x24,							  // lea ecx,dword ptr ss:[esp]		 //
			0x8D,0x54,0x24,0x04,					  // lea edx,dword ptr ss:[esp+4]	 //
			0x51,									  // push ecx						 // param_4: SectionSizePtr :
			0x52,									  // push edx						 // param_3: SectionPtrPtr  :
			0x6A,0x00,								  // push 0							 // param_2: Unknown        :
			0x53,									  // push ebx						 // param_1: CodePage       :
			0x6A,0x0B,								  // push B							 // param_0: NlsType        :
			0xFF,0x50,0x04,							  // call dword ptr ds:[eax+4]       // call info_struct.NtGetNlsSectionPtr
			0x5B,									  // pop ebx						 //
			0x59,									  // pop ecx						 //
			0x58,									  // pop eax						 // Restore info_struct ptr
			0x89,0x48,0x0C,							  // mov dword ptr ds:[eax+C],ecx	 //
			0xEB,0x0D,								  // jmp label_2					 //

			// label_1: restore RtlInitCodePageTable header								 //
			0x8B,0x58,0x08,							  // mov ebx,dword ptr ds:[eax+8]    // info_struct.RtlInitCodePageTable
			0xC7,0x03,0x8B,0xFF,0x55,0x8B,			  // mov dword ptr ds:[ebx],8B55FF8B // copy { {mov edi, edi}, {push ebp}, {mov ebp, esp} }
			0xC6,0x43,0x04,0xEC,					  // mov byte ptr ds:[ebx+4],EC      // 

			// label_2: modify RtlInitCodePageTable param0
			0x89,0x4C,0x24,0x28,					  // mov dword ptr ss:[esp+28],ecx
			0x9D,                                     // popfd
			0x61,                                     // popad
			0x8B, 0xFF,                               // mov edi, edi
			0x55,                                     // push ebp
			0x8B, 0xEC                                // mov ebp, esp
		};

		std::memcpy(pBuffer, shell_code.data(), shell_code.size());
		return shell_code.size();
	}

	static auto CodePageTablePatch_x32(const HANDLE hProcess, const std::size_t nCodePage, const bool isWin11) -> bool
	{
		const auto ntdll_handle = ::GetModuleHandleW(L"ntdll.dll");
		if (ntdll_handle == NULL) { return false; }
		const auto fn_NtGetNlsSectionPtr_va = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "NtGetNlsSectionPtr"));
		if (fn_NtGetNlsSectionPtr_va == 0) { return false; }
		const auto fn_RtlInitCodePageTable_va = reinterpret_cast<std::size_t>(::GetProcAddress(ntdll_handle, "RtlInitCodePageTable"));
		if (fn_RtlInitCodePageTable_va == 0) { return false; }

		// get shellcode
		std::uint8_t shell_code_buffer[512];
		const auto shell_code_bytes = isWin11 ? Utils::ShellCodeV2_x32(shell_code_buffer) : Utils::ShellCodeV1_x32(shell_code_buffer);
		const std::span<std::uint8_t> shell_code{ shell_code_buffer, shell_code_bytes };

		// create va_helper
		Utils::VAHelper va_helper{ hProcess };

		// alloc shellcode memory in target process
		const auto shell_code_mem_va = va_helper.Alloc(1024);
		if (shell_code_mem_va == 0) { return false; }

		// config shellcode
		{
			const auto info_strcut_ptr = reinterpret_cast<std::uint32_t*>(shell_code.data() + 7);
			info_strcut_ptr[0] = static_cast<const std::uint32_t>(nCodePage);
			info_strcut_ptr[1] = static_cast<const std::uint32_t>(fn_NtGetNlsSectionPtr_va);
			info_strcut_ptr[2] = static_cast<const std::uint32_t>(fn_RtlInitCodePageTable_va);
			info_strcut_ptr[3] = {};
		}

		// write shellcode
		{
			// write shellcode to shell_code_mem
			const auto shellcode_write_status = va_helper.Write(shell_code_mem_va, shell_code);
			if (shellcode_write_status == false) { return false; }

			// modify RtlInitNlsTables va protection properties
			const auto old_protection = va_helper.SetProtect(fn_RtlInitCodePageTable_va, 5, PAGE_EXECUTE_READWRITE);
			if (old_protection == 0) { return false; }

			// write jmp instr at the begin of NtRtlInitNlsTables
			const auto jmp_instr_write_status = va_helper.WriteAsmJmp(fn_RtlInitCodePageTable_va, shell_code_mem_va);
			if (jmp_instr_write_status == false) { return false; }

			// write jmp instr at the end of shell_code_mem that jmp back begin of NtRtlInitNlsTables
			const auto jmp_back_instr_write_status = va_helper.WriteAsmJmp(shell_code_mem_va + shell_code.size(), fn_RtlInitCodePageTable_va + 5);
			if (jmp_back_instr_write_status == false) { return false; }

			return Utils::NlsPatcher::SaveInfoViaEnv(fn_RtlInitCodePageTable_va, shell_code_mem_va, old_protection);
		}
	}

	static auto CodePageTablePatch_x64(const HANDLE /* hProcess*/, const std::size_t /* nCodePage */, const bool /* isWin11 */) -> bool
	{
		return false;
	}
} // namespace LEP::Utils

namespace LEP::Utils
{
	auto NlsPatcher::Install() const -> bool
	{
		const auto is_x64 = Utils::SysInfo::IsProcessx64(m_hProcess);
		const auto is_win11 = (Utils::SysInfo::GetBuildNumber() >= 22000);

		if (is_x64)
		{
			return CodePageTablePatch_x64(m_hProcess, m_nCodePage, is_win11);
		}
		else
		{
			return CodePageTablePatch_x32(m_hProcess, m_nCodePage, is_win11);
		}
	}

	auto NlsPatcher::AfterWith() -> bool
	{
#ifdef _WIN64
		LPCWSTR format = L"%ull";
#else
		LPCWSTR format = L"%ul";
#endif // _WIN64
		WCHAR buffer[64];
		BOOL get_status{};

		get_status = ::GetEnvironmentVariableW(L"LEP_NLS_PATCHER_HOOK_VA", buffer, 64);
		if (!get_status) { return false; }
		std::size_t hook_va{};
		::swscanf_s(buffer, format, &hook_va);
		if (!hook_va) { return false; }

		::GetEnvironmentVariableW(L"LEP_NLS_PATCHER_SHELLCODE_VA", buffer, 64);
		std::size_t shellcode_va{};
		::swscanf_s(buffer, format, &shellcode_va);
		if (!shellcode_va) { return false; }

		::GetEnvironmentVariableW(L"LEP_NLS_PATCHER_PROTECTION", buffer, 64);
		std::size_t old_protect{};
		::swscanf_s(buffer, format, &old_protect);
		if (!old_protect) { return false; }

		VAHelper va_helper{ ::GetCurrentProcess() };

		const auto free_status = va_helper.Free(shellcode_va);
		if (!free_status) { return false; }

		const auto set_protect_status = va_helper.SetProtect(hook_va, 5, old_protect);
		if (!set_protect_status) { return false; }

		return true;
	}

	auto NlsPatcher::SaveInfoViaEnv(const std::size_t nHookVA, const std::size_t nShellCodeVA, const std::size_t nOldProtect) -> bool
	{
#ifdef _WIN64
		LPCWSTR format = L"%ull";
#else
		LPCWSTR format = L"%ul";
#endif // _WIN64

		WCHAR buffer[64];
		BOOL set_status{};
		int format_status{};

		format_status = ::wsprintfW(buffer, format, nHookVA);
		if (!format_status) { return false; }
		set_status = ::SetEnvironmentVariableW(L"LEP_NLS_PATCHER_HOOK_VA", buffer);
		if (!set_status) { return false; }

		format_status = ::wsprintfW(buffer, format, nShellCodeVA);
		if (!format_status) { return false; }
		set_status = ::SetEnvironmentVariableW(L"LEP_NLS_PATCHER_SHELLCODE_VA", buffer);
		if (!set_status) { return false; }

		format_status = ::wsprintfW(buffer, format, nOldProtect);
		if (!format_status) { return false; }
		set_status = ::SetEnvironmentVariableW(L"LEP_NLS_PATCHER_PROTECTION", buffer);
		if (!set_status) { return false; }

		return true;
	}
} // namespace LEP::Utils

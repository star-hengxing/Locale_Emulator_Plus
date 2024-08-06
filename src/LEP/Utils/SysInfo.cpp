#include <LEP/Utils/SysInfo.h>
#include <cstdio>


namespace LEP::Utils
{
	auto SysInfo::GetBuildNumber() -> std::size_t
	{
		HKEY hkey{};
		const auto status = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", NULL, KEY_READ, &hkey);
		if (status == ERROR_SUCCESS)
		{
			DWORD type{};
			BYTE buffer[64];
			DWORD bytes = 62;
			::RegQueryValueExW(hkey, L"CurrentBuildNumber", nullptr, &type, buffer, &bytes);
			::RegCloseKey(hkey);

			if (type != REG_SZ) { return 0; }

			buffer[bytes + 0] = {};
			buffer[bytes + 1] = {};

			DWORD build_number{};
			::swscanf_s((LPCWSTR)buffer, L"%d", &build_number);
			return static_cast<std::size_t>(build_number);
		}

		return 0;
	}

	auto LEP::Utils::SysInfo::IsProcessx64(const HANDLE hProcess) -> bool
	{
		USHORT process_machine;
		const auto status = ::IsWow64Process2(hProcess, &process_machine, nullptr);
		if (!status) { return false; }

		return (process_machine == IMAGE_FILE_MACHINE_UNKNOWN);
	}
}
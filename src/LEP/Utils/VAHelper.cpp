#include <LEP/Utils/VAHelper.h>
#include <array>


namespace LEP::Utils
{
	VAHelper::VAHelper()
	{

	}

	VAHelper::VAHelper(const HANDLE hProcess) : m_hProcess{ hProcess }
	{

	};

	auto VAHelper::Alloc(const std::size_t nBytes) const -> std::size_t
	{
		return reinterpret_cast<std::size_t>(::VirtualAllocEx(m_hProcess, NULL, nBytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
	}

	auto VAHelper::Free(const std::size_t nVA) const -> bool
	{
		return (::VirtualFreeEx(m_hProcess, reinterpret_cast<LPVOID>(nVA), 0, MEM_RELEASE) != FALSE);
	}

	auto VAHelper::SetProtect(const std::size_t nVA, const std::size_t nBytes, const std::size_t nProtect) const -> std::size_t
	{
		DWORD old;
		const auto status = ::VirtualProtectEx(m_hProcess, reinterpret_cast<LPVOID>(nVA), static_cast<SIZE_T>(nBytes), static_cast<DWORD>(nProtect), &old);
		return (status != FALSE) ? static_cast<std::size_t>(old) : 0;
	}

	auto VAHelper::Write(const std::size_t nVA, const std::span<const std::uint8_t> spData) const -> bool
	{
		SIZE_T written;
		const auto status = ::WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(nVA), spData.data(), static_cast<SIZE_T>(spData.size_bytes()), &written);
		if (!status) { return false; }
		return written == static_cast<SIZE_T>(spData.size_bytes());
	}

	auto VAHelper::Read(const std::size_t nVA, const std::span<std::uint8_t> spBuffer) const -> bool
	{
		SIZE_T read;
		return (::ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(nVA), spBuffer.data(), static_cast<SIZE_T>(spBuffer.size_bytes()), &read) != FALSE);
	}

	auto VAHelper::Fill(const std::size_t nVA, const std::size_t nBytes, const std::uint8_t cVal) const -> void
	{
		SIZE_T written{};
		for (std::size_t idx = 0; idx < nBytes; idx++)
		{
			::WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(nVA + idx), &cVal, 1, &written);
		}
	}

	auto VAHelper::Copy(const std::size_t nDestVA, const std::size_t nSrcVA, const std::size_t nBytes) const -> bool
	{
		std::uint8_t tmp[1024];

		SIZE_T copied_bytes{};
		const auto data_bytes{ static_cast<SIZE_T>(nBytes) };
		while (copied_bytes != data_bytes)
		{
			SIZE_T read;
			::ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(nSrcVA + copied_bytes), tmp, 1024, &read);

			SIZE_T written;
			::WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(nDestVA + copied_bytes), tmp, read, &written);

			copied_bytes += written;
		}

		return true;
	}

	auto VAHelper::WriteAsmJmp(const std::size_t nSrcVA, const std::size_t nDestVA) const -> bool
	{
		std::array<std::uint8_t, 5> jmp_instr{ 0xE9, 0x00, 0x00, 0x00, 0x00 };
		const auto jmp_offset = reinterpret_cast<std::uint32_t*>(jmp_instr.data() + 1);
		jmp_offset[0] = static_cast<std::uint32_t>(nDestVA - nSrcVA - 5);
		return this->Write(nSrcVA, jmp_instr);
	}
}
#pragma once
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <span>


namespace LEP::Utils
{
	// virtual address helper
	class VAHelper
	{
	private:
		HANDLE m_hProcess{};

	public:
		VAHelper();
		VAHelper(const HANDLE hProcess);

	public:
		auto Alloc(const std::size_t nBytes) const -> std::size_t;
		auto Free(const std::size_t nVA) const -> bool;
		auto SetProtect(const std::size_t nVA, const std::size_t nBytes, const std::size_t nProtect) const -> std::size_t;
		auto Write(const std::size_t nVA, const std::span<const std::uint8_t> spData) const -> bool;
		auto Read(const std::size_t nVA, const std::span<std::uint8_t> spBuffer) const -> bool;
		auto Fill(const std::size_t nVA, const std::size_t nBytes, const std::uint8_t cVal) const -> void;
		auto Copy(const std::size_t nDestVA, const std::size_t nSrcVA, const std::size_t nBytes) const -> bool;
		auto WriteAsmJmp(const std::size_t nSrcVA, const std::size_t nDestVA) const -> bool;
	};
}


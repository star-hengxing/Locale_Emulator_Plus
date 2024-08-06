#pragma once
#include <cstddef>
#include <utility>
#include <windows.h>


namespace LEP::Utils
{
	class NlsPatcher
	{
	private:
		HANDLE m_hProcess{};
		std::size_t m_nCodePage{};

	public:
		NlsPatcher() {};
		NlsPatcher(const HANDLE hProcess, const std::size_t nCodePage) : m_hProcess{ hProcess }, m_nCodePage{ nCodePage } {};

	public:
		auto Install() const -> bool;

	public:
		static auto AfterWith() -> bool;
		static auto SaveInfoViaEnv(const std::size_t nHookVA, const std::size_t nShellCodeVA, const std::size_t nOldProtect) -> bool;

	};
} // namespace LEP::Utils



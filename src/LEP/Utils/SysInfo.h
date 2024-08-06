#pragma once
#include <Windows.h>
#include <cstddef>


namespace LEP::Utils
{
	class SysInfo
	{
	public:
		static auto GetBuildNumber() -> std::size_t;
		static auto IsProcessx64(const HANDLE hProcess) -> bool;
	};
}


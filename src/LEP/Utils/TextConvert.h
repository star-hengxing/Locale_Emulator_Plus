#pragma once
#include <memory>
#include <string_view>


namespace LEP::Utils
{
    auto MBCSToWide(const std::string_view msPath) -> std::unique_ptr<wchar_t[]>;
} // namespace LEP::Utils

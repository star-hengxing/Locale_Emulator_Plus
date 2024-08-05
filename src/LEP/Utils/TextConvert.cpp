#include <LEP/Utils/TextConvert.h>
#include <Windows.h>


namespace LEP::Utils
{
    auto MBCSToWide(const std::string_view msPath) -> std::unique_ptr<wchar_t[]>
    {
        const auto buffer_max_chars = (msPath.size() * sizeof(char) + 1) * 2;
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
        const auto char_count_real = static_cast<std::size_t>(::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(buffer_max_chars)));
        buffer[char_count_real] = {};
        return std::unique_ptr<wchar_t[]>{ std::move(buffer) };
    }
}



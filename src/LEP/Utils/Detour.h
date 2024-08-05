#pragma once


namespace LEP::Utils
{
    class Detour
    {
    public:
        static auto CommitBeg() -> void;
        static auto CommitEnd() -> void;
        static auto Attach(void* ppFunc, void* pDetour) -> void;
        static auto AfterWith() -> void;
    };

} // namespace LEP::Utils



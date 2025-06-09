-- hook backend(detours/minhook)

add_requires("microsoft-detours")

if is_mode("debug", "releasedbg") then
    add_requires("spdlog[shared=y,header_only=n,fmt_external,wchar,wchar_filenames,wchar_console]")
end

-- if has_config("minhook") then
--     add_requires("minhook")
-- end

if has_config("release") then
    add_requires("vc-ltl5")
end

-- test deps

if has_config("test") then
    add_requires("boost_ut")
end

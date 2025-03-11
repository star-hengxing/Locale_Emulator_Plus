-- hook backend(detours/minhook)

add_requires("microsoft-detours")

if is_mode("debug", "releasedbg") then
    add_requires("cpp-ipc f3bf137668ccdcb2a0398124d737426aa6fa9d30") -- 2025.03.08
end

-- if has_config("minhook") then
--     add_requires("minhook")
-- end

-- test deps

if has_config("test") then
    add_requires("boost_ut")
end

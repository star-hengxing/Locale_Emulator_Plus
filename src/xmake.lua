add_includedirs(os.scriptdir())
add_defines("WIN32_LEAN_AND_MEAN")

add_packages("microsoft-detours")
if is_mode("debug", "releasedbg") then
    add_packages("spdlog")
end
if has_config("release") then
    add_packages("vc-ltl5")
end

target("locale_emulator_plus")
    set_kind("shared")
    add_files(
        "core/*.cpp",
        "core/hook/common.cpp",
        "core/hook/table.cpp"
    )

    add_headerfiles("core/**.hpp", {install = false})
    add_cxxflags("cl::/wd4003")
    if is_mode("release") then
        add_cxxflags("cl::/kernel")
    end
    -- https://github.com/microsoft/Detours/wiki/DetourFinishHelperProcess
    add_shflags("/export:DetourFinishHelperProcess,@1", {tools = "link"})

    if is_plat("mingw") then
        -- libxxx -> xxx
        set_prefixname("")
        add_cxxflags("-fpermissive")
        add_files("core/export.def")
    end

    add_syslinks("user32")

    if has_config("test") then
        add_options("test")
    end

    on_load(function (target)
        local arch
        if target:is_arch("x86", "i386") then
            arch = "32"
        elseif target:is_arch("x64", "x86_64") then
            arch = "64"
        end

        target:set("basename", target:name() .. arch)
    end)

target("locale_emulator_plus_tool")
    set_kind("binary")
    add_files("tool/*.cpp")
    add_syslinks("shlwapi", "advapi32")

    add_deps("locale_emulator_plus", {inherit = false})

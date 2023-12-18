add_includedirs(os.scriptdir())
add_defines("WIN32_LEAN_AND_MEAN")

add_packages("microsoft-detours")

if is_plat("windows") and is_mode("release") then
    add_packages("vc-ltl5")
end

target("locale_emulator_plus")
    set_kind("shared")
    add_files("core/*.cpp")
    add_headerfiles("core/*.hpp")
    -- https://github.com/microsoft/Detours/wiki/DetourFinishHelperProcess
    add_shflags("/export:DetourFinishHelperProcess,@1,NONAME")

    add_syslinks("user32")

    on_load(function (target)
        local arch
        if target:is_arch("x86") then
            arch = "32"
        elseif target:is_arch("x64") then
            arch = "64"
        end

        target:set("basename", target:name() .. arch)
    end)

target("locale_emulator_plus_tool")
    set_default(false)
    set_kind("binary")
    add_files("tool/*.cpp")
    add_syslinks("shlwapi")

    add_deps("locale_emulator_plus", {inherit = false})

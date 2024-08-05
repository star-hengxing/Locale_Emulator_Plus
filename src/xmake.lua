add_includedirs(os.scriptdir())
add_defines("WIN32_LEAN_AND_MEAN")

if is_plat("windows") and is_mode("release") then
    add_packages("vc-ltl5")
end

target("lep_utils")
    set_kind("static")
    add_files("LEP/Utils/*.cpp")
    add_headerfiles("LEP/Utils/*.h")
    add_packages("microsoft-detours", {public = true})

target("lep_core")
    set_kind("shared")
    add_files("LEP/Core/*.cpp")
    add_headerfiles("LEP/Core/*.h")
    add_deps("lep_utils")

target("lep_loader")
    set_default(false)
    set_kind("binary")
    add_files("LEP/Loader/*.cpp")
    add_syslinks("user32")
    add_deps("lep_utils")

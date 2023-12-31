set_project("Locale Emulator Plus")

set_version("0.0.1")

set_xmakever("2.8.5")

set_allowedplats("windows", "mingw")
set_allowedarchs("windows|x86", "windows|x64", "mingw|x86_64", "mingw|i386")
set_allowedmodes("debug", "release")

set_languages("c++20")

set_warnings("all")
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_policy("build.warning", true)
elseif is_mode("release") then
    set_optimize("smallest")
end

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MD" or "MT")
    add_cxflags("/permissive-", {tools = "cl"})
end

set_encodings("source:utf-8")

includes("src", "xmake", "test")

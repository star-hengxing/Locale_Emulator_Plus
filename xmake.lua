set_project("Locale Emulator Plus")

set_version("0.0.1")

set_xmakever("2.9.8")

set_allowedplats("windows", "mingw")
set_allowedarchs("windows|x86", "windows|x64", "mingw|x86_64", "mingw|i386")
set_allowedmodes("debug", "release", "releasedbg")

includes("src", "xmake", "test")

-- fixed config
set_languages("c++20")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

if is_mode("debug", "releasedbg") then
    add_defines("PROJECT_DEBUG")
elseif is_mode("release") then
    set_optimize("smallest")
end

if is_plat("windows") then
    add_cxflags("cl::/permissive-")
end

set_encodings("utf-8")

-- dynamic config
if has_config("dev") then
    set_policy("compatibility.version", "3.0")
    set_policy("build.ccache", true)
    add_rules("plugin.compile_commands.autoupdate", {lsp = "clangd", outputdir = "build"})

    set_warnings("all")

    if is_plat("windows") then
        set_runtimes("MD")
    end
elseif has_config("release") then
    set_policy("build.optimization.lto", true)

    if is_plat("windows") then
        set_runtimes("MT")
    end
end

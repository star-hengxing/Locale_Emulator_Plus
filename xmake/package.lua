-- global package config

-- if is_mode("debug") then
--     add_requireconfs("*", {configs = {shared = true}})
-- end

-- dev deps

-- hook backend(detours/minhook)

add_requires("microsoft-detours")

if has_config("minhook") then
    add_requires("minhook")
end

if is_plat("windows") and is_mode("release") then
    add_requires("vc-ltl5")
end

-- test deps

if has_config("test") then
    add_requires("boost_ut", {optional = true})
end

-- custom package

package("minhook")
    set_homepage("https://github.com/TsudaKageyu/minhook")
    set_description("The Minimalistic x86/x64 API Hooking Library for Windows.")
    set_license("BSD-2-Clause")

    add_urls("https://github.com/TsudaKageyu/minhook.git")
    add_versions("2023.11.24", "f5485b8454544c2f034c78f8f127c1d03dea3636")

    on_install("windows|x86", "windows|x64", "mingw|x86_64", "mingw|i386", function (package)
        io.writefile("xmake.lua", [[
            add_rules("mode.debug", "mode.release")
            target("minhook")
                set_kind("$(kind)")
                add_includedirs("src", "src/hde")
                add_files("src/*.c", is_arch(".*64") and "src/hde/hde64.c" or "src/hde/hde32.c")
                if is_kind("shared") then
                    add_files("dll_resources/MinHook.rc", "dll_resources/MinHook.def")
                end
                add_headerfiles("include/MinHook.h")
                add_defines("WIN32_LEAN_AND_MEAN")
                add_cflags("/kernel", {tools = {"cl", "clang_cl"}})
        ]])
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("MH_Initialize", {includes = "MinHook.h"}))
    end)
package_end()

includes("@builtin/xpack")

xpack("release")
    set_formats("zip")
    set_extension(".7z")
    set_basename("Locale_Emulator_Plus")

    add_targets("locale_emulator_plus_tool")

    set_prefixdir("Locale_Emulator_Plus")

    set_bindir("")

    on_load(function (package)
        local locale_emulator_plus = package:target("locale_emulator_plus_tool"):dep("locale_emulator_plus")
        local dll = locale_emulator_plus:targetfile()

        local dll_2
        if locale_emulator_plus:is_arch64() then
            dll_2 = path.join(
                (locale_emulator_plus:targetdir():replace("x64", "x86", {plain = true})),
                (locale_emulator_plus:filename():replace("64", "32", {plain = true}))
            )
        else
            dll_2 = path.join(
                (locale_emulator_plus:targetdir():replace("x86", "x64", {plain = true})),
                (locale_emulator_plus:filename():replace("32", "64", {plain = true}))
            )
        end
        package:add("installfiles", dll, dll_2)
    end)

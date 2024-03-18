import("core.base")
import("utils.archive")

function _build(arch)
    os.vexecv(os.programfile(), {"config", "--clean", "--yes", "--mode=release", "--runtimes=MT", "--arch=" .. arch, "--policies=build.warning:n"})
    os.vexecv(os.programfile(), {"build", "--all"})
end

function main()
    local dir = "build/xpack/Locale_Emulator_Plus"
    os.mkdir("build/xpack/Locale_Emulator_Plus")

    _build("x64")
    os.cp("build/windows/x64/release/locale_emulator_plus64.dll", dir)

    _build("x86")
    os.cp("build/windows/x86/release/locale_emulator_plus_tool.exe", dir)
    os.cp("build/windows/x86/release/locale_emulator_plus32.dll", dir)

    local opt = {}
    opt.curdir = "build/xpack"

    local file = "build/xpack/Locale_Emulator_Plus.7z"
    os.tryrm(file)
    archive.archive("Locale_Emulator_Plus.7z", ".", opt)

    print("sha256:", hash.sha256(file))
end

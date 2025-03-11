target("test")
    set_default(false)
    set_kind("binary")
    add_files("main.cpp")

    add_deps("locale_emulator_plus")
    add_packages("boost_ut")

    add_tests("default")

option("dev")
    set_default(true)
    set_showmenu(true)
    set_description("Enable development mode")

option("test")
    set_default(false)
    set_showmenu(true)
    set_description("Enable test")
    add_defines("PROJECT_TEST")

-- option("minhook")
--     set_default(false)
--     set_showmenu(true)
--     set_description("Enable minhook backend")

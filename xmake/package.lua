-- dev

-- if is_mode("debug") then
--     add_requireconfs("*", {configs = {shared = true}})
-- end

add_requires("microsoft-detours")

if is_plat("windows") and is_mode("release") then
    add_requires("vc-ltl5")
end

-- test

add_requires("boost_ut", {optional = true})

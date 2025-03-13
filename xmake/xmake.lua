-- third party libraries
includes("requires.lua")
-- project option
includes("option.lua")

if has_config("release") then
    includes("pack.lua")
end

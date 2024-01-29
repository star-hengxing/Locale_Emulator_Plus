rule("module.program")
    on_load(function (target)
        target:set("kind", "binary")
        target:set("rundir", os.projectdir())
        if target:is_plat("windows") and target:get("runtimes") == "MT" then
            target:add("packages", "vc-ltl5")
        end
    end)

rule("module.component")
    on_load(function (target)
        if is_mode("debug") then
            target:set("kind", "shared")
            if target:is_plat("windows") then
                import("core.project.rule")
                local rule = rule.rule("utils.symbols.export_all")
                target:rule_add(rule)
                target:extraconf_set("rules", "utils.symbols.export_all", {export_classes = true})
            end
        elseif is_mode("release") then
            target:set("kind", "static")
        end
    end)

rule("module.test")
    on_load(function (target)
        target:set("default", false)
        target:set("policy", "build.warning", true)
        target:set("rundir", os.projectdir())
        target:set("group", "test")
        target:add("packages", "boost_ut")
    end)

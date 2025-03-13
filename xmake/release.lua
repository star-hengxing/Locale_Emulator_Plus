function main()
    local argv =
    {
        "config",
        "--clean",
        "--yes",
        "--dev=n",
        "--test=n",
        "--release=y"
    }
    os.vexecv(os.programfile(), table.join(argv, "--arch=x86"))
    os.vexecv(os.programfile())
    os.vexecv(os.programfile(), argv)
    os.vexecv(os.programfile(), {"pack"})
end

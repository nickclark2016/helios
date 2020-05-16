project "glad"
    kind "StaticLib"
    language "C"

    targetdir (binaries)
    objdir (intermediate)

    files {
        "include/**.h",
        "src/**.c"
    }

    includedirs {
        "%{IncludeDir.glad}"
    }

    filter "system:windows"
        cdialect "C11"
        systemversion "latest"
        staticruntime "Off"

    filter "system:linux"
        toolset "clang"
        buildoptions "-std=c11"
        staticruntime "Off"

    filter {}
project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir (binaries)
    objdir (intermediate)

    dependson {
        "containers",
        "ecs",
        "googletest",
        "math",
    }

    links {
        "containers",
        "ecs",
        "googletest",
        "math",
    }

    files {
        "src/main.cpp"
    }

    includedirs {
        "%{IncludeDir.containers}",
        "%{IncludeDir.gtest}",
        "%{IncludeDir.math}",
    }

    filter "system:windows"
        toolset "msc-ClangCL"
        systemversion "latest"
        staticruntime "Off"

        postbuildcommands {
            "\"" .. binaries .. "\\tests.exe\""
        }

    filter "system:linux"
        toolset "clang"
        staticruntime "Off"
        links {
            "pthread"
        }

        postbuildcommands {
            "\"" .. binaries .. "/tests\""
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        runtime "Release"
        symbols "Off"
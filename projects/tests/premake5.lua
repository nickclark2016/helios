project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir (binaries)
    objdir (intermediate)

    dependson {
        "googletest",
        "math",
    }

    links {
        "googletest",
        "math",
    }

    files {
        "src/main.cpp"
    }

    postbuildcommands {
        binaries .. "/tests"
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

    filter "system:linux"
        toolset "clang"
        staticruntime "Off"
        links {
            "pthread"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        runtime "Release"
        symbols "Off"
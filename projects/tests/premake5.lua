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

        buildoptions {
            "-fms-extensions"
        }

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
project "ecs"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir (binaries)
    objdir (intermediate)

    dependson {
        "containers"
    }

    links {
        "containers"
    }
    
    files {
        "include/**.hpp",
        "src/**.cpp",
        "src**.hpp", -- private headers
    }

    includedirs {
        "%{IncludeDir.containers}",
        "%{IncludeDir.ecs}",
        "src", -- private headers
    }

    flags {
        "MultiProcessorCompile"
    }

    filter "system:windows"
        toolset "msc-ClangCL"
        systemversion "latest"
        staticruntime "Off"

        ignoredefaultlibraries {
            "LIBCMT",
            "LIBCMTD"
        }

        defines {
            "HELIOS_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
        }
    
    filter "system:linux"
        toolset "clang"

        staticruntime "Off"

        defines {
            "HELIOS_PLATFORM_LINUX"
        }

        buildoptions {
            "-Wall",
            "-Wextra",
            "-Werror"
        }

        linkoptions {
            "-ldl"
        }

    filter "configurations:Debug"
        defines { 
            "HELIOS_DEBUG",
            "HELIOS_ENABLE_ASSERTS"
        }

        staticruntime "Off"
        runtime "Debug"

        symbols "On"

    filter "configurations:Release"
        defines { 
            "HELIOS_RELEASE",
            "NDEBUG"
        }

        optimize "Full"
        runtime "Release"
        staticruntime "Off"
        symbols "Off"
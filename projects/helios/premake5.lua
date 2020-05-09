project "helios"
    kind "StaticLib"
    language "C++"

    targetdir (binaries)
    objdir (intermediate)

    dependson {
        "glad",
        "glfw",
    }

    files {
        "include/**.hpp",
        "src/**.cpp"
    }

    includedirs {
        "%{IncludeDir.glad}",
        "%{IncludeDir.helios}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.vma}",
    }

    links {
        "glad",
        "glfw",
    }

    flags {
        "MultiProcessorCompile"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        staticruntime "Off"

        ignoredefaultlibraries {
            "LIBCMT",
            "LIBCMTD"
        }

        disablewarnings {
            "4005"
        }

        linkoptions {
            "/ignore:4006",
            "/ignore:4075",
            "/ignore:4099",
            "/ignore:4221",
        }

        buildoptions {
            "/wd26439",
            "/wd26812"
        }

        defines {
            "HELIOS_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
        }
    
    filter "system:linux"
        toolset "clang"
        cppdialect "C++17"

        staticruntime "Off"

        defines {
            "HELIOS_PLATFORM_LINUX"
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

        staticruntime "Off"
        runtime "Release"

        optimize "On"

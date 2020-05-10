project "application"
    kind "ConsoleApp"
    language "C++"

    targetdir (binaries)
    objdir (intermediate)
    debugdir("%{sln.location}")

    dependson {
        "glad",
        "glfw",
        "helios",
    }

    files {
        "include/**.hpp",
        "src/**.cpp"
    }

    includedirs {
        "%{IncludeDir.application}",
        "%{IncludeDir.helios}",
    }

    links {
        "helios",
        "glfw",
        "glad",
    }

    defines {
    	"GLFW_INCLUDE_NONE",
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
            "4005",
            "4006",
            "4075",
            "4099",
            "4221",
        }

        defines {
            "NIGHTWING_PLATFORM_WINDOWS"
        }

    filter "system:linux"
        toolset "clang"
        cppdialect "C++17"

        defines {
            "NIGHTWING_PLATFORM_LINUX"
        }

        linkoptions {
            "-ldl",
            "-lX11",
            "-pthread"
        }

        links {
            "glad",
            "glfw",
        }

    filter "configurations:Debug"
        defines { 
            "NIGHTWING_DEBUG",
            "NIGHTWING_ENABLE_ASSERTS"
        }

        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { 
            "NIGHTWING_RELEASE",
			"NDEBUG"
        }

        runtime "Release"
        optimize "On"
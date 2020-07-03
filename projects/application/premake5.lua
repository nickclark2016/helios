project "application"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir (binaries)
    objdir (intermediate)
    debugdir("%{sln.location}")

    dependson {
        "containers",
        "core",
        "ecs",
        "glad",
        "glfw",
        "math",
    }

    links {
        "containers",
        "core",
        "ecs",
        "glfw",
        "glad",
        "math",
    }

    files {
        "include/**.hpp",
        "src/**.cpp"
    }

    includedirs {
        "%{IncludeDir.application}",
        "%{IncludeDir.containers}",
        "%{IncludeDir.core}",
        "%{IncludeDir.ecs}",
        "%{IncludeDir.math}",
        "%{IncludeDir.stb}", -- TODO: Remove once abstraction created
    }

    defines {
    	"GLFW_INCLUDE_NONE",
    }
    
    filter "system:windows"
        toolset "msc-ClangCL"
        systemversion "latest"
        staticruntime "Off"

        ignoredefaultlibraries {
            "LIBCMT",
            "LIBCMTD"
        }

        buildoptions {
            "-Wno-missing-braces",
        }

        defines {
            "HELIOS_PLATFORM_WINDOWS"
        }

    filter "system:linux"
        toolset "clang"

        defines {
            "HELIOS_PLATFORM_LINUX"
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
            "HELIOS_DEBUG",
            "HELIOS_ENABLE_ASSERTS"
        }

        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { 
            "HELIOS_RELEASE",
            "NDEBUG"
        }

        optimize "Full"
        runtime "Release"
        symbols "Off"

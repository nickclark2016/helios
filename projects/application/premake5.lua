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
        "glad",
        "glfw",
        "math",
    }

    links {
        "containers",
        "core",
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
            "NIGHTWING_PLATFORM_WINDOWS"
        }

    filter "system:linux"
        toolset "clang"

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

        optimize "Full"
        runtime "Release"
        symbols "Off"

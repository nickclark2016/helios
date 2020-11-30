project "core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir (binaries)
    objdir (intermediate)

    dependson {
        "containers",
        "glad",
        "glfw",
        "math",
    }

    links {
        "containers",
        "glad",
        "glfw",
        "math",
    }
    
    defines {
        "GLFW_INCLUDE_NONE"
    }

    files {
        "include/**.hpp",
        "src/**.cpp",
        "src/**.hpp", -- private headers
    }
    
    includedirs {
        "%{IncludeDir.containers}",
        "%{IncludeDir.core}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.fxgltf}",
        "%{IncludeDir.json}",
        "%{IncludeDir.math}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.vma}",
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
            "-fms-extensions"
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

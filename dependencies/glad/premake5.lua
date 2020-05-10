project "glad"
    kind "StaticLib"
    language "C"

    targetdir (binaries)
    objdir (intermediate)

    prebuildcommands {
        "glad --out-path=. --api=vulkan --extensions=\"VK_KHR_surface,VK_KHR_swapchain,VK_EXT_debug_utils\" c --loader --header"
    }

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
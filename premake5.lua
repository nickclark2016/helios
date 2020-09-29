-- Main Premake
workspace "helios"
    configurations {
        "Debug",
        "Release",
    }

    filter "configurations:Debug"
        runtime "Debug"

    filter "configurations:Release"
        runtime "Release"

    filter{}

    architecture "x64"

    sln = solution()
    binaries = "%{sln.location}/bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"
    intermediate = "%{sln.location}/bin-int/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"

    -- Dependencies
    include "dependencies/glad"
    include "dependencies/glfw"
    include "dependencies/googletest"
    
    -- Projects
    include "projects/application"
    include "projects/containers"
    include "projects/core"
    include "projects/math"
    include "projects/tests"

    -- Include Directories
    IncludeDir = {}
    IncludeDir["application"] = "%{sln.location}/projects/application/include"
    IncludeDir["containers"] = "%{sln.location}/projects/containers/include"
    IncludeDir["core"] = "%{sln.location}/projects/core/include"
    IncludeDir["entt"] = "%{sln.location}/dependencies/entt/include"
    IncludeDir["fxgltf"] = "%{sln.location}/dependencies/fx-gltf/include"
    IncludeDir["glad"] = "%{sln.location}/dependencies/glad/include"
    IncludeDir["glfw"] = "%{sln.location}/dependencies/glfw/include"
    IncludeDir["gtest"] = "%{sln.location}/dependencies/googletest/include"
    IncludeDir["json"] = "%{sln.location}/dependencies/json/include"
    IncludeDir["math"] = "%{sln.location}/projects/math/include"
    IncludeDir["stb"] = "%{sln.location}/dependencies/stb/include"
    IncludeDir["tf"] = "%{sln.location}/dependencies/taskflow/include"
    IncludeDir["vma"] = "%{sln.location}/dependencies/vma/include"
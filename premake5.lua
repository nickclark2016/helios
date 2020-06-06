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
    
    -- Projects
    include "projects/containers"
    include "projects/core"
    include "projects/application"

    -- Include Directories
    IncludeDir = {}
    IncludeDir["glad"] = "%{sln.location}/dependencies/glad/include"
    IncludeDir["glfw"] = "%{sln.location}/dependencies/glfw/include"
    IncludeDir["vma"] = "%{sln.location}/dependencies/vma/include"
    IncludeDir["containers"] = "%{sln.location}/projects/containers/include"
    IncludeDir["core"] = "%{sln.location}/projects/core/include"
    IncludeDir["application"] = "%{sln.location}/projects/application/include"
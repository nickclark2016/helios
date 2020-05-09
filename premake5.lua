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

    include "dependencies/glad"
    include "dependencies/glfw"
    include "projects/helios"
    include "projects/application"

    -- Include Directories
    IncludeDir = {}
    IncludeDir["glad"] = "%{sln.location}/dependencies/glad/include"
    IncludeDir["glfw"] = "%{sln.location}/dependencies/glfw/include"
    IncludeDir["vma"] = "%{sln.location}/dependencies/vma/include"
    IncludeDir["helios"] = "%{sln.location}/projects/helios/include"
    IncludeDir["application"] = "%{sln.location}/projects/application/include"
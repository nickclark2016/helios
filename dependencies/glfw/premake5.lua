project "glfw"
    kind "StaticLib"
    language "C"
    
    targetdir (binaries)
    objdir (intermediate)

    files {
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
        "src/glfw_config.h",
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c"
    }
    
    filter "system:windows"
        cdialect "C11"
        systemversion "latest"
        staticruntime "Off"

        links {
            "gdi32"
        }
        
        files {
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_time.c",
            "src/win32_thread.c",
            "src/win32_window.c",
            "src/wgl_context.c",
            "src/egl_context.c",
            "src/osmesa_context.c"
        }

        defines { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        toolset "clang"
        buildoptions "-std=c11"
        staticruntime "Off"

        files {
            "src/egl_context.c",
            "src/glx_context.c",
            "src/linux_joystick.c",
            "src/osmesa_context.c",
            "src/posix_thread.c",
            "src/posix_time.c",
            "src/x11_init.c",
            "src/x11_monitor.c",
            "src/x11_window.c",
            "src/xkb_unicode.c"
        }

        defines {
            "_GLFW_X11",
            "BUILD_SHARED_LIBS"
        }

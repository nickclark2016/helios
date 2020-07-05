#include <helios/core/window.hpp>

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

namespace helios
{
    static bool glfw_init()
    {
        const bool init = glfwInit();

        atexit([]() { glfwTerminate(); });

        return init;
    }

    static bool GlfwInitializer = glfw_init();

    struct WindowBuilder::WindowBuilderImpl
    {
        std::string title;
        u32 width = ~0U;
        u32 height = ~0U;
        bool resizable = false;
        u32 monitor = ~0U;
    };

    struct GlfwWindow final : IWindow
    {
        GlfwWindow() = default;
        ~GlfwWindow() override;

        [[nodiscard]] std::string title() const override;
        [[nodiscard]] u32 width() const override;
        [[nodiscard]] u32 height() const override;
        [[nodiscard]] bool shouldClose() const override;
        void poll() const override;
        void close() override;
        const Keyboard& getKeyboard() const noexcept override;
        const Mouse& getMouse() const noexcept override;

        GLFWwindow* window = nullptr;
        std::string windowTitle;
        Keyboard keyboard;
        Mouse mouse;

        GlfwWindow(const GlfwWindow&) = delete;
        GlfwWindow(GlfwWindow&&) noexcept = delete;
        GlfwWindow& operator=(const GlfwWindow&) = delete;
        GlfwWindow& operator=(GlfwWindow&&) noexcept = delete;
    };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
    static void key_callback(GLFWwindow* win, i32 key, i32 scan, i32 action, i32 mods)
    {
        GlfwWindow* window = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(win));
        window->keyboard.setStatus(static_cast<EKey>(key), static_cast<EKeyStatus>(action));
    }

    static void mouse_button_callback(GLFWwindow* win, i32 btn, i32 action, i32 mods)
    {
        GlfwWindow* window = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(win));
        window->mouse.setStatus(static_cast<EMouseButton>(btn), static_cast<EMouseButtonStatus>(action));
    }

    static void cursor_position_callback(GLFWwindow* win, f64 x, f64 y)
    {
        GlfwWindow* window = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(win));
        window->mouse.setCursor(static_cast<f32>(x), static_cast<u32>(y));
    }

    static void scroll_callback(GLFWwindow* win, f64 x, f64 y)
    {
        GlfwWindow* window = reinterpret_cast<GlfwWindow*>(glfwGetWindowUserPointer(win));
        window->mouse.setScroll(static_cast<f32>(x));
    }
#pragma clang diagnostic pop

    GLFWwindow* as_native(const IWindow* window)
    {
        if (const GlfwWindow* win = dynamic_cast<const GlfwWindow*>(window))
        {
            return win->window;
        }
        return nullptr;
    }

    WindowBuilder::WindowBuilder()
    {
        _impl = new WindowBuilderImpl;
    }

    WindowBuilder::~WindowBuilder()
    {
        delete _impl;
    }

    WindowBuilder& WindowBuilder::title(const std::string& title)
    {
        _impl->title = title;
        return *this;
    }

    WindowBuilder& WindowBuilder::width(const u32 width)
    {
        _impl->width = width;
        return *this;
    }

    WindowBuilder& WindowBuilder::height(const u32 height)
    {
        _impl->height = height;
        return *this;
    }

    WindowBuilder& WindowBuilder::monitor(const u32 monitor)
    {
        _impl->monitor = monitor;
        return *this;
    }

    WindowBuilder& WindowBuilder::resizable(const bool resizable)
    {
        _impl->resizable = resizable;
        return *this;
    }

    IWindow* WindowBuilder::build() const
    {
        GlfwWindow* win = new GlfwWindow;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, _impl->resizable ? GLFW_TRUE : GLFW_FALSE);

        int32_t monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        GLFWmonitor* monitor = nullptr;

        if (_impl->monitor != ~0U && _impl->monitor < static_cast<u32>(monitorCount))
        {
            monitor = monitors[_impl->monitor];
        }

        win->windowTitle = _impl->title;
        GLFWwindow* window = glfwCreateWindow(static_cast<int32_t>(_impl->width), static_cast<int32_t>(_impl->height),
                                              win->windowTitle.c_str(), monitor, nullptr);
        win->window = window;

        glfwSetWindowUserPointer(window, win);

        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwShowWindow(window);

        return win;
    }

    GlfwWindow::~GlfwWindow()
    {
        glfwDestroyWindow(window);
    }

    std::string GlfwWindow::title() const
    {
        return windowTitle;
    }

    u32 GlfwWindow::width() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return static_cast<u32>(width);
    }

    u32 GlfwWindow::height() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return static_cast<u32>(height);
    }

    bool GlfwWindow::shouldClose() const
    {
        return glfwWindowShouldClose(window) == GLFW_TRUE;
    }

    void GlfwWindow::poll() const
    {
        glfwPollEvents();
    }

    void GlfwWindow::close()
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const Keyboard& GlfwWindow::getKeyboard() const noexcept
    {
        return keyboard;
    }

    const Mouse& GlfwWindow::getMouse() const noexcept
    {
        return mouse;
    }
} // namespace helios

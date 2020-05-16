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
        uint32_t width = ~0U;
        uint32_t height = ~0U;
        bool resizable = false;
        uint32_t monitor = ~0U;
    };

    struct GlfwWindow final : IWindow
    {
        GlfwWindow() = default;
        ~GlfwWindow() override;

        [[nodiscard]] std::string title() const override;
        [[nodiscard]] uint32_t width() const override;
        [[nodiscard]] uint32_t height() const override;
        [[nodiscard]] bool shouldClose() const override;
        void poll() const override;
        void close() override;

        GLFWwindow* window = nullptr;
        std::string windowTitle;

        GlfwWindow(const GlfwWindow&) = delete;
        GlfwWindow(GlfwWindow&&) noexcept = delete;
        GlfwWindow& operator=(const GlfwWindow&) = delete;
        GlfwWindow& operator=(GlfwWindow&&) noexcept = delete;
    };

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

    WindowBuilder& WindowBuilder::width(const uint32_t width)
    {
        _impl->width = width;
        return *this;
    }

    WindowBuilder& WindowBuilder::height(const uint32_t height)
    {
        _impl->height = height;
        return *this;
    }

    WindowBuilder& WindowBuilder::monitor(const uint32_t monitor)
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
        glfwWindowHint(GLFW_RESIZABLE,
                       _impl->resizable ? GLFW_TRUE : GLFW_FALSE);

        int32_t monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        GLFWmonitor* monitor = nullptr;

        if (_impl->monitor != ~0U &&
            _impl->monitor < static_cast<uint32_t>(monitorCount))
        {
            monitor = monitors[_impl->monitor];
        }

        win->windowTitle = _impl->title;
        GLFWwindow* window =
            glfwCreateWindow(static_cast<int32_t>(_impl->width),
                             static_cast<int32_t>(_impl->height),
                             win->windowTitle.c_str(), monitor, nullptr);
        win->window = window;

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

    uint32_t GlfwWindow::width() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return static_cast<uint32_t>(width);
    }

    uint32_t GlfwWindow::height() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return static_cast<uint32_t>(height);
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
} // namespace helios

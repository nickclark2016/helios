#pragma once

#include <helios/macros.hpp>

#include <string>

namespace helios
{
    class IWindow;

    class WindowBuilder
    {
    public:
        WindowBuilder();
        WindowBuilder(const WindowBuilder&) = delete;
        WindowBuilder(WindowBuilder&&) noexcept = delete;
        ~WindowBuilder();
        WindowBuilder& operator=(const WindowBuilder&) = delete;
        WindowBuilder& operator=(WindowBuilder&&) noexcept = delete;

        WindowBuilder& title(const std::string& title);
        WindowBuilder& width(const u32 width);
        WindowBuilder& height(const u32 height);
        WindowBuilder& monitor(const u32 monitor);
        WindowBuilder& resizable(const bool resizable);
        [[nodiscard]] IWindow* build() const;

    private:
        struct WindowBuilderImpl;

        WindowBuilderImpl* _impl;
    };

    class IWindow
    {
    protected:
        IWindow() = default;

    public:
        virtual ~IWindow() = default;
        IWindow(const IWindow&) = delete;
        IWindow(IWindow&&) noexcept = delete;
        IWindow& operator=(const IWindow&) = delete;
        IWindow& operator=(IWindow&&) noexcept = delete;

        [[nodiscard]] virtual std::string title() const = 0;
        [[nodiscard]] virtual u32 width() const = 0;
        [[nodiscard]] virtual u32 height() const = 0;
        [[nodiscard]] virtual bool shouldClose() const = 0;
        virtual void poll() const = 0;
        virtual void close() = 0;
    };
} // namespace helios

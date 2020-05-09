#pragma once

#include <cstdint>
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
		WindowBuilder& width(const uint32_t width);
		WindowBuilder& height(const uint32_t height);
		WindowBuilder& monitor(const uint32_t monitor);
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
		[[nodiscard]] virtual uint32_t width() const = 0;
		[[nodiscard]] virtual uint32_t height() const = 0;
		[[nodiscard]] virtual bool shouldClose() const = 0;
		virtual void poll() const = 0;
		virtual void close() = 0;
	};
}

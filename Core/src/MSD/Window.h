#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "GLFW/glfw3.h"
#include <string>
#include <chrono>

namespace MSD {

	struct WindowProps
	{
		std::string Title;
		unsigned int Height, Width;

		WindowProps(const std::string& title = "MaSDep - DEV Version",
					const unsigned int& width = 1000,
					const unsigned int& height = 700)
			: Title(title), Width(width), Height(height)
		{}
	};

	class MSD_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		inline GLFWwindow* GetID() const { return m_Window; }

		void SetEventCallBack(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		static Window* Create(const WindowProps& props = WindowProps());
	private:
		void Init(const WindowProps& props);
		void ShutDown();

		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Height, Width;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}


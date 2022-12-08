#pragma once

#include "GLFW/glfw3.h"
#include <string>

namespace MSD {

	struct WindowProps
	{
		std::string Title;
		unsigned int Height, Width;

		WindowProps(const std::string& title = "MSD",
					const unsigned int& height = 600,
					const unsigned int& width = 800)
			: Title(title), Height(height), Width(width)
		{}
	};

	class Window
	{
	private:
		GLFWwindow* m_Window;
	public:
		Window(const WindowProps& props);
		~Window();

		void Init(const WindowProps& props);
		static Window* Create(const WindowProps& props);
	};

}


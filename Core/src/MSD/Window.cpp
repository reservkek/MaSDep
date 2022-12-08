#include "Window.h"

namespace MSD {

	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new Window(props);
	}

	void Window::Init(const WindowProps& props)
	{
		if (!s_GLFWInitialized)
		{
			glfwInit();
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window,&m_Window);
	}

	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}
}


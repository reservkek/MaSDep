#include <iostream>
#include "ApplicationCore.h"

#include "glad/glad.h"

namespace MSD {

	ApplicationCore* ApplicationCore::s_Instance = nullptr;

	ApplicationCore::ApplicationCore()
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(ApplicationCore::OnEvent));
	}

	ApplicationCore::~ApplicationCore()
	{

	}

	void ApplicationCore::Run()
	{
		while (m_Running)
		{
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();

			}
			m_Window->OnUpdate();
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
		};
	}

	void ApplicationCore::OnEvent(Event& e)
	{
		std::cout << e.ToString() << "\n";
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(ApplicationCore::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void ApplicationCore::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void ApplicationCore::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool ApplicationCore::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
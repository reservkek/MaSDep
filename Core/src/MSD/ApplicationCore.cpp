#include <iostream>
#include "ApplicationCore.h"
#include "Events/ApplicationEvent.h"

namespace MSD {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	ApplicationCore::ApplicationCore()
	{
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
		};
	}

	void ApplicationCore::OnEvent(Event& e)
	{
		std::cout << e.GetName() << "\n";

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
	}

	void ApplicationCore::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

}
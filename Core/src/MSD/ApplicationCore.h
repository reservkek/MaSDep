#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerSystem/LayerStack.h"
#include "Events/Event.h"
#include "GLFW/glfw3.h"

namespace MSD {

	class MSD_API ApplicationCore
	{
	public:
		ApplicationCore();
		virtual ~ApplicationCore();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;

		LayerStack m_LayerStack;
	};

}

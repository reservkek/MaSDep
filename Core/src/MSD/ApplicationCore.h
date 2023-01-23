#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerSystem/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "MathModels/AngularMSD/AngularMSD.h"

#include "ImGui/ImGuiLayer.h"

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

		inline Window& GetWindow() { return *m_Window; }
		inline AngMSD& GetModel() { return *m_Model;  }

		void ModelUpdate();
		void WindowUpdate();

		inline static ApplicationCore& Get() { return *s_Instance; }
	private:
		
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		AngMSD* m_Model;

		bool m_Running = true;

		LayerStack m_LayerStack;
		static ApplicationCore* s_Instance;
	};

}

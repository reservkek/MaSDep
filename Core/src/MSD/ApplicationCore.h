#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerSystem/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "MathModels/AngularMSD/AngularMSD.h"
#include "Graphics/Renderer.h"
#include "Graphics/Internal/Timestep.h"

#include "UserInterface/MainLayer.h"
#include "UserInterface/GraphicsLayer.h"

#include "../glm/gtc/matrix_transform.hpp"

#define MSD_VERSION 0.5.2

namespace MSD {

	class MSD_API ApplicationCore
	{
	public:

	public:
		ApplicationCore();
		virtual ~ApplicationCore();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline AngMSD& GetModel() { return *m_Model; }

		GraphicsLayer* GetGraphicsLayer() { return m_GraphicsLayer; }

		void ModelUpdate();
		void WindowUpdate();

		inline static ApplicationCore& Get() { return *s_Instance; }

		bool IsViewportShown();
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		
		MainLayer* m_MainLayer;
		GraphicsLayer* m_GraphicsLayer;
		AngMSD* m_Model;

		bool m_Running = true;

		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
		float m_FPSlimit = 150.0f;
	private:
		static ApplicationCore* s_Instance;
	};
}

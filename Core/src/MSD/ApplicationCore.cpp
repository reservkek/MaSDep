#include "msdpch.h"

#include "ApplicationCore.h"

#include "glad/glad.h"

#include <thread>

namespace MSD {

	ApplicationCore* ApplicationCore::s_Instance = nullptr;

	ApplicationCore::ApplicationCore()
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BIND_EVENT_FN(ApplicationCore::OnEvent));

		m_Model = new AngMSD();
		m_GraphicsLayer = new GraphicsLayer();
		m_MainLayer = new MainLayer();

		PushOverlay(m_MainLayer);
		PushOverlay(m_GraphicsLayer);
	}

	void ApplicationCore::ModelUpdate()
	{
		while (m_Running)
		{
			if (m_Model->GetStatus()) m_Model->OnUpdate();
			else std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}

	void ModelUpdateWrapper()
	{
		ApplicationCore::Get().ModelUpdate();
	}

	static float lasttime = 0.0f;

	void ApplicationCore::WindowUpdate()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_GraphicsLayer->isUpdating() = m_MainLayer->GetViewportStatus();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate(timestep);
			}

			m_Window->OnUpdate();

			while (glfwGetTime() < lasttime + 1.0 / m_FPSlimit) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			lasttime += 1.0f / m_FPSlimit;
		};
	}

	ApplicationCore::~ApplicationCore()
	{

	}

	void ApplicationCore::Run()
	{
		std::thread* calculationThread = new std::thread(ModelUpdateWrapper);

		WindowUpdate();

		calculationThread->join();
		delete calculationThread;
	}

	void ApplicationCore::OnEvent(Event& e)
	{
		//std::cout << e.ToString() << "\n";

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

	bool ApplicationCore::IsViewportShown()
	{
		if (m_MainLayer == nullptr) return false;
		return m_MainLayer->GetViewportStatus();
	}

	bool ApplicationCore::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
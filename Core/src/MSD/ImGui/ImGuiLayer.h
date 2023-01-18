#pragma once

#include "../LayerSystem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

namespace MSD {

	class MSD_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;

		void Begin();
		void End();

		void MainMenuBar();
		void MainPanel();
		void ModelParametersWindow(bool* p_open);

	private:
		bool show = true;
		bool show_app_main_menu_bar = false;
		bool show_app_console = false;
		bool show_app_model_parameters = false;
		bool show_app_model_status = false;
		bool show_app_property_editor = false;

		double m_Time = 0.0;
	};

}


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

	private:
		double m_Time = 0.0;
	};

}


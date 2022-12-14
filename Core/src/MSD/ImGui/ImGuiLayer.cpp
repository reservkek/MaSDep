#include "ImGuiLayer.h"
#include "Core.h"

#include "GLFW/glfw3.h"
#include "ApplicationCore.h"

namespace MSD {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

        ImFontConfig font_config;
        font_config.OversampleH = 1; //or 2 is the same
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x044F, // Cyrillic
            0,
        };

        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f, &font_config, ranges);

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ApplicationCore& app = ApplicationCore::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetID());

		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui_ImplGlfw_InitForOpenGL(window, true);
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		
		ApplicationCore& app = ApplicationCore::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		double time = glfwGetTime();
		io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0 / 60.0);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
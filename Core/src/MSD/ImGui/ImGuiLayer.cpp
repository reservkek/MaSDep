#include "ImGuiLayer.h"

#include "GLFW/glfw3.h"

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
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();
		/*ImGui::StyleColorsDark();*/

		ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImFontConfig font_config;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x044F, // Cyrillic
            0,
        };

		io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f, &font_config, ranges);

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ApplicationCore& app = ApplicationCore::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetID());

		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui_ImplGlfw_InitForOpenGL(window, true);
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();

		ApplicationCore& app = ApplicationCore::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::MainMenuBar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Open");
				ImGui::MenuItem("Save", NULL, &show_app_console);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Model"))
			{
				ImGui::MenuItem("Model parameters", NULL, &show_app_model_parameters);
				ImGui::MenuItem("Status", NULL, &show_app_model_status);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("null", NULL, &show_app_property_editor);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar();
	}

	void ImGuiLayer::MainPanel()
	{
		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
		float height = ImGui::GetFrameHeight();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
		if (ImGui::BeginViewportSideBar("toolbar", viewport, ImGuiDir_Up, height+11.0f, window_flags))
		{
			if (ImGui::BeginMenuBar()) {
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 8.0f));
				if (ImGui::Button(!running ? "Run Program###Run" : "Stop Program###Run"))
				{
					ApplicationCore& app = ApplicationCore::Get();
					AngMSD& model = app.GetModel();

					if (running)
					{
						model.Stop();
						running = false;
					}
					else
					{
						model.Run();
						running = true;
					}
				}
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}
			ImGui::End();
			ImGui::PopStyleVar();
		};
	}

	void ImGuiLayer::MagnetronParameters(Magnetron* magnetron)
	{
		ImGui::InputFloat("Radius", magnetron->GetRadius());
		float* pos[3] = { magnetron->GetPosX(), magnetron->GetPosY(), magnetron->GetPosZ() };
		ImGui::InputFloat3("Magnetron position", *pos);
		float* normal[3] = { magnetron->GetNormalX(), magnetron->GetNormalY(), magnetron->GetNormalZ() };
		ImGui::InputFloat3("Magnetron normal vector", *normal);
		ImGui::InputFloat("###rotate", magnetron->GetRotationAngle());
		ImGui::SameLine();
		if (ImGui::Button("Rotate clockwise"))
		{
			magnetron->Rotate();
		}
	}

	void ImGuiLayer::SubstrateParameters(Substrate* substrate)
	{
		if (ImGui::CollapsingHeader("Substrate", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float* pos[3] = { substrate->GetPosX(), substrate->GetPosY(), substrate->GetPosZ() };
			ImGui::InputFloat3("Magnetron position", *pos);
			float* normal[3] = { substrate->GetNormalX(), substrate->GetNormalY(), substrate->GetNormalZ() };
			ImGui::InputFloat3("Magnetron normal vector", *normal);
			ImGui::InputFloat("###rotate", substrate->GetRotationAngle());
			ImGui::SameLine();
			if (ImGui::Button("Rotate clockwise"))
			{
				substrate->Rotate();
			}
			ImGui::PushItemWidth(100.0f);
			ImGui::InputFloat("RPM", substrate->GetRPM());
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(20.0f,ImGui::GetFrameHeight()));
			ImGui::SameLine();
			ImGui::InputFloat("Sub RPM", substrate->GetSubRPM());
			ImGui::PopItemWidth();
		}
	}

	void ImGuiLayer::ModelParametersWindow(bool* p_open)
	{

		if (!ImGui::Begin("Model Parameters", p_open))
		{
			ImGui::End();
			return;
		}

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		m_ProgressBar = model.GetCurrentProgress();

		ImGui::Text("Ticks: %d", model.timeTicksCounter);
		ImGui::Text("Magnetrons: %d", model.m_Magnetrons.size());
		ImGui::Separator();

		if (ImGui::Button("Add Magnetron"))
		{
			model.AddMagnetron();
		}

		unsigned int count = 0;
		for (auto i_magnetron : model.m_Magnetrons)
		{
			count++;
			if (!i_magnetron->GetIndex()) { i_magnetron->SetIndex(model.m_MagnetronIndex); }
			std::string countstr = "Magnetron " + std::to_string(i_magnetron->GetIndex());
			if (ImGui::CollapsingHeader((const char*)countstr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushID(count);
				MagnetronParameters(i_magnetron);
				if (ImGui::Button("Delete"))
				{
					model.DeleteMagnetron(count);
					if (count != 1) { model.m_MagnetronIndex = model.m_Magnetrons.back()->GetIndex(); }
					else { model.m_MagnetronIndex = 0; }
				}
				ImGui::PopID();
			}
		}
		ImGui::Separator();
		SubstrateParameters(model.m_Substrate);

		ImGui::End();
	}

	void ImGuiLayer::OnUpdate()
	{
		Begin();

		MainMenuBar();
		MainPanel();
		
		ImGui::DockSpaceOverViewport();

		if (show_app_model_parameters) ModelParametersWindow(&show_app_model_parameters);

		End();
	}
}
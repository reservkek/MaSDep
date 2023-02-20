#include "msdpch.h"

#include "MainLayer.h"
#include "Input/Controller.h"

#include "GLFW/glfw3.h"

namespace fs = std::filesystem;
namespace MSD {

	MainLayer::MainLayer()
		: Layer("MainLayer")
	{
	}

	MainLayer::~MainLayer()
	{
	}

	// IMGUI LAYER CREATION AND RENDERING FUNCTIONS
	void MainLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGui::StyleColorsMSD();
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
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoeui.ttf", 16.0f, &font_config, ranges);

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.ConfigWindowsMoveFromTitleBarOnly = true;

		io.DeltaTime = 100;

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

	void MainLayer::OnDetach()
	{
	}

	void MainLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void MainLayer::OnUpdate(Timestep ts)
	{
		m_Time = ts.GetSeconds();
		Begin();

		MainMenuBar();
		MainPanel();

		ImGui::DockSpaceOverViewport();

		if (show_file_path_err) FilePathErrPopup(&show_file_path_err, &errorMsg);
		if (show_app_model_parameters) ModelParametersWindow(&show_app_model_parameters);
		if (show_app_model_results) ModelResultsWindow(&show_app_model_results);
		if (show_app_model_viewport) ModelViewportWindow(&show_app_model_viewport);

		End();
	}

	void MainLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();

		ApplicationCore& app = ApplicationCore::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

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

	///////////////////
	// IMGUI WINDOWS //
	///////////////////

	// MAIN MENU
	void MainLayer::MainMenuBar()
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
				ImGui::MenuItem("Results", NULL, &show_app_model_results);
				ImGui::MenuItem("Viewport", NULL, &show_app_model_viewport);
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

	// MAIN PANEL
	void MainLayer::MainPanel()
	{
		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
		float height = ImGui::GetFrameHeight();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
		if (ImGui::BeginViewportSideBar("toolbar", viewport, ImGuiDir_Up, height + 11.0f, window_flags))
		{

			if (ImGui::BeginMenuBar())
			{

				ApplicationCore& app = ApplicationCore::Get();
				AngMSD& model = app.GetModel();
				bool& running = app.GetModel().GetStatus();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 8.0f));
				if (ImGui::Button(!running ? "Run Program###Run" : "Stop Program###Run"))
				{
					if (running)
					{
						model.Stop();
					}
					else
					{
						if (app.GetModel().m_Magnetrons.size())
						{ 
							if (!model.Run())
							{
								show_file_path_err = true;
							};
						}
						else
						{
							ImGui::OpenPopup("No magnetrons");
						}
					}
				}

				// Always center this window when appearing
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("No magnetrons", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("It appears you didn't add any magnetrons.\n\nFor a proper calculation, add a magnetron in Model Parameters Window.\n");
					ImGui::Separator();

					if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();
				ImGui::EndMenuBar();
			}
			ImGui::End();
			ImGui::PopStyleVar();
		};
	}


	// PARAMETERS 
	void MainLayer::ModelParametersWindow(bool* p_open)
	{

		if (!ImGui::Begin("Model Parameters", p_open))
		{
			ImGui::End();
			return;
		}

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		ImGuiIO& io = ImGui::GetIO();

		m_ProgressBar = model.GetCurrentProgress();
		ImGui::ProgressBar(m_ProgressBar);
		ImGui::Text("Ticks: %d", model.m_TimeTicksCounter);
		ImGui::Text("Magnetrons: %d", model.m_Magnetrons.size());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		if (ImGui::InputFloat("Rotation Limit", &model.m_RotationLimit)) {
			if (model.m_RotationLimit < 0) model.m_RotationLimit = 0;
		};
		if (ImGui::InputInt("Time Limit", &model.m_TimeLimit))
		{
			if (model.m_TimeLimit < 0) model.m_TimeLimit = 0;
		};
		if (ImGui::InputInt("Ticks Per Second", &model.m_TicksPerSecond))
		{
			if (model.m_TicksPerSecond < 1) model.m_TicksPerSecond = 1;
		};
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
				if (ImGui::Button("Delete Magnetron"))
				{
					model.DeleteMagnetron(count);
					if (model.m_Magnetrons.size() != 0) { model.m_MagnetronIndex = model.m_Magnetrons.back()->GetIndex(); }
					else { model.m_MagnetronIndex = 0; }
				}
				ImGui::PopID();
			}
		}
		ImGui::Separator();
		SubstrateParameters(model.m_Substrate);

		ImGui::End();
	}

	void MainLayer::MagnetronParameters(Magnetron* magnetron)
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
		ImGui::Separator();
		ImGui::Text("Sput rates input");
		ImGui::InputText("###SputRates",magnetron->m_InputFilePath,sizeof(magnetron->m_InputFilePath),ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (result == NFD_OPEN)
			ImGui::BeginDisabled();
		if (ImGui::Button("Browse"))
		{
			//if (!m_AllowInputWindow) return;
			//m_AllowInputWindow = false;
			result = NFD_OpenDialog(NULL, NULL, &outPath);
			*magnetron->GetInputFilePath() = outPath;
		}
		if (result == NFD_OPEN)
			ImGui::EndDisabled();
	}

	void MainLayer::SubstrateParameters(Substrate* substrate)
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
			ImGui::PushItemWidth(110.0f);
			ImGui::InputFloat("RPM", substrate->GetRPM());
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(20.0f, ImGui::GetFrameHeight()));
			ImGui::SameLine();
			ImGui::InputFloat("Sub RPM", substrate->GetSubRPM());
			ImGui::PopItemWidth();
		}
	}


	// RESULTS
	void MainLayer::ModelResultsWindow(bool* p_open)
	{
		if (!ImGui::Begin("Model results", p_open))
		{
			ImGui::End();
			return;
		}

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		std::vector<float>& data = model.m_SubstrateBuffer->GetDepEvolution();
		std::vector<float>* data_array[1] = { &data };

		if (ImGui::CollapsingHeader("Deposition Evolution"))
		{
			ApplicationCore& app = ApplicationCore::Get();
			AngMSD& model = app.GetModel();

			std::vector<float>& data = model.m_SubstrateBuffer->GetDepEvolution();

			DynamicPlot(FindPlotCond(), data);
			if (ImGui::Button("Export CSV file"))
			{
				outPath = (nfdchar_t*)(projectDirPath.c_str());
				result = NFD_SaveDialog("csv", NULL, &outPath);
				if (ExportCSV(outPath, data_array, 1, result))
				{
					ImGui::OpenPopup("Success");
				};
			}
			if (ImGui::BeginPopupModal("Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("CSV file successfully saved\n\n");
				ImGui::Separator();
				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}


	// MODEL VIEWPORT
	void MainLayer::ModelViewportWindow(bool* p_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (!ImGui::Begin("Model Viewport", p_open, ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::PopStyleVar(2);
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowFocused())
		{
			GraphicsLayer::m_HandleInputs = true;
		}
		else GraphicsLayer::m_HandleInputs = false;

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		ImTextureID texID = (ImTextureID)app.GetGraphicsLayer()->GetFrameBuffer().GetColorAttachment();

		//FOR RATIO 1:1
		ImVec2 wsize = ImGui::GetWindowSize();
		float length = std::max(wsize.x, wsize.y);

		ImGuiStyle& style = ImGui::GetStyle();

		float availX = ImGui::GetContentRegionAvail().x;
		float offsetX = (availX - (float)length) * 0.5f;

		float availY = ImGui::GetContentRegionAvail().y;
		float offsetY = (availY - (float)length) * 0.5f;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

		ImGui::Image(texID, ImVec2(length, length), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopupContextItem("Viewport Settings"))
		{
			if (ImGui::MenuItem("Reset camera position")) { Controller::ResetCameraPosition(); };
			if (ImGui::MenuItem("Add Magnetron")) { model.AddMagnetron(); };
			ImGui::EndPopup();
		}

		ImGui::End();
		return;
	}


	// PLOTS
	void MainLayer::DynamicPlot(ImPlotCond cond, std::vector<float>& data)
	{
		auto x_min = 0;
		auto x_max = (int)data.size();
		auto y_min = std::numeric_limits<float>::max();
		auto y_max = std::numeric_limits<float>::lowest();
		for (auto& val : data) {
			y_min = std::min(y_min, val);
			y_max = std::max(y_max, val);
		}
		ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, cond);
		if (ImPlot::BeginPlot("Deposition Evolution"))
		{
			if (data.size() != 0) ImPlot::PlotLine("", data.data(), (int)data.size());
			ImPlot::EndPlot();
		}
	}

	ImPlotCond MainLayer::FindPlotCond()
	{
		if (ApplicationCore::Get().GetModel().GetStatus()) return ImPlotCond_Always;
		return ImPlotCond_None;
	}


	// POPUPS
	void MainLayer::FilePathErrPopup(bool* p_open, std::string* error_msg)
	{
		if (!p_open) return;

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		ImGui::OpenPopup("File Error");

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("File Error", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(model.GetErrorMessage().c_str());

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				*p_open = false;
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			ImGui::EndPopup();
		}

	}

	bool MainLayer::ExportCSV(const char* outpath, std::vector<float>** data, int number_of_vectors, nfdresult_t result)
	{	
		if (result != NFD_OKAY) return false;
		
		// Open the file for writing
		std::filesystem::path filepath = outpath;
		if (filepath.extension() == "") filepath.replace_extension(".csv");

		std::ofstream file(filepath);

		if (!file.is_open()) {
			return false;
		}

		// Loop through each vector in the data array
		for (int i = 0; i < number_of_vectors; ++i) {
			// Get a reference to the current vector
			std::vector<float>& vec = *data[i];

			// Write the contents of the vector to the file
			auto size = vec.size();
			for (int j = 0; j < (int)size; ++j) {
				file << j << "," << vec[j] << std::endl;
			}
			file << std::endl;
		}

		// Close the file
		file.close();
		return true;
	}
}

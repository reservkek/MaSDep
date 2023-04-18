#include "msdpch.h"

#include "MainLayer.h"
#include "Graphics/Controller.h"

#include "GLFW/glfw3.h"

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
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoeui.ttf", 15.0f, &font_config, ranges);

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

		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();
		graphicsLayer->SetModel(&model);
		graphicsLayer->UpdateObjects();

		ImGui_ImplOpenGL3_Init("#version 430");
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

		ReadViewPortObjects();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		if (show_popup_file_path_err) FilePathErrPopup(&show_popup_file_path_err, &errorMsg);
		if (show_popup_success) SuccessPopup(&show_popup_success);
		if (show_app_model_parameters) ModelParametersWindow(&show_app_model_parameters);
		if (show_app_model_results) ModelResultsWindow(&show_app_model_results);
		if (show_app_model_viewport) ModelViewportWindow(&show_app_model_viewport);
		if (show_app_periodic_table) PeriodicTableWindow(&show_app_periodic_table, m_SelectedElement);
		ImGui::PopStyleVar();

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

	void MainLayer::ReadViewPortObjects()
	{
		if (!show_app_model_viewport) return;


		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		auto graphicsLayer = app.GetGraphicsLayer();
		auto framebuffer = app.GetGraphicsLayer()->GetFrameBuffer();
		auto fbSize = framebuffer->GetSpecification().Width;

		graphicsLayer->UpdateObjectStates();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		int mouseX = (int)(mx * m_ViewportWindowRelation);
		int mouseY = (int)(fbSize - (my * m_ViewportWindowRelation));

		if (mouseX > 0 && mouseY > 0 && mouseX < (int)fbSize && mouseY < (int)fbSize)
		{
			framebuffer->Bind();
			auto hoveredID = framebuffer->ReadPixel(1, mouseX, mouseY);

			framebuffer->Unbind();

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				toBeSelected = true;
			}

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				toBeSelected = false;
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && toBeSelected)
			{
				app.GetGraphicsLayer()->SetSelectedItem(hoveredID);
			}

			if (hoveredID == 99999)
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 100.0f);
				ImGui::TextUnformatted("Substrate");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			std::cout << "ID: " << hoveredID << "\n";
		}
	}

	// STATIC FUNCTIONS
	static bool ExportCSV(const char* outpath, std::vector<std::vector<float>*> data, std::vector<std::string> column_names)
	{
		std::filesystem::path filepath = outpath;
		if (filepath.extension() == "") filepath.replace_extension(".csv");

		std::ofstream file(filepath);

		if (!file.is_open()) {
			return false;
		}

		size_t number_of_vectors = data.size();
		size_t elements_per_vector = 0;
		for (auto& vec : data) {
			elements_per_vector = std::max(elements_per_vector, vec->size());
		}
		
		for (size_t i = 0; i < column_names.size(); ++i)
		{
			file << column_names[i];
			if (i < column_names.size() - 1) file << ",";
		}
		file << std::endl;

		// Write each row of the CSV file
		for (size_t row = 0; row < elements_per_vector; ++row) {
			for (size_t col = 0; col < number_of_vectors; ++col) {
				if (col == 0)
				{
					file << row+1 << ",";
				}
				if (row < data[col]->size()) {
					file <<  (*(data[col]))[row];
				}
				if (col != number_of_vectors - 1) {
					file << ",";
				}
			}
			file << std::endl;
		}

		// Close the file
		file.close();
		return true;
	}

	static void DrawVec3Control(const std::string& label, float* data, float columnwidth = 120.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2, (const char*)0, false);

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,5.0f));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize)) {};
		ImGui::SameLine();
		ImGui::InputFloat("##X", &data[0], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Y", buttonSize)) {};
		ImGui::SameLine();
		ImGui::InputFloat("##Y", &data[1], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Z", buttonSize)) {};
		ImGui::SameLine();
		ImGui::InputFloat("##Z", &data[2], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		ImGui::NextColumn();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));
		ImGui::Text(label.c_str());
		ImGui::PopStyleVar();

		ImGui::PopID();

		ImGui::Columns(1);
	}

	static void SetHandCursor()
	{
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(7);
	}

	static void HelpMarker(const char* desc)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
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
			SetHandCursor();
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
								show_popup_file_path_err = true;
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));
		if (!ImGui::Begin("Model Parameters", p_open))
		{
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}
		ImGui::PopStyleVar();

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();
		ImGuiIO& io = ImGui::GetIO();

		m_ProgressBar = model.GetCurrentProgress();
		ImGui::ProgressBar(m_ProgressBar);
		ImGui::Separator();

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
			graphicsLayer->UpdateObjects();
		}

		unsigned int count = 0;
		for (auto i_magnetron : model.m_Magnetrons)
		{
			count++;
			bool keepMagnetron = true; // Deletes magnetron if false
			if (!i_magnetron->GetIndex()) { i_magnetron->SetIndex(model.m_MagnetronIndex); }
			std::string countstr = "Magnetron " + std::to_string(i_magnetron->GetIndex());
			ImGui::PushID(count);
			if (ImGui::CollapsingHeader((const char*)countstr.c_str(), &keepMagnetron, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("DA", ImVec2(0, 200), true);
				MagnetronParameters(i_magnetron);
				ImGui::PopStyleVar();
				ImGui::EndChild();
			}
			if (keepMagnetron == false)
			{
				model.DeleteMagnetron(count);
				graphicsLayer->UpdateObjects();
				if (model.m_Magnetrons.size() != 0) { model.m_MagnetronIndex = model.m_Magnetrons.back()->GetIndex(); }
				else { model.m_MagnetronIndex = 0; }
			}
			ImGui::PopID();
		}
		ImGui::Separator();
		SubstrateParameters(model.m_Substrate);

		ImGui::End();
	}

	void MainLayer::MagnetronParameters(Magnetron* magnetron)
	{
		ImGui::InputFloat("Radius", magnetron->GetRadius());
		float* pos[3] = { magnetron->GetPosX(), magnetron->GetPosY(), magnetron->GetPosZ() };
		DrawVec3Control("Magnetron position", *pos);
		float* normal[3] = { magnetron->GetNormalX(), magnetron->GetNormalY(), magnetron->GetNormalZ() };
		DrawVec3Control("Magnetron normal vector", *normal);
		ImGui::InputFloat("###rotate", magnetron->GetRotationAngle());
		ImGui::SameLine();
		if (ImGui::Button("Rotate clockwise"))
		{
			magnetron->Rotate();
		}
		ImGui::Text("Target element: %s", GetSymbol(magnetron->GetElement()));
		ImGui::SameLine();
		if (ImGui::Button("..."))
		{
			show_app_periodic_table = true;
			m_SelectedElement = &magnetron->GetElement();
		}
		ImGui::Separator();
		ImGui::Text("Sput rates input");
		ImGui::InputText("###SputRates",magnetron->m_InputFilePath,sizeof(magnetron->m_InputFilePath),ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("Browse"))
		{
			//if (!m_AllowInputWindow) return;
			//m_AllowInputWindow = false;
			m_FileResult = NFD_OpenDialog(NULL, NULL, &m_OutPath);
			*magnetron->GetInputFilePath() = m_OutPath;
		}
	}

	void MainLayer::SubstrateParameters(Substrate* substrate)
	{
		if (ImGui::CollapsingHeader("Substrate", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float* pos[3] = { substrate->GetPosX(), substrate->GetPosY(), substrate->GetPosZ() };
			DrawVec3Control("Magnetron position", *pos);
			float* normal[3] = { substrate->GetNormalX(), substrate->GetNormalY(), substrate->GetNormalZ() };
			DrawVec3Control("Magnetron normal vector", *normal);
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


	// MODEL VIEWPORT
	void MainLayer::ModelViewportWindow(bool* p_open)
	{
		m_ViewportHeaderSize = ImGui::GetFrameHeightWithSpacing();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400 + m_ViewportHeaderSize));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		if (!ImGui::Begin("Model Viewport", p_open, ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::PopStyleVar(2);
			ImGui::End();
			return;
		}

		ImVec2 viewportOffset = ImGui::GetCursorPos();

		if (ImGui::IsWindowFocused())
		{
			GraphicsLayer::m_HandleInputs = true;
			Controller::EnableEvents();
		}
		else
		{
			GraphicsLayer::m_HandleInputs = false;
			Controller::DisableEvents();
		}

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();
		ImTextureID texID = (ImTextureID)app.GetGraphicsLayer()->GetFrameBuffer()->GetColorAttachment();
		auto fbSize = app.GetGraphicsLayer()->GetFrameBuffer()->GetSpecification().Width;

		//FOR RATIO 1:1
		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.y -= m_ViewportHeaderSize;
		float length = std::max(windowSize.x, windowSize.y);
		m_ViewportWindowRelation = fbSize / length;

		ImGuiStyle& style = ImGui::GetStyle();

		float availX = ImGui::GetContentRegionAvail().x;
		float offsetX = (availX - (float)length) * 0.5f;

		float availY = ImGui::GetContentRegionAvail().y;
		float offsetY = (availY - (float)length) * 0.5f;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

		ImVec2 minBound = ImGui::GetWindowPos();
		ImVec2 offsetToKeepRatio = { 0.5f * (length - windowSize.x), 0.5f * (length - windowSize.y) };
		minBound.x += viewportOffset.x - offsetToKeepRatio.x;
		minBound.y += viewportOffset.y - offsetToKeepRatio.y;

		ImVec2 maxBound = { minBound.x + windowSize.x + offsetToKeepRatio.x, minBound.y + windowSize.y + offsetToKeepRatio.y};
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };

		ImGui::Image(texID, ImVec2(length, length), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopupContextItem("Viewport Settings"))
		{
			if (ImGui::MenuItem("Reset camera position")) { Controller::ResetCameraPosition(); };
			if (ImGui::MenuItem("Add Magnetron"))
			{
				model.AddMagnetron();
				graphicsLayer->UpdateObjects();
			};
			ImGui::EndPopup();
		}

		ImGui::End();
		return;
	}

	// RESULTS
	void MainLayer::ModelResultsWindow(bool* p_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));

		if (!ImGui::Begin("Model results", p_open))
		{
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}
		ImGui::PopStyleVar();

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		std::vector<std::vector<float>*> depRatesData;

		if (ImGui::CollapsingHeader("Deposition Evolution"))
		{
			ApplicationCore& app = ApplicationCore::Get();
			AngMSD& model = app.GetModel();

			std::vector<float>& data = model.m_SubstrateBuffer->GetDepEvolution();

			DynamicPlot(FindPlotCond(), data, axesDepEvolution);
			ExportButton(model.m_ExportData, model.m_ExportDataColumnNames);
			ImGui::Separator();
		}


		unsigned int count = 0;
		for (auto magnetron : model.m_Magnetrons)
		{
			std::vector<float>& depRates = magnetron->GetDepRates();
			depRatesData.push_back(&depRates);

			ImGui::PushID(count);
			std::string countstr = "Deposition rate of magnetron " + std::to_string(magnetron->GetIndex());;
			if (ImGui::CollapsingHeader((const char*)countstr.c_str()))
			{
				DynamicPlot(FindPlotCond(), depRates, axesDepRates);
			}
			count++;
			ImGui::PopID();
		}
		ImGui::End();
		depRatesData.clear();
	}

	void MainLayer::ExportButton(std::vector<std::vector<float>*> data, std::vector<std::string> column_names)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		float alignment = 0.5f;
		float size = ImGui::CalcTextSize("Export CSV File").x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (avail - size) * alignment;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button("Export CSV file"))
		{
			m_OutPath = (nfdchar_t*)(projectDirPath.c_str());
			m_FileResult = NFD_SaveDialog("csv", NULL, &m_OutPath);
			if (m_FileResult == NFD_OKAY)
			{
				if (ExportCSV(m_OutPath, data, column_names))
				{
					show_popup_success = true;
				};
			}
		}
	}


	// PLOTS
	void MainLayer::DynamicPlot(ImPlotCond cond, std::vector<float>& data, const char* axes[2])
	{
		auto x_values = ApplicationCore::Get().GetModel().GetTimeValues();

		auto x_min = 0;
		auto x_max = ApplicationCore::Get().GetModel().GetCurrentTime()+1.0f;
		auto y_min = std::numeric_limits<float>::max();
		auto y_max = std::numeric_limits<float>::lowest();
		for (auto& val : data) {
			y_min = std::min(y_min, val);
			y_max = std::max(y_max, val);
		}
		
		ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, cond);

		if (ImPlot::BeginPlot("###plt"))
		{
			ImPlot::SetupAxes(*axes, *(axes+1));
			if (data.size() != 0) ImPlot::PlotLine("", x_values.data(), data.data(), (int)data.size());
			ImPlot::EndPlot();
		}
	}

	ImPlotCond MainLayer::FindPlotCond()
	{
		if (ApplicationCore::Get().GetModel().GetStatus()) return ImPlotCond_Always;
		return ImPlotCond_None;
	}


	// POPUPS
	void MainLayer::SuccessPopup(bool* p_open)
	{
		if (!p_open) return;

		ImGui::OpenPopup("Success");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Success", p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("CSV file successfully saved\n\n");
			ImGui::Separator();
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

	static const Element elements[] = { Al, Ti, Cr, Cu };

	static const int num_elements = sizeof(elements) / sizeof(Element);

	void MainLayer::PeriodicTableWindow(bool* p_open, Element* element)
	{
		if (!*p_open) return;

		ImGui::Begin("Choose element: ", p_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);

		// TODO Periodic table
		for (int i = 0; i < num_elements; i++) {

			ImGui::PushID(i);
			ImGui::Button(GetSymbol(elements[i]), ImVec2(40, 40));

			// Element name and atomic number as tooltip
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("%s", GetName(elements[i]));
				ImGui::Text("Atomic number: %d", GetAtomicNumber(elements[i]));
				ImGui::EndTooltip();
			}

			if (ImGui::IsItemClicked()) {
				*element = elements[i];
				*p_open = false;
			}

			ImGui::PopID();

			// Periodic table layout
			if ((i + 1) % 18 != 0) {
				ImGui::SameLine();
			}
			else {
				ImGui::Spacing();
			}
		}

		ImGui::End();
	}

}

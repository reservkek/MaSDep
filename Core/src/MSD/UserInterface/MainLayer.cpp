#include "msdpch.h"

#include "MainLayer.h"
#include "Controller.h"

#include "GLFW/glfw3.h"

namespace MSD {

	MainLayer::MainLayer()
		: Layer("MainLayer")
	{
	}

	MainLayer::~MainLayer()
	{
	}

	static std::map<std::string, ImFont*> Fonts;
	static int problemID = 0;

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
		ImFont* mainfont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoeui.ttf", 15.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());
		ImFont* bold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoeuib.ttf", 15.0f, &font_config, ranges);
		ImFont* bold17 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoeuib.ttf", 17.0f, &font_config, ranges);
		Fonts.insert({ "mainfont", mainfont });
		Fonts.insert({ "bold", bold });
		Fonts.insert({ "bold17", bold17 });

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

		ImGui::SetWindowFocus(MSDSTR_MODELPARAMETERS);
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
		if (show_popup_file_path_err) FilePathErrPopup(&show_popup_file_path_err);
		if (show_popup_success) SuccessPopup(&show_popup_success);
		if (show_app_model_objecttree) ModelObjectTree(&show_app_model_objecttree);
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

		m_ViewportMousePosX = (mx * m_ViewportWindowRelation);
		m_ViewportMousePosY = (fbSize - (my * m_ViewportWindowRelation));

		auto mouseX = (int)m_ViewportMousePosX;
		auto mouseY = (int)m_ViewportMousePosY;

		if (mouseX > 0 && mouseY > 0 && mouseX < (int)fbSize && mouseY < (int)fbSize && Controller::s_EnableEvents)
		{
			framebuffer->Bind();
			auto hoveredID = framebuffer->ReadPixel(1, mouseX, mouseY);

			framebuffer->Unbind();

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				toBeSelected = true;

				if (Controller::RotateObjectStop())
				{
					toBeSelected = false;
				}

				if (hoveredID == Renderer::GetSelectedItemID() && hoveredID != -1)
				{
					Controller::DragObjectStart();
				}
			}

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				toBeSelected = false;
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				Controller::DragObjectStop();
				if (toBeSelected) SetSelectedObject(hoveredID);
			}

			//std::cout << "ID: " << hoveredID << "\n";
			//std::cout << "Selected Object: " << Renderer::GetSelectedItemID() << "\n";
		}
	}

	void MainLayer::SetSelectedObject(unsigned int id)
	{
		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		m_SelectedObjectID = id;
		app.GetGraphicsLayer()->SetSelectedItem(id);
		if (id == -1 or id == 0) Controller::SetState(ControllerState::View);
		else
		{
			ImGui::SetWindowFocus(MSDSTR_OBJECTTREE);
			ImGui::SetWindowFocus(MSDSTR_MODELVIEWPORT);
			if (!model.m_ModelRunning) Controller::ObjectStartTransform(AngMSDObject::GetObject(id));
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

		auto red = ImVec4(1.0f, 0.2f, 0.25f, 1.0f);
		auto blue = ImVec4(0.3f, 0.5f, 1.0f, 1.0f);
		auto green = ImVec4(0.4f, 1.0f, 0.3f, 1.0f);
		auto textcolor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, red);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, red);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, red);
		ImGui::PushFont(Fonts["bold"]);
		ImGui::PushStyleColor(ImGuiCol_Text, textcolor);
		if (ImGui::Button("X", buttonSize)) {};
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::InputFloat("##X", &data[0], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, green);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, green);
		ImGui::PushFont(Fonts["bold"]);
		ImGui::PushStyleColor(ImGuiCol_Text, textcolor);
		if (ImGui::Button("Y", buttonSize)) {};
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::InputFloat("##Y", &data[1], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, blue);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
		ImGui::PushFont(Fonts["bold"]);
		ImGui::PushStyleColor(ImGuiCol_Text, textcolor);
		if (ImGui::Button("Z", buttonSize)) {};
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::InputFloat("##Z", &data[2], 0, 0, "%.1f");
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		ImGui::PopStyleColor(9);
		ImGui::NextColumn();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));
		ImGui::Text(label.c_str());
		ImGui::PopStyleVar();

		ImGui::PopID();

		ImGui::Columns(1);
	}

	static void DrawCoordBox()
	{
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

	static void TextCentered(std::string text) {
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::Text(text.c_str());
	}

	void MainLayer::ContextMenu_AddMagnetron()
	{
		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();

		auto size = ImGui::CalcTextSize(MSDSTR_ADDMAGNETRON);
		ImVec2 sizenew;
		sizenew.x = size.x * 1.2f;
		sizenew.y = size.y * 1.3f;
		ImGui::SetNextWindowSize(sizenew);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(size.x * 0.1f, size.y * 0.15f));
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem(MSDSTR_ADDMAGNETRON))
			{
				model.AddMagnetron();
				graphicsLayer->UpdateObjects();
				if (!show_app_model_objecttree) show_app_model_objecttree = true;
				SetSelectedObject(model.m_RecentMagnetronID);
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
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
			if (ImGui::BeginMenu(MSDSTR_MENU))
			{
				ImGui::MenuItem(MSDSTR_OPEN);
				ImGui::MenuItem(MSDSTR_SAVE, NULL, &show_app_console);
				ImGui::EndMenu();
			}
			SetHandCursor();
			if (ImGui::BeginMenu(MSDSTR_MODEL))
			{
				ImGui::MenuItem(MSDSTR_MODELPARAMETERS, NULL, &show_app_model_parameters);
				ImGui::MenuItem(MSDSTR_OBJECTTREE, NULL, &show_app_model_objecttree);
				ImGui::MenuItem(MSDSTR_RESULTS, NULL, &show_app_model_results);
				ImGui::MenuItem(MSDSTR_VIEWPORT, NULL, &show_app_model_viewport);
				ImGui::EndMenu();
			}
			SetHandCursor();
			if (ImGui::BeginMenu(MSDSTR_TOOLS))
			{
				ImGui::MenuItem(MSDSTR_HELP, NULL, &show_app_property_editor);
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

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 8.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
				ImGui::PushFont(Fonts["bold17"]);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.2f, 0.3f, 1.0f));
				if (ImGui::Button(!running ? MSDSTR_MODELRUN : MSDSTR_MODELSTOP))
				{
					ImGui::PopFont();
					ImGui::PopStyleColor();
					ImGui::SetWindowFocus(MSDSTR_MODELPARAMETERS);
					SetSelectedObject(-1);
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
								ImGui::SetWindowFocus(MSDSTR_OBJECTTREE);
								problemID = model.GetProblemObjectID();
								show_popup_file_path_err = true;
							};
						}
						else
						{
							ImGui::OpenPopup(MSDSTR_NOMAGNETRONS);
						}
					}
				}
				else {
					ImGui::PopFont();
					ImGui::PopStyleColor();
				}
				SetHandCursor();

				// Always center this window when appearing
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal(MSDSTR_NOMAGNETRONS, NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text(MSDSTR_NOMAGNETRONSERR);
					ImGui::Separator();

					if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { ImGui::CloseCurrentPopup(); }
					SetHandCursor();
					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					ImGui::EndPopup();
				}
				ImGui::PopStyleVar(2);
				ImGui::EndMenuBar();
			}
			ImGui::End();
			ImGui::PopStyleVar();
		};
	}

	static std::string axesDepEvolution[2] = { MSDSTR_TIME, MSDSTR_TOTALDEPOSITED_METERS };
	static std::string axesDepRates[2] = { MSDSTR_TIME, MSDSTR_DEPOSITIONRATE_METERS };

	// MODEL PARAMETERS 
	void MainLayer::ModelParametersWindow(bool* p_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));
		if (!ImGui::Begin(MSDSTR_MODELPARAMETERS, p_open))
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
		ImGui::Text(MSDSTR_TICKS, model.m_TimeTicksCounter);
		ImGui::Text(MSDSTR_SIMULATIONTIME, model.m_SimulationTime);


		ImGui::Separator();

		ImGui::Text(MSDSTR_TOTALMAGNETRONS, model.m_Magnetrons.size());
		ImGui::Text(MSDSTR_TOTALSUBSTRATES);
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		bool disable = model.m_ModelRunning;
		if (disable) ImGui::BeginDisabled();

		ImGui::Text(MSDSTR_FINISHWHEN);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);

		if (ImGui::InputFloat(MSDSTR_ROTATIONLIMIT, &model.m_RotationLimit)) {
			if (model.m_RotationLimit < 0) model.m_RotationLimit = 0;
		};

		if (ImGui::InputInt(MSDSTR_TIMELIMIT, &model.m_TimeLimit))
		{
			if (model.m_TimeLimit < 0) model.m_TimeLimit = 0;
		};
		if (ImGui::InputInt(MSDSTR_TICKSPERSECOND, &model.m_TicksPerSecond))
		{
			if (model.m_TicksPerSecond < 1) model.m_TicksPerSecond = 1;
		};


		ImGui::Separator();

		ImGui::Checkbox(MSDSTR_SHOWREALTIMEMOVEMENT, &model.m_ShowMovementRealTime);

		ImGui::Checkbox(MSDSTR_ENABLEFLUXSCATTERING, &model.m_EnableFluxScattering);

		ImGui::RadioButton(MSDSTR_ANGMSD_STANDARD, &model.m_ModelType, ANGMSD_STANDARD);
		ImGui::SameLine();
		ImGui::RadioButton(MSDSTR_ANGMSD_REACTIVE, &model.m_ModelType, ANGMSD_REACTIVE);

		if (model.m_EnableFluxScattering or model.m_ModelType == ANGMSD_REACTIVE)
		{
			if (ImGui::InputFloat(MSDSTR_PRESSURE, &model.m_Pressure)) {
				if (model.m_Pressure < 0) model.m_Pressure = 0;
			};
			if (ImGui::InputFloat(MSDSTR_TEMPERATURE, &model.m_Temperature)) {
				if (model.m_Temperature < 0) model.m_Temperature = 0;
			};
			ImGui::Text(MSDSTR_GAS, GetSymbol(model.m_Gas));
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				show_app_periodic_table = true;
				m_SelectedElement = &model.m_Gas;
			}
		};

		if (model.m_ModelType == ANGMSD_REACTIVE)
		{
			ImGui::Text(MSDSTR_REACTIVEGAS, GetSymbol(model.m_ReactiveGas));
			ImGui::SameLine();
			if (ImGui::Button("...##reactive"))
			{
				show_app_periodic_table = true;
				m_SelectedElement = &model.m_ReactiveGas;
			}
			ImGui::SliderFloat(MSDSTR_REACTIVEGASRATIO, &model.m_GasRatio, 0.0f, 1.0f, "%.2f");
		}

		ImGui::Text("Scattering Coeff: %.3f", model.m_ScatteringCoeff);
		ImGui::Separator();

		if (ImGui::CollapsingHeader(MSDSTR_RESULTSPARAMETERS))
		{
			ImGui::Combo(MSDSTR_DEPOSITEDUNITS, &model.m_TotalDepositedType, MSDSTR_DEPOSITED_COMBO);
			switch (model.m_TotalDepositedType)
			{
			case ANGMSD_DEPOSITED_PARTICLES:
				axesDepEvolution[1] = MSDSTR_TOTALDEPOSITED_PARTICLES;
				break;
			case ANGMSD_DEPOSITED_THICKNESS_M:
				axesDepEvolution[1] = MSDSTR_TOTALDEPOSITED_METERS;
				break;
			case ANGMSD_DEPOSITED_THICKNESS_MCM:
				axesDepEvolution[1] = MSDSTR_TOTALDEPOSITED_MICROMETERS;
				break;
			case ANGMSD_DEPOSITED_THICKNESS_NM:
				axesDepEvolution[1] = MSDSTR_TOTALDEPOSITED_NANOMETERS;
				break;
			default:
				break;
			}
		}

		if (ImGui::Button(MSDSTR_ADDMAGNETRON))
		{
			model.AddMagnetron();
			graphicsLayer->UpdateObjects();
			if (!show_app_model_objecttree) show_app_model_objecttree = true;
			SetSelectedObject(model.m_RecentMagnetronID);
		}

		ImGui::PopStyleVar();

		if (disable) ImGui::EndDisabled();

		ImGui::End();
	}

	void MainLayer::ModelObjectTree(bool* p_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));
		if (!ImGui::Begin(MSDSTR_OBJECTTREE, p_open))
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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.92f, 0.97f, 1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImGui::BeginChild("ChildObjectTree", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y*0.5f), false, window_flags);
		
		if (m_SelectedObjectID > 0)
		{
			auto& objects = AngMSDObject::s_Objects;
			auto& keys = AngMSDObject::s_KeyValues;

			auto pos = std::find(keys.begin(), keys.end(), m_SelectedObjectID);

			if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			{
				if ((pos + 1) != keys.end())
				{
					SetSelectedObject(*(pos + 1));
				}
			}

			if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			{
				if ((pos - 1) >= keys.begin())
				{
					SetSelectedObject(*(pos - 1));
				}
			}
		}

		//if (ImGui::IsItemClicked) SetSelectedObject(0);


		bool is_selected = false;

		unsigned int count = 1;
		auto& objects = AngMSDObject::s_Objects;
		auto& keys = AngMSDObject::s_KeyValues;

		std::vector<std::string> name_container;
		name_container.push_back(MSDSTR_MAGNETRON);

		for (int i = 0; i < keys.size(); ++i)
		{
			is_selected = objects[keys[i]]->GetID() == m_SelectedObjectID;
			std::string name = objects[keys[i]]->GetType();
			while (std::find(name_container.begin(), name_container.end(), name) != name_container.end())
			{
				name = std::string(objects[keys[i]]->GetType()) + " " + std::to_string(count);
				count++;
			}
			count = 1;
			name_container.push_back(name);

			ImGui::Selectable((const char*)name.c_str(), is_selected);
			if (ImGui::IsItemClicked()) SetSelectedObject(objects[keys[i]]->GetID());
		}

		ContextMenu_AddMagnetron();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::Separator();

		if (m_SelectedObjectID == -1 or m_SelectedObjectID == 0)
		{
			ImGui::End();
			return;
		}

		if (objects[m_SelectedObjectID] == nullptr)
		{
			ImGui::End();
			return;
		}

		if (objects[m_SelectedObjectID]->GetType() == MSDSTR_SUBSTRATE)
		{
			SubstrateParameters((Substrate*)objects[m_SelectedObjectID]);
		}

		if (objects[m_SelectedObjectID]->GetType() == MSDSTR_MAGNETRON)
		{
			MagnetronParameters((Magnetron*)objects[m_SelectedObjectID]);

			if (ImGui::Button(MSDSTR_DELETEMAGNETRON))
			{
				DeleteObject(m_SelectedObjectID);
			}

		}

		ImGui::End();
	}

	void MainLayer::DeleteObject(unsigned int id)
	{
		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();

		auto& objects = AngMSDObject::s_Objects;
		auto& keys = AngMSDObject::s_KeyValues;

		if (objects[id]->GetType() == MSDSTR_SUBSTRATE)
		{
			return;
		}

		auto iter = std::find(keys.begin(), keys.end(), id);
		if (keys.size() == 1) m_SelectedObjectID = 0;
		else if (iter == keys.begin()) m_SelectedObjectID = *(++iter);
		else m_SelectedObjectID = *(--iter);

		if (objects[id]->GetType() == MSDSTR_MAGNETRON)
		{
			model.DeleteMagnetron(id);
		}

		graphicsLayer->UpdateObjects();

		if (objects[m_SelectedObjectID] == nullptr) m_SelectedObjectID = 0;

		SetSelectedObject(m_SelectedObjectID);
	}

	static bool sigmund = 0;

	void MainLayer::MagnetronParameters(Magnetron* magnetron)
	{
		if (magnetron == nullptr) return;

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);

		ImGui::PushFont(Fonts["bold"]);
		ImGui::Button(MSDSTR_MAGNETRONPROPERTIES, ImVec2(ImGui::GetContentRegionAvail().x, 20));
		ImGui::PopFont();
		ImGui::InputFloat(MSDSTR_RADIUS, &magnetron->GetRadius());
		float* pos[3] = { magnetron->GetPosX(), magnetron->GetPosY(), magnetron->GetPosZ() };
		DrawVec3Control(MSDSTR_MAGNETRONPOS, *pos);
		float* normal[3] = { magnetron->GetNormalX(), magnetron->GetNormalY(), magnetron->GetNormalZ() };
		DrawVec3Control(MSDSTR_MAGNETRONNORMAL, *normal);
		ImGui::InputFloat("###rotate", &magnetron->GetRotationAngle());
		ImGui::SameLine();
		if (ImGui::Button(MSDSTR_ROTATECLOCKWISE))
		{
			magnetron->Rotate();
		}

		ImGui::Text(MSDSTR_TARGETELEMENT, GetSymbol(magnetron->GetElement()));
		ImGui::SameLine();
		if (ImGui::Button("..."))
		{
			show_app_periodic_table = true;
			m_SelectedElement = &magnetron->GetElement();
		}
		ImGui::Separator();
		ImGui::RadioButton(MSDSTR_CALCSPUTRATES, &magnetron->m_CalculationParameters, ANGMSD_CALC_RAW);
		ImGui::SameLine();
		ImGui::RadioButton(MSDSTR_USEFILE, &magnetron->m_CalculationParameters, ANGMSD_USE_FILE);
		if (magnetron->m_CalculationParameters == ANGMSD_CALC_RAW)
		{
			ImGui::Text(MSDSTR_ENTERMAGNETRONPARAMETERS);
			ImGui::InputInt(MSDSTR_VOLTAGE, &magnetron->m_Voltage);
			ImGui::InputFloat(MSDSTR_CURRENT, &magnetron->m_Current);
			ImGui::Text(MSDSTR_ENERGYTRANSMISSIONCOEFFS);
			ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth()/2);
			ImGui::InputFloat(MSDSTR_CURRENTCOEFF, &magnetron->m_CoeffCurr);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			ImGui::InputFloat(MSDSTR_VOLTAGECOEFF, &magnetron->m_CoeffVoltage);
			ImGui::PopItemWidth();
			if (magnetron->m_SputteringYieldType != ANGMSD_YIELD_CUSTOM) ImGui::BeginDisabled();
			ImGui::Text(MSDSTR_SPUTTERINGYIELD);
			ImGui::SameLine();
			ImGui::PushItemWidth(60.0f);
			ImGui::InputFloat("###SputYield", &magnetron->m_SputteringYield);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (magnetron->m_SputteringYieldType != ANGMSD_YIELD_CUSTOM) ImGui::EndDisabled();
			ImGui::Checkbox(MSDSTR_SIGMUNDCALCULATION, &sigmund);
			if (sigmund) magnetron->m_SputteringYieldType = ANGMSD_YIELD_SIGMUND;
			else magnetron->m_SputteringYieldType = ANGMSD_YIELD_CUSTOM;
			if (ImGui::CollapsingHeader(MSDSTR_MAGNETICFIELDDISTRIBUTION))
			{
				if (ImPlot::BeginPlot("###mfd", ImVec2(300,200), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs))
				{
					auto& data = magnetron->m_MagneticFieldDistributionInput;
					ImPlot::SetupAxes(MSDSTR_DISTANCEFROMCENTER,MSDSTR_RADIALMAGNETICFIELD);
					std::vector<float> keys = { std::views::keys(data).begin(), std::views::keys(data).end() };
					std::vector<float> values = { std::views::values(data).begin(), std::views::values(data).end() };
					if (data.size() != 0) ImPlot::PlotLine("", keys.data(), values.data(), (int)data.size());
					ImPlot::EndPlot();
				}
			}
		}
		else if (magnetron->m_CalculationParameters == ANGMSD_USE_FILE)
		{
			ImGui::Text(MSDSTR_SPUTRATESINPUT);
			ImGui::InputText("###SputRates", *magnetron->GetInputFilePath(), sizeof(*magnetron->GetInputFilePath()), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button(MSDSTR_BROWSE))
			{
				//if (!m_AllowInputWindow) return;
				//m_AllowInputWindow = false;
				m_FileResult = NFD_OpenDialog(NULL, NULL, &m_OutPath);
				*magnetron->GetInputFilePath() = m_OutPath;
			}
		}

		ImGui::PopStyleVar();

	}

	static float angle = 0;

	void MainLayer::SubstrateParameters(Substrate* substrate)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
		ImGui::PushFont(Fonts["bold"]);
		ImGui::Button(MSDSTR_SUBSTRATEPROPERTIES, ImVec2(ImGui::GetContentRegionAvail().x, 20));
		ImGui::PopFont();
		float* pos[3] = { substrate->GetPosX(), substrate->GetPosY(), substrate->GetPosZ() };
		DrawVec3Control(MSDSTR_SUBSTRATEPOS, *pos);
		float* normal[3] = { substrate->GetNormalX(), substrate->GetNormalY(), substrate->GetNormalZ() };
		DrawVec3Control(MSDSTR_SUBSTRATENORMAL, *normal);
		ImGui::InputFloat("###rotate", &substrate->GetRotationAngle());
		ImGui::SameLine();
		if (ImGui::Button(MSDSTR_ROTATECLOCKWISE))
		{
			substrate->Rotate();
		}
		ImGui::InputFloat("###rotate2", &angle);
		ImGui::SameLine();
		if (ImGui::Button(MSDSTR_ROTATEAROUNDCENTER))
		{
			substrate->RotateAroundCenter(angle*PI/180);
			angle = 0;
		}
		ImGui::PushItemWidth(110.0f);
		ImGui::InputFloat(MSDSTR_RPM, &substrate->GetRPM());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(MSDSTR_RPMHELP);
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(20.0f, ImGui::GetFrameHeight()));
		ImGui::SameLine();
		ImGui::InputFloat(MSDSTR_SUBRPM, &substrate->GetSubRPM());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(MSDSTR_SUBRPMHELP);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}


	// MODEL VIEWPORT
	void MainLayer::ModelViewportWindow(bool* p_open)
	{
		m_ViewportHeaderSize = ImGui::GetFrameHeightWithSpacing();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400 + m_ViewportHeaderSize));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();
		GraphicsLayer* graphicsLayer = app.GetGraphicsLayer();
		auto& objects = AngMSDObject::s_Objects;

		if (!ImGui::Begin(MSDSTR_MODELVIEWPORT, p_open, ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::PopStyleVar(2);
			ImGui::End();
			return;
		}

		Controller::s_EnableEvents = ImGui::IsWindowHovered();

		ContextMenu_AddMagnetron();

		ImVec2 viewportOffset = ImGui::GetCursorPos();

		if (ImGui::IsWindowFocused())
		{
			GraphicsLayer::m_HandleInputs = true;
			Controller::EnableCameraEvents();
		}
		else
		{
			GraphicsLayer::m_HandleInputs = false;
			Controller::DisableCameraEvents();
		}

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

		float zoom = Controller::GetCameraZoomLevel();
		auto camPos = Controller::GetCameraPosition();
		auto projectionMatrix = Controller::GetProjectionMatrix();
		auto viewMatrix = Controller::GetViewMatrix();

		double mouseX = (m_ViewportMousePosX - fbSize * 0.5);
		double mouseY = (-m_ViewportMousePosY + fbSize * 0.5);

		mouseX *= zoom * 0.5;
		mouseY *= zoom * 0.5;

		float angle = (180 - Controller::GetCameraRotation()) * PI / 180;

		auto x = mouseX;
		auto y = mouseY;

		mouseX = + x * cos(angle) + y * sin(angle);
		mouseY = y * cos(angle) - x * sin(angle);

		mouseX -= camPos.x;
		mouseY += camPos.y;

		mouseX *= 0.1;
		mouseY *= 0.1;

		// MOUSE COORDS CORNER BOX //
		ImGui::SetCursorPosX(windowSize.x - 140.0f);
		ImGui::SetCursorPosY(windowSize.y - 5.0f);

		std::string str = "";
		if (Controller::GetState() == ControllerState::View)
		{
			str = std::format(MSDSTR_CM3F, mouseX) + " ; " + std::format(MSDSTR_CM3F, mouseY);
		}
		else if (Controller::GetState() == ControllerState::Tranform && objects[m_SelectedObjectID] != nullptr)
		{
			auto x = *objects[m_SelectedObjectID]->GetPosX();
			auto y = *objects[m_SelectedObjectID]->GetPosY();
			str = std::format(MSDSTR_CM3F, x) + " ; " + std::format(MSDSTR_CM3F, y);
		}
		str += std::string("###CoordBox");
		const char* name = str.c_str();
		auto boxColor = ImVec4(0.65f, 0.8f, 1.0f, 0.5f);
		ImGui::PushStyleColor(ImGuiCol_Button, boxColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, boxColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, boxColor);
		if (ImGui::Button(name, ImVec2(130.0f, 0.0f))) {}
		ImGui::PopStyleColor(3);
		//////////////////////////////


		// TOP INSTUMENT PANEL //
		//ImGui::SetCursorPosX((windowSize.x-300.0f)/2.0f);
		//ImGui::SetCursorPosY(25.0f);
		//ImGui::BeginChild("###instrumentpanel");
		//ImGui::PushStyleColor(ImGuiCol_Button, boxColor);
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, boxColor);
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, boxColor);
		//ImGui::PopStyleColor(3);
		//ImGui::Button("test");
		//ImGui::SameLine();
		//ImGui::Button("test");
		//ImGui::SameLine();
		//ImGui::Button("test");
		//ImGui::SameLine();
		//ImGui::Button("test");
		//ImGui::EndChild();
		/////////////////////////
		ImGui::End();
		return;
	}

	// RESULTS
	void MainLayer::ModelResultsWindow(bool* p_open)
	{
		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 400));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);

		if (!ImGui::Begin(MSDSTR_MODELRESULTS, p_open))
		{
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}

		std::vector<std::vector<float>*> depRatesData;

		ImGui::PushFont(Fonts["bold"]);
		if (ImGui::CollapsingHeader(MSDSTR_SUBSTRATEDEPOSITIONEVOLUTION))
		{
			ImGui::PushFont(Fonts["mainfont"]);
			std::vector<float>& data = model.m_SubstrateBuffer->GetDepEvolution();
			const char* axes[2] = { axesDepEvolution[0].c_str(), axesDepEvolution[1].c_str() };
			DynamicPlot(FindPlotCond(), data, axes);
			ExportButton(model.m_ExportData, model.m_ExportDataColumnNames);
			ImGui::PopFont();
			TextCentered(MSDSTR_OTHERRESULTS);
			ImGui::Text(MSDSTR_MEANINCIDENTANGLE, model.m_SubstrateBuffer->MeanIncidentAngle);

			ImGui::Text(MSDSTR_COATINGCOMPOSITION);
			ImGui::PushFont(Fonts["mainfont"]);
			auto& composition = model.m_SubstrateBuffer->Composition();
			if (ImGui::BeginTable("Coating composition", composition.size()+1, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn(MSDSTR_ELEMENT);
				for (auto& pair : composition)
				{
					ImGui::TableSetupColumn(GetSymbol(pair.first));
				}
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				int column = 0;
				ImGui::TableSetColumnIndex(column);
				ImGui::Text(MSDSTR_PERCENTAGE);
				column++;
				for (auto& pair : composition)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::Text("%f", pair.second);
					column++;
				}
				ImGui::EndTable();
			}
			ImGui::PopFont();
			ImGui::Separator();
		}
		ImGui::PopFont();

		unsigned int count = 0;
		for (auto mpair : model.m_Magnetrons)
		{
			auto& magnetron = mpair.second;
			std::vector<float>& depRates = magnetron->GetDepRates();
			depRatesData.push_back(&depRates);

			ImGui::PushID(count);
			std::string countstr = MSDSTR_DEPOSITIONFROMMAGNETRON + std::to_string(magnetron->GetID());

			ImGui::PushFont(Fonts["bold"]);
			if (ImGui::CollapsingHeader((const char*)countstr.c_str()))
			{
				ImGui::PushFont(Fonts["mainfont"]);
				const char* axes[2] = { axesDepRates[0].c_str(), axesDepRates[1].c_str() };

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25.0f);
				if (ImGui::CollapsingHeader(MSDSTR_DEPRATETIMEDEPENDENCE))
				{
					DynamicPlot(FindPlotCond(), depRates, axes);
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25.0f);
				if (ImGui::CollapsingHeader(MSDSTR_ATOMENERGYDISTRIBUTION))
				{
					if (ImPlot::BeginPlot("###plt33"))
					{
						auto& data = magnetron->GetEnergyDistribution()->GetData();
						std::vector<float> x_vals = { std::views::keys(data).begin(),  std::views::keys(data).end() };
						std::vector<float> y_vals = { std::views::values(data).begin(),  std::views::values(data).end() };
						ImPlot::SetupAxes(MSDSTR_ENERGYEV, MSDSTR_PROBABILITYDENSITY);
						{
							if (data.size() != 0) ImPlot::PlotLine("", x_vals.data(), y_vals.data(), (int)data.size());
						}
						ImPlot::EndPlot();
					}
				}
				ImGui::PopFont();

				ImGui::PushFont(Fonts["bold"]);
				TextCentered(MSDSTR_OTHERRESULTS);
				ImGui::Text(MSDSTR_SPUTTERINGRATE, magnetron->GetOverallSputRate());
				ImGui::Text(MSDSTR_MEANDEPOSITIONRATE, magnetron->GetMeanDepRate());
				ImGui::Text(MSDSTR_MEANATOMENERGY, magnetron->GetEnergyDistribution()->GetMeanEnergy());
				ImGui::PopFont();
				ImGui::Text("");
				ImGui::Separator();
			}
			ImGui::PopFont();
			count++;
			ImGui::PopID();
		}

		ImGui::PopStyleVar(2);

		ImGui::End();
		depRatesData.clear();
	}

	void MainLayer::ExportButton(std::vector<std::vector<float>*> data, std::vector<std::string> column_names)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		//float alignment = 0.5f;
		//float size = ImGui::CalcTextSize("Export CSV File").x + style.FramePadding.x * 2.0f;
		//float avail = ImGui::GetContentRegionAvail().x;

		//float off = (avail - size) * alignment;
		//if (off > 0.0f)
		//	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button(MSDSTR_EXPORTCSV, ImVec2(ImGui::GetContentRegionAvail().x, 20)))
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

		//auto x_min = 0;
		//auto x_max = ApplicationCore::Get().GetModel().GetCurrentTime()+1.0f;
		//auto y_min = std::numeric_limits<float>::max();
		//auto y_max = std::numeric_limits<float>::lowest();
		//for (auto& val : data) {
		//	y_min = std::min(y_min, val);
		//	y_max = std::max(y_max, val);
		//}
		//
		// ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max, cond);
		if (cond) ImPlot::SetNextAxesToFit();

		if (ImPlot::BeginPlot("###plt"))
		{
			ImPlot::SetupAxes(*axes, *(axes+1));
			if (data.size() != 0) ImPlot::PlotLine("", x_values.data(), data.data(), (int)data.size());
			ImPlot::EndPlot();
		}

	}

	static float timecount = 0;
	static bool stopped;

	ImPlotCond MainLayer::FindPlotCond()
	{
		if (ApplicationCore::Get().GetModel().GetStatus())
		{
			if (ApplicationCore::Get().GetModel().m_SimulationTime > timecount)
			{
				stopped = false;
				timecount += 0.2f;
				return ImPlotCond_Always;
			}
		}
		else if (stopped == false)
		{
			stopped = true;
			timecount = 0;
			return ImPlotCond_Always;
		}
		return ImPlotCond_None;
	}


	// POPUPS
	void MainLayer::SuccessPopup(bool* p_open)
	{
		if (!p_open) return;

		ImGui::OpenPopup(MSDSTR_SUCCESS);

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(MSDSTR_SUCCESS, p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(MSDSTR_CSVSAVED);
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
			{
				ImGui::CloseCurrentPopup();
				*p_open = false;
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			ImGui::EndPopup();
		}
	}

	void MainLayer::FilePathErrPopup(bool* p_open)
	{
		if (!p_open) return;

		ApplicationCore& app = ApplicationCore::Get();
		AngMSD& model = app.GetModel();

		ImGui::OpenPopup(MSDSTR_FILEERROR);

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(MSDSTR_FILEERROR, p_open, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(model.GetErrorMessage().c_str());

			if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
			{
				ImGui::CloseCurrentPopup();
				SetSelectedObject(problemID);
				*p_open = false;
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			ImGui::EndPopup();
		}
		else SetSelectedObject(problemID);
	}

	static const Element elements[] = { Al, Ti, Cr, Cu, Y, Ce, Ar, N };

	static const int num_elements = sizeof(elements) / sizeof(Element);

	void MainLayer::PeriodicTableWindow(bool* p_open, Element* element)
	{
		if (!*p_open) return;

		auto mousepos = ImGui::GetMousePos();
		ImGui::SetNextWindowPos({ mousepos.x, mousepos.y }, ImGuiCond_Once);

		ImGui::Begin(MSDSTR_CHOOSEELEMENT, p_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
		


		// TODO Periodic table
		for (int i = 0; i < num_elements; i++) {

			ImGui::PushID(i);
			ImGui::Button(GetSymbol(elements[i]), ImVec2(24, 24));

			// Element name and atomic number as tooltip
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("%s", GetName(elements[i]));
				ImGui::Text(MSDSTR_ATOMICNUMBER, GetAtomicNumber(elements[i]));
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

	void MainLayer::PeriodicTableWindowNew(bool* p_open, Element* element)
	{
		if (!*p_open) return;

		auto mousepos = ImGui::GetMousePos();
		ImGui::SetNextWindowPos({ mousepos.x, mousepos.y }, ImGuiCond_Once);

		ImGui::Begin(MSDSTR_CHOOSEELEMENT, p_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);



		// TODO Periodic table
		for (int i = H; i <= Rn; i++) {

			ImGui::PushID(i);

			// Element name and atomic number as tooltip
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("%s", GetName(elements[i]));
				ImGui::Text(MSDSTR_ATOMICNUMBER, GetAtomicNumber(elements[i]));
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

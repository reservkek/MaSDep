#pragma once

#include "Core.h"

#include "../LayerSystem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"

#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_glfw.h"

#include "ApplicationCore.h"
#include "GraphicsLayer.h"

#include "GLFW/glfw3.h"

#include "../NFD/include/nfd.h"

namespace MSD {

	class MSD_API MainLayer : public Layer
	{
	public:

		MainLayer();
		~MainLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;

		void Begin();
		void End();

		// MAIN MENU BAR
		void MainMenuBar();
		void MainPanel();

		// WINDOWS
		void ModelParametersWindow(bool* p_open);
		void ModelResultsWindow(bool* p_open);
		void ModelViewportWindow(bool* p_open);

		// SUBSECTIONS
		void MagnetronParameters(Magnetron* magnetron);
		void SubstrateParameters(Substrate* substrate);

		// WIDGETS
		void DynamicPlot(ImPlotCond cond, std::vector<float>& data, const char* axes[2]);
		void ExportButton(std::vector<std::vector<float>*> data, const char* id = "");

		// POPUPS
		void SuccessPopup(bool* p_open);
		void FilePathErrPopup(bool* p_open, std::string* error_msg);

		// FUNCTIONS
		bool GetViewportStatus() { return show_app_model_viewport; }

		ImPlotCond FindPlotCond();
	private:
		int val = 0;

		bool show_app_main_menu_bar = false;
		bool show_app_console = false;
		bool show_app_model_parameters = false;
		bool show_app_model_results = false;
		bool show_app_property_editor = false;
		bool show_app_model_viewport = false;
		bool show_popup_file_path_err = false;
		bool show_popup_success = false;

		float m_ProgressBar = 0;

		float m_Time = 0.0f;

		bool m_ViewPortHandleInputs = false;

		std::string errorMsg = "";
		std::string projectDirPath = "C:/users/eeo5/Documents/";

		const char* axesDepEvolution[2] = { "Time (s)", "Number of deposited atoms (1/m2)" };
		const char* axesDepRates[2] = { "Time (s)", "Deposition rate (m/s)" } ;

		nfdchar_t* outPath = (nfdchar_t*)"";
		bool m_AllowInputWindow = true;
		nfdresult_t result = NFD_ERROR;
	};

}


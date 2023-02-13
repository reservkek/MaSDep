#pragma once

#include "Core.h"

#include "../LayerSystem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"

#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_glfw.h"

#include "ApplicationCore.h"

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

		void MainMenuBar();
		void MainPanel();

		void MagnetronParameters(Magnetron* magnetron);
		void SubstrateParameters(Substrate* substrate);
		void ModelParametersWindow(bool* p_open);
		void ModelResultsWindow(bool* p_open);
		void ModelViewportWindow(bool* p_open);
		void FilePathErrPopup(bool* p_open, std::string* error_msg);
		void DynamicPlot(ImPlotCond cond, std::vector<float>& data);

		bool ExportCSV(const char* path, std::vector<float>** data, int number_of_vectors, nfdresult_t result);

		ImPlotCond FindPlotCond();
	private:
		int val = 0;

		bool show_app_main_menu_bar = false;
		bool show_app_console = false;
		bool show_app_model_parameters = false;
		bool show_app_model_results = false;
		bool show_app_property_editor = false;
		bool show_app_model_viewport = true;
		bool show_file_path_err = false;

		float m_ProgressBar = 0;

		float m_Time = 0.0f;

		std::string errorMsg = "";
		std::string projectDirPath = "C:/users/eeo5/Documents/";
		nfdchar_t* outPath = (nfdchar_t*)"";
		nfdresult_t result = NFD_ERROR;
	};

}


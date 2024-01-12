#pragma once

#include "Core.h"

#include "MSDObjects.h"

#include "imgui.h"

import ReactiveMSD;
import SputteringRates;
import EnergyDistribution;
import FluxScattering;

namespace MSD {

	class MSD_API AngMSD {
	public:
		AngMSD();
		~AngMSD()
		{
			delete s_Instance;
		};
		
		inline int GetTimeTicksCounter() const { return m_TimeTicksCounter; }
		inline float GetCurrentProgress() const { return m_CurrentProgress; }
		inline unsigned int GetRecentMagnetronID() const { return m_RecentMagnetronID; }
		bool& GetStatus() { return m_ModelRunning; }

		inline static AngMSD& GetModelID() { return *s_Instance; }

		void CalculateFlux(Magnetron* magnetron, Substrate* substrate, bool write = true);
		void CalculateMeanFluxAngle(int count);
		bool Run();
		void Stop();
		void Clear();
		void Complete();
		void OnUpdate();

		void AddMagnetron();
		void DeleteMagnetron(unsigned int index);


		std::string GetErrorMessage() { return m_ErrorMsg; }
		float& GetCurrentTime() { return m_CurrentTime; }
		std::vector<float>& GetTimeValues() { return m_TimeValues; }

		extern friend class MainLayer;
		extern friend class GraphicsLayer;
	private:

		// Время в модели
		int m_TimeTicksCounter = 0; // Счётчик времени в тиках
		int m_TicksPerSecond = 5; // Количество тиков в секунду
		float m_TimePerTick = 1.0f / m_TicksPerSecond; // Количество времени за 1 тик.

		int m_TimeLimit = 60; // Ограничения моделирования по времени в секундах
		float m_CurrentTime = 0;

		// Пространство в модели
		unsigned int m_SpaceRatio = 100; // Разделение 1 метра пространства на виртуальные отрезки

		// Другие параметры
		float m_RotationLimit = 1;
		float m_RotationCounter = 0;

		float m_IntegrationDelta = 0.1f;

		vec3 m_CurrentFluxVector;
		float m_CurrentGamma = 0; //  Incident angle to substrate
		float m_CurrentPhi = 0; // Angle between flux and target
		float m_MeanFluxAngle = 0; // Mean incident angle

		// Прогресс-шкала
		float m_CurrentProgress = 0;
		float m_CurrentProgressDelta = 0;

		// Объекты (Начальные значения)
		Substrate* m_Substrate = new Substrate();
		std::unordered_map<unsigned int, Magnetron*> m_Magnetrons;
		unsigned int m_MagnetronCount = 0;
		unsigned int m_RecentMagnetronID = 0;

		// Буфер для расчёта
		Substrate* m_SubstrateBuffer = new Substrate();
		std::unordered_map<unsigned int, Magnetron*> m_MagnetronsBuffer;

		// Контейнер для значений по времени
		std::vector<float> m_TimeValues = {};

		// Данные для вывода в текстовый файл.
		std::vector<std::vector<float>*> m_ExportData = {};
		std::vector<std::string> m_ExportDataColumnNames = {};

		// FLUX SCATTERING
		bool m_EnableFluxScattering = 0;
		float m_Pressure = 0.18f;
		float m_Temperature = 300.0f;
		float m_ScatteringCoeff = 1.0f;
		Element m_Gas = Ar;

		// Simulation time calculations
		float m_SimulationTime = 0.0f;
		std::chrono::time_point<std::chrono::system_clock> m_TimePointStart;

		static AngMSD* s_Instance;
		bool m_ModelRunning = false;
		bool m_ToBeCleared = false;
		bool m_MeanFluxAngleCalculation = false;

		std::string m_ErrorMsg = "";
	};
}
#pragma once

#include "Core.h"

#include "MSDObjects.h"

#include "imgui.h"


namespace MSD {

	class MSD_API AngMSD {
	public:
		AngMSD();
		~AngMSD()
		{
			delete m_Substrate;
			for (auto i_magnetron : m_Magnetrons) delete i_magnetron;
		};
		
		inline int GetTimeTicksCounter() const { return m_TimeTicksCounter; }
		inline float GetCurrentProgress() const { return m_CurrentProgress; }
		bool& GetStatus() { return m_ModelRunning; }

		inline static AngMSD& GetModelID() { return *s_Instance; }

		void CalculateFlux(Magnetron* magnetron, Substrate* substrate, bool write = true);
		bool Run();
		void Stop();
		void Clear();
		void Complete();
		void OnUpdate();

		void AddMagnetron();
		void DeleteMagnetron(unsigned int& index);

		std::string GetErrorMessage() { return m_ErrorMsg; }
		float& GetCurrentTime() { return m_CurrentTime; }
		std::vector<float>& GetTimeValues() { return m_TimeValues; }

		extern friend class MainLayer;
	private:

		// Время в модели
		int m_TimeTicksCounter = 0; // Счётчик времени в тиках
		int m_TicksPerSecond = 5; // Количество тиков в секунду
		float m_TimePerTick = 1.0f / m_TicksPerSecond; // Количество времени за 1 тик.

		int m_TimeLimit = 60; // Лимит моделирования во времени в секундах
		float m_CurrentTime = 0;

		// Пространство в модели
		unsigned int spaceRatio = 100; // Разделение 1 метра пространства на виртуальные отрезки

		// Другие параметры
		float m_RotationLimit = 1;
		float m_RotationCounter = 0;

		float m_IntegrationDelta = 0.1f;

		vec3 m_CurrentFluxVector;
		float m_CurrentGamma = 0;
		float m_CurrentPhi = 0;

		// Прогресс-бар
		float m_CurrentProgress = 0;
		float m_CurrentProgressDelta = 0;

		// Объекты (Начальные значения)
		Substrate* m_Substrate = new Substrate();
		std::vector<Magnetron*> m_Magnetrons;
		unsigned int m_MagnetronIndex = 0;

		// Буфер для расчёта
		Substrate* m_SubstrateBuffer = new Substrate();
		std::vector<Magnetron*> m_MagnetronsBuffer;

		std::vector<float> m_TimeValues = {};

		static AngMSD* s_Instance;
		bool m_ModelRunning = false;
		bool m_ToBeCleared = false;

		std::string m_ErrorMsg = "";
	};
}
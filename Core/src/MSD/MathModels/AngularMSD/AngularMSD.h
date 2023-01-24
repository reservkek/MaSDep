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
		
		inline int GetTimeTicksCounter() const { return timeTicksCounter; }
		inline float GetCurrentProgress() const { return m_CurrentProgress; }
		inline bool GetStatus() { return m_ModelRunning; }

		inline static AngMSD& GetModelID() { return *s_Instance; }

		void CalculateFlux(Magnetron* magnetron, Substrate* substrate);
		void Run();
		void Stop();
		void OnUpdate();

		void AddMagnetron();
		void DeleteMagnetron(unsigned int& index);

		extern friend class ImGuiLayer;

		// TO DO:
		// Implement File Explorer Dialogue window
		const char* a = "input.txt";

	private:

		// Время в модели
		int timeTicksCounter = 0; // Счётчик времени в тиках
		unsigned int const timeRatio = 5; // Количество тиков в секунду
		double const timePerTick = 1.0 / timeRatio; // Количество времени за 1 тик.
		unsigned int timeLimit = 60; // Лимит моделирования во времени в секундах
		float currentTime = 0;

		// Пространство в модели
		unsigned int spaceRatio = 100; // Разделение 1 метра пространства на виртуальные отрезки

		// Другие параметры
		float rotationLimit = 0;
		float rotationCounter = 0;

		float m_IntegrationDelta = 0.1;

		vec3 m_CurrentFluxVector;
		float m_CurrentGamma = 0;
		float m_CurrentPhi = 0;

		// Прогресс-бар
		float m_CurrentProgress = 0;
		float m_CurrentProgressDelta = 0;

		// Объекты (Начальные значение)
		Substrate* m_Substrate = new Substrate();
		std::vector<Magnetron*> m_Magnetrons;
		unsigned int m_MagnetronIndex = 0;

		// Буфер для расчёта
		Substrate* m_SubstrateBuffer = nullptr;
		std::vector<Magnetron*> m_MagnetronsBuffer;

		static AngMSD* s_Instance;
		bool m_ModelRunning = false;
	};

}
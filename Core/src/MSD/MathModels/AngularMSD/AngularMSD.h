#pragma once

#include "Core.h"

#include "Magnetron.h"

#include "imgui.h"

#define PI 3.14159265358979

namespace MSD {

	class MSD_API AngMSD {
	public:
		AngMSD();
		
		inline int GetTimeTicksCounter() const { return timetickscounter; }
		inline bool GetStatus() { return m_ModelRunning; }

		inline static AngMSD& GetModelID() { return *s_Instance; }

		void Run();
		void Stop();
		void OnUpdate();

		void AddMagnetron();
		void DeleteMagnetron(unsigned int& index);

		extern friend class ImGuiLayer;

	private:
		class Substrate
		{
		public:
			Substrate(const vec3& pos = { 0.0, 15.0, 0.0 }, const vec3& normal = { 0.0, 15.0, 0.0 },
				const double& velocity = 0, const double& subvelocity = 0)
				: subpos(pos), subnormal(normal), angularvelocity(velocity), subangularvelocity(subvelocity)
			{}

			vec3 getPos() const { return subpos; };
			vec3 getNormal() const { return subnormal; };

		private:
			vec3 subpos, subnormal;
			const double angularvelocity, subangularvelocity;
		};

		// Время в модели
		int timetickscounter = 0; // Счётчик времени в тиках
		unsigned int const timeratio = 5; // Количество тиков в секунду
		double const timepertick = 1.0 / timeratio; // Количество времени за 1 тик.
		unsigned int timelimit = 60; // Лимит моделирования во времени в секундах

		// Пространство в модели
		unsigned int spaceratio = 100; // Разделение 1 метра пространство на виртуальные отрезки

		std::vector<Magnetron*>::iterator begin() { return m_Magnetrons.begin(); }
		std::vector<Magnetron*>::iterator end() { return m_Magnetrons.end(); }

		std::vector<Magnetron*> m_Magnetrons;
		unsigned int m_MagnetronIndex = 0;
		Substrate m_Substrate;

		static AngMSD* s_Instance;
		bool m_ModelRunning = false;
	};

}
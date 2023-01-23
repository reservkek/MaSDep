#include "AngularMSD.h"

namespace MSD {

	AngMSD* AngMSD::s_Instance = nullptr;

	AngMSD::AngMSD()
	{
		s_Instance = this;
	}

	void AngMSD::Run()
	{
		m_ModelRunning = true;
	}

	void AngMSD::Stop()
	{
		m_ModelRunning = false;
	}

	void AngMSD::OnUpdate()
	{
		if (m_ModelRunning == false) return;

		for (int i = 0; i < 10000; i++)
		{
			float b = i / 20.0;
			float c = i / 30.0;
			std::cout << b << ", " << c << std::endl;
		}
	}

	void AngMSD::AddMagnetron()
	{
		m_Magnetrons.push_back(new Magnetron());
		m_MagnetronIndex++;
	}

	void AngMSD::DeleteMagnetron(unsigned int& index)
	{
		m_Magnetrons.erase(m_Magnetrons.begin()+index-1);
	}

}
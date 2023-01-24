#include "AngularMSD.h"

namespace MSD {

	AngMSD* AngMSD::s_Instance = nullptr;

	AngMSD::AngMSD()
	{
		s_Instance = this;
	}

	void AngMSD::Run()
	{
		m_MagnetronsBuffer = m_Magnetrons;
		m_SubstrateBuffer = m_Substrate;

		float& vel = *(m_SubstrateBuffer->GetRPM());

		m_ModelRunning = true;
	}

	void AngMSD::CalculateFlux(Magnetron* magnetron, Substrate* substrate)
	{
		auto radius = *(magnetron->GetRadius());
		float localRadius = 0;
		float localSputRate = 0;
		vec3 localPos;

		for (auto i = -radius; i < radius; i += m_IntegrationDelta)
		{
			for (auto j = -radius; j < radius; j += m_IntegrationDelta)
			{
				if ((localRadius = sqrt(i * i + j * j)) > radius) continue;

				localPos = (magnetron->GetPos() + magnetron->integrationvectorI * i + magnetron->integrationvectorJ * j);
				m_CurrentFluxVector = FindVector(substrate->GetPos(), localPos);
				m_CurrentGamma = Angle(m_CurrentFluxVector, substrate->GetNormal());
				m_CurrentPhi = Angle(-m_CurrentFluxVector, magnetron->GetNormal());

				if (m_CurrentGamma >= PI / 2 or m_CurrentPhi >= PI / 2) continue;

				localSputRate = magnetron->FindSputRate(localRadius);

				//vrelsputlocal = Approx(localradius, input);
				//vdepi = vrelsputlocal * dl * dl * cos(phi) * cos(gamma) / (PI * pow(Magnitude(flux), 2));
				//vdep += vdepi;
				//vrelsput += vrelsputlocal;
			}
		}
	}

	void AngMSD::Stop()
	{
		m_ModelRunning = false;

		m_CurrentProgress = 0.0f;
	}

	void AngMSD::OnUpdate()
	{
		if (m_ModelRunning == false) return;

		if (rotationCounter >= rotationLimit or currentTime >= timeLimit)
		{
			m_ModelRunning = false;
			m_CurrentProgress = 1.0f;
			return;
		}

		Substrate& s = *m_SubstrateBuffer;

		for (auto m : m_MagnetronsBuffer)
		{
			CalculateFlux(m, &s);
		}

		s.GetTotalAngle() += s.GetTotalAngleDelta();
		s.GetTotalSubAngle() += s.GetTotalSubAngleDelta();

		timeTicksCounter++;
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
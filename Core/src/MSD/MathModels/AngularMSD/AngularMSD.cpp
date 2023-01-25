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
		*m_SubstrateBuffer = *m_Substrate;

		float vel = *(m_SubstrateBuffer->GetRPM());
		m_SubstrateBuffer->GetTotalAngleDelta() = m_TimePerTick * vel * PI / 30;

		vel = *(m_SubstrateBuffer->GetSubRPM());
		m_SubstrateBuffer->GetTotalSubAngleDelta() = m_TimePerTick * vel * PI / 30;

		float expectedRotationTicks = (m_RotationLimit * 2 * PI) / m_SubstrateBuffer->GetTotalAngleDelta();
		float expectedTimeLimitTicks = m_TimeLimit * m_TicksPerSecond;

		for (auto m : m_MagnetronsBuffer)
		{
			m->InputSputRates(*(m->GetInputFilePath()), m_IntegrationDelta);
		}

		m_CurrentProgressDelta = 1.0f / std::min(expectedRotationTicks, expectedTimeLimitTicks);

		m_ModelRunning = true;
		m_ToBeCleared = true;
	}

	void AngMSD::CalculateFlux(Magnetron* magnetron, Substrate* substrate)
	{
		auto radius = *(magnetron->GetRadius());
		float localRadius = 0;
		float localSputRate = 0;
		float localDepRate = 0;
		float fullDepRate = 0;
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
				localDepRate = localSputRate * m_IntegrationDelta * m_IntegrationDelta * cos(m_CurrentPhi) * cos(m_CurrentGamma) / (PI * pow(Magnitude(m_CurrentFluxVector), 2));
				fullDepRate += localDepRate;

			}
		}

		substrate->GetTotalDeposited() += fullDepRate * m_TimePerTick * 8.33e28;
		substrate->WriteDepEvolution();

		m_CurrentFluxVector = FindVector(substrate->GetPos(), magnetron->GetPos());
		m_CurrentGamma = Angle(m_CurrentFluxVector, substrate->GetNormal());
		m_CurrentPhi = Angle(-m_CurrentFluxVector, magnetron->GetNormal());

		magnetron->GetCurrentDepRate() = fullDepRate;
		magnetron->WriteDepRate();
		magnetron->WriteGamma(m_CurrentGamma);
		magnetron->WritePhi(m_CurrentPhi);

	}

	void AngMSD::Stop()
	{
		m_ModelRunning = false;
		m_ToBeCleared = true;
	}

	void AngMSD::Clear()
	{
		if (m_ToBeCleared == false) return;
		m_ToBeCleared = false;

		m_TimeTicksCounter = 0;
		m_CurrentProgress = 0;
		m_CurrentTime = 0;
	}

	void AngMSD::Complete()
	{
		m_ToBeCleared = false;
		m_ModelRunning = false;
		m_CurrentProgress = 1.0f;
	}

	void AngMSD::OnUpdate()
	{
		Clear();

		if (m_ModelRunning == false)
		{
			return;
		}

		if (m_RotationCounter >= m_RotationLimit or m_CurrentTime >= m_TimeLimit)
		{
			Complete();
			return;
		}

		Substrate& s = *m_SubstrateBuffer;

		for (auto m : m_MagnetronsBuffer)
		{
			CalculateFlux(m, &s);
		}

		s.Update();

		m_RotationCounter += s.GetTotalAngleDelta()/(2*PI);
		m_CurrentProgress += m_CurrentProgressDelta;
		m_CurrentTime += m_TimePerTick;
		m_TimeTicksCounter++;
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
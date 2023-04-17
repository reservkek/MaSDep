#include "msdpch.h"

#include "AngularMSD.h"
#include "../Database/PhysicsData.h"

namespace MSD {
	using namespace Database;

	AngMSD* AngMSD::s_Instance = nullptr;

	AngMSD::AngMSD()
	{
		s_Instance = this;
	}

	bool AngMSD::Run()
	{
		m_MagnetronsBuffer = m_Magnetrons;
		m_SubstrateBuffer = new Substrate();
		*m_SubstrateBuffer = *m_Substrate;

		m_TimeValues = {};

		float vel = *(m_SubstrateBuffer->GetRPM());
		m_TimePerTick = 1.0f / m_TicksPerSecond;

		m_SubstrateBuffer->GetTotalAngleDelta() = m_TimePerTick * vel * PI / 30;

		vel = *(m_SubstrateBuffer->GetSubRPM());
		m_SubstrateBuffer->GetTotalSubAngleDelta() = m_TimePerTick * vel * PI / 30;

		float expectedRotationTicks = (m_RotationLimit * 2 * PI) / m_SubstrateBuffer->GetTotalAngleDelta();
		int expectedTimeLimitTicks = m_TimeLimit * m_TicksPerSecond;

		for (auto m : m_MagnetronsBuffer)
		{
			m->Clear();

			m->InputSputRates(*(m->GetInputFilePath()), m_IntegrationDelta);
			if (m->GetFilePathErr())
			{
				m_ErrorMsg = m->GetErrorMessage();
				m_ModelRunning = false;
				return false;
			}
		}

		m_CurrentProgressDelta = 1.0f / std::min(expectedRotationTicks, (float)expectedTimeLimitTicks);

		m_ToBeCleared = true;
		m_ModelRunning = true;
		return true;
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
		m_RotationCounter = 0;
		m_CurrentProgress = 0;
		m_CurrentTime = 0;

		m_ExportData.clear();
		m_ExportDataColumnNames.clear();
	}

	void AngMSD::Complete()
	{
		m_ToBeCleared = false;
		m_ModelRunning = false;
		m_CurrentProgress = 1.0f;

		m_ExportDataColumnNames.push_back("Tick");
		m_ExportDataColumnNames.push_back("Time (s)");
		m_ExportDataColumnNames.push_back("Substrate deposition (1/m2)");

		m_ExportData.push_back(&m_TimeValues);
		m_ExportData.push_back(&m_SubstrateBuffer->GetDepEvolution());

		for (auto magnetron : m_MagnetronsBuffer)
		{
			m_ExportData.push_back(&magnetron->GetDepRates());
			std::string name = std::string("Dep rate from magnetron No.") + std::to_string(magnetron->GetIndex()) + std::string(" (m/s)");

			m_ExportDataColumnNames.push_back(name);
		}
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

		static bool write = false;
		unsigned short count = 0;
		for (auto m : m_MagnetronsBuffer)
		{
			count++;
			if (count == m_MagnetronsBuffer.size()) write = true;
			else write = false;
			CalculateFlux(m, &s, write);
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
		m_Magnetrons.back()->SetIndex(m_MagnetronIndex);
	}

	void AngMSD::DeleteMagnetron(unsigned int& index)
	{
		m_Magnetrons.erase(m_Magnetrons.begin()+index-1);
	}

	void AngMSD::CalculateFlux(Magnetron* magnetron, Substrate* substrate, bool write)
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
				localDepRate = localSputRate * m_IntegrationDelta * m_IntegrationDelta * cos(m_CurrentPhi) * cos(m_CurrentGamma) / (PI * (float)pow(Magnitude(m_CurrentFluxVector), 2));
				fullDepRate += localDepRate;
			}
		}

		const static float CrDensity = GetAtomicDensity(magnetron->GetElement());
		substrate->GetTotalDeposited() += fullDepRate * m_TimePerTick * CrDensity;

		if (write) substrate->WriteDepEvolution();

		m_CurrentFluxVector = FindVector(substrate->GetPos(), magnetron->GetPos());
		m_CurrentGamma = Angle(m_CurrentFluxVector, substrate->GetNormal());
		m_CurrentPhi = Angle(-m_CurrentFluxVector, magnetron->GetNormal());

		if (write) m_TimeValues.push_back(m_CurrentTime);

		magnetron->GetCurrentDepRate() = fullDepRate;
		magnetron->WriteDepRate();
		magnetron->WriteGamma(m_CurrentGamma);
		magnetron->WritePhi(m_CurrentPhi);

	}
}
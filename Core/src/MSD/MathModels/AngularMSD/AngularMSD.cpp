#include "msdpch.h"

#include "AngularMSD.h"
#include "../Database/PhysicsData.h"
#include "UserInterface/Controller.h"

namespace MSD {
	using namespace Database;

	AngMSD* AngMSD::s_Instance = nullptr;

	AngMSD::AngMSD()
	{
		s_Instance = this;
		m_Substrate->SetID(1248956);
	}

	bool AngMSD::Run()
	{
		m_Substrate->Clear();

		m_MagnetronsBuffer = m_Magnetrons;
		m_SubstrateBuffer = m_Substrate->clone();

		m_TimeValues.clear();

		float vel = m_SubstrateBuffer->GetRPM();
		m_TimePerTick = 1.0f / m_TicksPerSecond;

		m_SubstrateBuffer->GetTotalAngleDelta() = m_TimePerTick * vel * PI / 30;

		vel = m_SubstrateBuffer->GetSubRPM();
		m_SubstrateBuffer->GetTotalSubAngleDelta() = m_TimePerTick * vel * PI / 30;

		float expectedRotationTicks = (m_RotationLimit * 2 * PI) / m_SubstrateBuffer->GetTotalAngleDelta();
		int expectedTimeLimitTicks = m_TimeLimit * m_TicksPerSecond;

		for (auto mpair : m_MagnetronsBuffer)
		{
			auto& m = mpair.second;
			m->Clear();

			if (m->GetSputRatesCalculationType() == ANGMSD_USE_FILE) {
				m->InputSputRates(*(m->GetInputFilePath()), m_IntegrationDelta);
				if (m->GetFilePathErr())
				{
					m_ErrorMsg = m->GetErrorMessage();
					m_ModelRunning = false;
					m_problemObjectID = m->GetID();
					return false;
				}
			}
			else if (m->GetSputRatesCalculationType() == ANGMSD_CALC_RAW)
			{
				m->RawCalcSputRates(m_IntegrationDelta, m_Gas);
			}
		}

		m_CurrentProgressDelta = 1.0f / std::min(expectedRotationTicks, (float)expectedTimeLimitTicks);

		m_ToBeCleared = true;
		m_ModelRunning = true;
		
		m_TimePointStart = std::chrono::system_clock::now();

		if (m_EnableEnergyDistribution)
		{
			for (auto mpair : m_MagnetronsBuffer)
			{
				mpair.second->CalcEnergyDistribution(m_Gas);
			}
		}

		if (!m_EnableFluxScattering) return true;

		for (auto mpair : m_MagnetronsBuffer)
		{
			auto& m = mpair.second;
			auto m1 = Database::GetAtomicMass(m->GetElement());
			auto m2 = Database::GetAtomicMass(m_Gas);
			auto r1 = Database::GetAtomicRadius(m->GetElement());
			auto r2 = Database::GetAtomicRadius(m_Gas);
			auto& temp = m_Temperature;
			auto en_bind = Database::GetBindingEnergy(m->GetElement());
			m->GetPD() = FluxScattering::CalcPD(m1,m2,r1,r2,temp,en_bind);
		}

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

		m_SimulationTime = 0.0f;
		m_TimeTicksCounter = 0;
		m_RotationCounter = 0;
		m_CurrentProgress = 0;
		m_CurrentTime = 0;

		m_ExportData.clear();
		m_ExportDataColumnNames.clear();
		
		m_Substrate->Clear();
		m_SubstrateBuffer->Clear();
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

		for (auto mpair : m_MagnetronsBuffer)
		{
			auto& magnetron = mpair.second;
			m_ExportData.push_back(&magnetron->GetDepRates());
			std::string name = std::string("Dep rate from magnetron No.") + std::to_string(magnetron->GetID()) + std::string(" (m/s)");

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
		for (auto mpair : m_MagnetronsBuffer)
		{
			auto& m = mpair.second;
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

		auto timePointNow = std::chrono::system_clock::now();
		m_SimulationTime = std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_TimePointStart).count()*0.001f;
	}

	void AngMSD::AddMagnetron()
	{
		m_MagnetronCount += 1;
		if (m_Magnetrons.count(m_MagnetronCount))
		{
			AddMagnetron();
			return;
		}
		m_Magnetrons.emplace(m_MagnetronCount, new Magnetron());
		m_Magnetrons[m_MagnetronCount]->SetID(m_MagnetronCount);
		m_RecentMagnetronID = m_MagnetronCount;
	}

	void AngMSD::DeleteMagnetron(unsigned int id)
	{
		if (m_Magnetrons.count(id))
		{
			AngMSDObject::GetObject(id)->Delete();
			m_Magnetrons.erase(id);
			m_MagnetronCount -= 1;
			std::cout << "Magnetron with ID: " << id << " was successfully erased \n";
		}
		else std::cout << "There is no such magnetron present in the model.\n";
	}

	void AngMSD::CalculateFlux(Magnetron* magnetron, Substrate* substrate, bool write)
	{
		auto radius = magnetron->GetRadius();
		float localRadius = 0;
		float localSputRate = 0;
		float localDepRate = 0;
		float fullDepRate = 0;
		vec3 localPos;
		static std::mutex mutex;

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

				substrate->MeanIncidentAngle_raw += m_CurrentGamma * localDepRate;
			}
		}

		substrate->TotalDepositedRaw() += fullDepRate;
		auto& el = magnetron->GetElement();
		auto& compositionraw = substrate->CompositionRaw();
		if (compositionraw.count(el)) compositionraw[el] += fullDepRate;
		else compositionraw.insert({ el, fullDepRate });

		if (m_EnableFluxScattering)
		{
			vec3 MagnetronSubstrateVec = FindVector(substrate->GetPos(), magnetron->GetPos());
			float distance = Magnitude(MagnetronSubstrateVec);
			//float geometryfactor = FluxScattering::GeometryFactor(*magnetron->GetRadius(), distance);
			m_ScatteringCoeff = FluxScattering::FluxPressureInteractionCoeff(distance, m_Pressure, magnetron->GetPD(), 1);
			fullDepRate *= m_ScatteringCoeff;
		}

		switch (m_TotalDepositedType)
		{
		case ANGMSD_DEPOSITED_PARTICLES:
			static float atomicDensity = GetAtomicDensity(magnetron->GetElement());
			substrate->TotalDeposited() += fullDepRate * m_TimePerTick;
			break;
		case ANGMSD_DEPOSITED_THICKNESS_M:
			substrate->TotalDeposited() += fullDepRate * m_TimePerTick / atomicDensity;
			break;
		case ANGMSD_DEPOSITED_THICKNESS_MCM:
			substrate->TotalDeposited() += fullDepRate * m_TimePerTick * 1e6f / atomicDensity;
			break;
		case ANGMSD_DEPOSITED_THICKNESS_NM:
			substrate->TotalDeposited() += fullDepRate * m_TimePerTick * 1e9f / atomicDensity;
			break;
		default:
			substrate->TotalDeposited() += fullDepRate * m_TimePerTick;
		}

		m_CurrentFluxVector = FindVector(substrate->GetPos(), magnetron->GetPos());
		m_CurrentGamma = Angle(m_CurrentFluxVector, substrate->GetNormal());
		m_CurrentPhi = Angle(-m_CurrentFluxVector, magnetron->GetNormal());

		std::lock_guard<std::mutex> lock(mutex);

		if (write)
		{
			substrate->WriteDepEvolution();
			m_TimeValues.push_back(m_CurrentTime);
		}

		magnetron->GetCurrentDepRate() = fullDepRate;
		magnetron->WriteDepRate();
		magnetron->WriteGamma(m_CurrentGamma);
		magnetron->WritePhi(m_CurrentPhi);

	}

}
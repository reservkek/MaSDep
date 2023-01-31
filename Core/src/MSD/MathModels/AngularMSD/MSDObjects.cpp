#include "MSDObjects.h"

namespace MSD {

	Magnetron::Magnetron(const vec3& pos, const vec3& normal, const double& radius)
		: msdpos(pos), msdnormal(normal), m_Radius(radius)
	{
		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);
	}

	Magnetron::~Magnetron()
	{
	}

	void Magnetron::Rotate()
	{
		m_RotationAngle = m_RotationAngle * PI / 180;
		msdpos = RotateAroundZ(msdpos, -m_RotationAngle);
		msdnormal = RotateAroundZ(msdnormal, -m_RotationAngle);

		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);

		m_RotationAngle = 0;
	}

	void Magnetron::InputSputRates(const char* filepath, const float& integrationDelta)
	{
		float localRadius = 0;
		float localSputRate = 0;
		double key = NULL;
		double value = NULL;

		std::ifstream stream(filepath);
		if (!stream.good())
		{
			std::cout << "FILEPATH ERROR" << std::endl;
			return;
		}

		std::string line;
		size_t pos = 0;
		auto linecount = 0;

		while (getline(stream, line))
		{
			++linecount;
			auto keycount = 0;
			while ((pos = line.find(" ")) != std::string::npos and keycount < 1)
			{
				key = std::stod(line.substr(0, pos));
				++keycount;
				line.erase(0, line.find(" ") + 1);
			}
			value = std::stod(line);
			m_InputSputRates.insert({ key * 100, value });
		}

		for (const auto& elem : m_InputSputRates)
		{
			std::cout << elem.first << " " << elem.second << "\n";
		}

		for (auto i = -m_Radius; i < m_Radius; i += integrationDelta)
		{
			for (auto j = -m_Radius; j < m_Radius; j += integrationDelta)
			{
				if ((localRadius = sqrt(i * i + j * j)) > m_Radius) continue;

				localSputRate = Approx(localRadius, m_InputSputRates);
				m_SputRates.insert({ localRadius, localSputRate });
			}
		}
	}

	float Magnetron::FindSputRate(const float& radius)
	{
		if (auto it = m_SputRates.find(radius); it != m_SputRates.end())
		{
			return it->second;
		}
		return 0.0f;
	}

	void Magnetron::WriteDepRate()
	{
		m_DepRates.push_back(m_CurrentDepRate);
	}

	void Magnetron::WriteGamma(const float& gamma)
	{
		m_GammaAngles.push_back(gamma);
	}

	void Magnetron::WritePhi(const float& phi)
	{
		m_PhiAngles.push_back(phi);
	}

	Substrate::Substrate(const vec3& pos, const vec3& normal, const float& rpm, const float& subrpm)
		: subpos(pos), subnormal(normal), RPM(rpm), subRPM(subrpm)
	{
	}

	void Substrate::Rotate()
	{
		m_RotationAngle = m_RotationAngle * PI / 180;
		subpos = RotateAroundZ(subpos, -m_RotationAngle);
		subnormal = RotateAroundZ(subnormal, -m_RotationAngle);
		m_RotationAngle = 0;
	}
	void Substrate::Update()
	{
		m_TotalAngle += m_TotalAngleDelta;
		m_TotalSubAngle += m_TotalSubAngleDelta;
		subpos = RotateAroundZ(subpos, m_TotalAngleDelta);
		subnormal = RotateAroundZ(subnormal, m_TotalAngleDelta+m_TotalSubAngleDelta);

		if (m_TotalAngle >= 2 * PI) m_TotalAngle -= 2 * PI;
		if (m_TotalAngle < 0) m_TotalAngle += 2 * PI;
		if (m_TotalSubAngle >= 2 * PI) m_TotalSubAngle -= 2 * PI;
		if (m_TotalSubAngle < 0) m_TotalSubAngle += 2 * PI;
	}

	void Substrate::WriteDepEvolution()
	{
		m_DepEvolution.push_back(m_TotalDeposited);
	}

}

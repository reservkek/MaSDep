#include "msdpch.h"

#include "MSDObjects.h"

namespace MSD {

	////////////////////////////////
	// GENERAL ANGULAR MSD OBJECT //
	////////////////////////////////

	std::unordered_map<unsigned int, AngMSDObject*> AngMSDObject::s_Objects
		= std::unordered_map<unsigned int, AngMSDObject*>();
	
	std::vector<unsigned int> AngMSDObject::s_KeyValues = std::vector<unsigned int>();

	AngMSDObject* AngMSDObject::GetObject(unsigned int id)
	{
		return s_Objects[id];
	}

	AngMSDObject::AngMSDObject(const vec3& pos, const vec3& normal)
		: msdpos(pos), msdnormal(normal)
	{
		m_Object = new Rect();
		m_NormalVectorArrow.reset(new Arrow);
		m_ID = NULL;
	}

	AngMSDObject::~AngMSDObject()
	{
	}

	void AngMSDObject::SetID(unsigned int val)
	{
		if (val == m_ID) return;
		bool validID = (s_Objects.find(val) == s_Objects.end());
		if (val == 0) validID = true;
		if (!validID)
		{
			if (s_Objects[val] == nullptr)
			{
				s_Objects.erase(val);
				validID = true;
			}
			else
			{
				std::cout << "ID " << val << " is already taken. Trying next...\n";
				SetID(val + 1);
				return;
			}
		}
		s_Objects.emplace(std::make_pair(val, this));
		s_KeyValues.push_back(val);

		m_ID = val;
		m_Object->SetID(val);
		std::cout << "ID: " << val << " was set to " << GetType() << std::endl;
	}

	void AngMSDObject::DeleteObject(unsigned int val)
	{
		auto pos = std::find(s_KeyValues.begin(), s_KeyValues.end(), val);
		if (pos != s_KeyValues.end())
		{
			s_Objects.erase(val);
			s_KeyValues.erase(pos);			
		}
		else
			std::cout << "Could not delete an object. Object does not have a record in \"Key Values\"\n";
	}

	void AngMSDObject::Delete()
	{
		s_KeyValues.erase(std::remove(s_KeyValues.begin(), s_KeyValues.end(), m_ID), s_KeyValues.end());
		s_Objects.erase(m_ID);
	}

	float AngMSDObject::CalcAngle()
	{
		auto res = AngleProjectionXY(msdnormal, { 0, 1, 0 });
		if (msdnormal.x < 0) return res;
		else return -res;
	}

	////////////////////////////////
	// MAGNETRON OBJECT FUNCTIONS //
	////////////////////////////////

	Magnetron::Magnetron(const vec3& pos, const vec3& normal, const float& radius)
		: AngMSDObject(pos, normal), m_Radius(radius)
	{
		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);
	}

	Magnetron::~Magnetron()
	{
		std::cout << "ID " << m_ID << " was set free when " << GetType() << " was deleted.\n";
	}

	void Magnetron::RotateAroundCenter(float rotationangle)
	{
		if (rotationangle == 0) rotationangle = m_RotationAngle * PI / 180;
		msdpos = RotateAroundZ(msdpos, -rotationangle);
		msdnormal = RotateAroundZ(msdnormal, -rotationangle);

		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);

		m_RotationAngle = 0;
	}

	void Magnetron::Rotate(float rotationangle, glm::vec3 axis)
	{
		if (rotationangle == 0) rotationangle = m_RotationAngle * PI / 180;
		msdnormal = glm::rotate(glm::vec3(msdnormal), -rotationangle, axis);

		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);

		m_RotationAngle = 0;
	}

	void Magnetron::InputSputRates(const char* filepath, const float& integrationDelta)
	{
		m_FilePathErr = false;

		float localRadius = 0;
		float localSputRate = 0;
		float key = NULL;
		float value = NULL;

		std::ifstream stream(filepath);
		if (!stream.good())
		{
			m_FilePathErr = true;
			m_ErrorMsg = std::string("Couldn't find the file for the magnetron.") + std::string("\nPlease make sure that the path is correct \n\n");
			stream.close();
			return;
		}

		std::string line;
		size_t pos = 0;
		auto linecount = 0;

		while (std::getline(stream, line))
		{
			if (!line.length())
			{
				m_ErrorMsg = std::string("An error occured while reading input file for the magnetron #");
				m_ErrorMsg.append(std::to_string(m_Index));
				m_ErrorMsg.append("\nPlease make sure that the file has correct format.\n\n");
				m_FilePathErr = true;
				stream.close();
				return;
			}
			++linecount;
			auto keycount = 0;
			while ((pos = line.find(" ")) != std::string::npos and keycount < 1)
			{
				key = (float)std::stod(line.substr(0, pos));
				++keycount;
				line.erase(0, line.find(" ") + 1);
			}
			value = (float)std::stod(line);
			m_InputSputRates.insert({ key * 100, value });
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

		stream.close();
	}

	void Magnetron::Clear()
	{
		m_DepRates.clear();
		m_GammaAngles.clear();
		m_PhiAngles.clear();
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

	////////////////////////////////
	// SUBSTRATE OBJECT FUNCTIONS //
	////////////////////////////////

	Substrate::Substrate(const vec3& pos, const vec3& normal, const float& rpm, const float& subrpm)
		: AngMSDObject(pos, normal), RPM(rpm), subRPM(subrpm)
	{
	}

	Substrate* Substrate::clone()
	{
		return new Substrate(msdpos, msdnormal, RPM, subRPM);
	}

	void Substrate::RotateAroundCenter(float rotationangle)
	{
		if (rotationangle == 0) rotationangle = m_RotationAngle * PI / 180;
		msdpos = RotateAroundZ(msdpos, -rotationangle);
		msdnormal = RotateAroundZ(msdnormal, -rotationangle);
		m_RotationAngle = 0;
	}

	void Substrate::Update()
	{
		// Updating substrate position and angle during simulation

		m_TotalAngle += m_TotalAngleDelta;
		m_TotalSubAngle += m_TotalSubAngleDelta;
		msdpos = RotateAroundZ(msdpos, m_TotalAngleDelta);
		msdnormal = RotateAroundZ(msdnormal, m_TotalAngleDelta+m_TotalSubAngleDelta);

		// Setting angle range from 0 to 360 degrees

		if (m_TotalAngle >= 2 * PI) m_TotalAngle -= 2 * PI;
		if (m_TotalAngle < 0) m_TotalAngle += 2 * PI;
		if (m_TotalSubAngle >= 2 * PI) m_TotalSubAngle -= 2 * PI;
		if (m_TotalSubAngle < 0) m_TotalSubAngle += 2 * PI;
	}

	void Substrate::Rotate(float rotationangle, glm::vec3 axis)
	{
		if (rotationangle == 0) rotationangle = m_RotationAngle * PI / 180;
		msdnormal = glm::rotate(glm::vec3(msdnormal), -rotationangle, axis);
		m_RotationAngle = 0;
	}

	void Substrate::WriteDepEvolution()
	{
		m_DepEvolution.push_back(m_TotalDeposited);
	}

}

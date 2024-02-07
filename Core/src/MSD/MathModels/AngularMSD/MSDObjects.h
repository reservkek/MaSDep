#pragma once


#include "Core.h"

#include "Graphics/Internal/Objects.h"

#include "MSDMath.h"
#include "../Database/PhysicsData.h"

namespace MSD {

	using namespace Database;

	enum CalculationParams : int { ANGMSD_USE_FILE, ANGMSD_CALC_RAW };
	enum MagneticFieldVariant : int { ANGMSD_MAGFIELD_STANDARD, ANGMSD_MAGFIELD_FROMFILE };
	enum SputteringYieldType : int { ANGMSD_YIELD_CUSTOM, ANGMSD_YIELD_SIGMUND };

	template <typename T>
	std::map<T, T> InputFromFile(const char* filepath);
	template <typename T>
	std::map<T, T>Normalize(const std::map<T, T>& map, float coeff = 1);

	class AngMSDObject
	{
	public:
		AngMSDObject(const vec3& pos, const vec3& normal);
		~AngMSDObject();

		void SetID(unsigned int val);
		int GetID() const { return m_ID; }
		static void DeleteObject(unsigned int val);
		void Delete();

		virtual void RotateAroundCenter(float rotationangle = 0) = 0;
		virtual void Rotate(float rotationangle = 0, glm::vec3 axis = glm::vec3(0, 0, 1)) = 0;

		float* GetPosX() { return &(msdpos.x); }
		float* GetPosY() { return &(msdpos.y); }
		float* GetPosZ() { return &(msdpos.z); }
		float* GetNormalX() { return &(msdnormal.x); }
		float* GetNormalY() { return &(msdnormal.y); }
		float* GetNormalZ() { return &(msdnormal.z); }

		virtual std::string GetType() const { return m_Type; }

		static AngMSDObject* GetObject(unsigned int id);

		vec3& GetPos() { return msdpos; }
		vec3& GetNormal() { return msdnormal; }

		float CalcAngle();

		Arrow* GetArrow() { return m_NormalVectorArrow.get(); }

		void SetGraphicsObject(Object* obj) { m_Object = obj; }
		Object* GetGraphicsObject() const { return m_Object; }


		friend class MainLayer;
	protected:
		unsigned int m_ID;
		// Geometry
		vec3 msdpos, msdnormal;

		// Graphics container
		Object* m_Object = nullptr;

		std::shared_ptr<Arrow> m_NormalVectorArrow;
		float m_RotationAngle = 0;

		std::string m_Type;

		static std::unordered_map<unsigned int, AngMSDObject*> s_Objects;
		static std::vector<unsigned int> s_KeyValues;
	};

	class Magnetron : public AngMSDObject
	{
	public:
		vec3 integrationvectorI;
		vec3 integrationvectorJ;

		Magnetron(const vec3& pos = { 0.0, 25.0, 0.0 }, const vec3& normal = { 0.0, -1.0, 0.0 }, const float& radius = 4.5);
		~Magnetron();

		virtual void RotateAroundCenter(float rotationangle = 0) override;
		virtual void Rotate(float rotationangle = 0, glm::vec3 axis = glm::vec3(0,0,1)) override;

		void WriteDepRate();
		void WriteGamma(const float& gamma);
		void WritePhi(const float& phi);
		void InputSputRates(const char* filepath, const float& integrationDelta);
		void RawCalcSputRates(float integrationDelta, Element gas);
		void Clear();

		const int SputRatesCalculationType() { return m_CalculationParameters; }

		virtual std::string GetType() const override { return m_Type; };

		const int AngleContainerSize() const { return (int)m_PhiAngles.size(); }
		float FindSputRate(const float& radius);

		float* GetRadius() { return &m_Radius; }
		float* GetRotationAngle() { return &(m_RotationAngle); }
		float& GetCurrentDepRate() { return m_CurrentDepRate; }
		float& GetPD() { return m_pd; }
		char** GetInputFilePath() { return &m_InputFilePath; }

		float GetPower() { return m_Voltage * m_Current; }

		std::vector<float>& GetDepRates() { return m_DepRates; }

		std::string GetErrorMessage() { return m_ErrorMsg;}

		bool& GetFilePathErr() { return m_FilePathErr; }

		Element& GetElement() { return m_Element; }
	private:
		// Geometry
		float m_Radius;

		// Element
		Element m_Element = Cr;

		// Sput rates used for calculations
		std::map<float, float> m_InputSputRates;
		std::map<float, float> m_SputRates;

		float m_CurrentDepRate = 0;
		float m_RotationAngle = 0;

		// Result containers
		std::vector<float> m_DepRates; // Deposition rates onto substrate;
		std::vector<float> m_GammaAngles; 
		std::vector<float> m_PhiAngles;

		char* m_InputFilePath = new char();

		unsigned int m_Index = 0;


		// For raw calculations
		int m_CalculationParameters = ANGMSD_USE_FILE;
		int m_Voltage = 300;
		int m_SputteringYieldType = ANGMSD_YIELD_CUSTOM;
		float m_SputteringYield = 1.0f;
		float m_Current = 5.0;
		float m_Power = m_Current * m_Voltage;
		int m_MagneticField = ANGMSD_MAGFIELD_STANDARD;
		std::map<float, float> m_MagneticFieldDistributionInput = InputFromFile<float>("../assets/data/standartmagneticfield.txt");
		std::map<float, float> m_MagneticFieldDistribution;


		bool m_FilePathErr = false;
		std::string m_ErrorMsg = "";

		std::string m_Type = "Magnetron";

				// Flux scattering
		float m_pd = 0;

		// Reactive Model
		float m_Coverage = 0.0f;

		friend class MainLayer;
	};

	class Substrate : public AngMSDObject
	{
	public:
		Substrate(const vec3& pos = { 0.0, 15.0, 0.0 }, const vec3& normal = { 0.0, 1.0, 0.0 },
			const float& rpm = 0, const float& subrpm = 0);

		Substrate* clone();

		virtual void RotateAroundCenter(float rotationangle = 0) override;
		virtual void Rotate(float rotationangle = 0, glm::vec3 axis = glm::vec3(0, 0, 1)) override;
		void Update();
		void WriteDepEvolution();

		float* GetRPM() { return &RPM; }
		float* GetSubRPM() { return &subRPM; }
		float* GetRotationAngle() { return &m_RotationAngle; }
		float& GetTotalAngle() { return m_TotalAngle; }
		float& GetTotalAngleDelta() { return m_TotalAngleDelta; }
		float& GetTotalSubAngle() { return m_TotalSubAngle; }
		float& GetTotalSubAngleDelta() { return m_TotalSubAngleDelta; }
		float& GetTotalDeposited() { return m_TotalDeposited; }

		virtual std::string GetType() const override { return m_Type; };

		std::vector<float>& GetDepEvolution() { return m_DepEvolution; }
	private:
		float RPM, subRPM; // Rotations per minute
		float m_RotationAngle = 0;
		float m_TotalAngle = 0;
		float m_TotalAngleDelta = 0;
		float m_TotalSubAngle = 0;
		float m_TotalSubAngleDelta = 0;
		float m_TotalDeposited = 0;

		std::vector<float> m_TimeEvolution;
		std::vector<float> m_DepEvolution;

		std::string m_Type = "Substrate";
	};

	template <typename T>
	std::map<T, T> InputFromFile(const char* filepath)
	{
		std::map<T, T> result;
		std::ifstream stream(filepath);
		if (!stream.good())
		{
			stream.close();
			return std::map<T,T>();
		}

		std::string line;
		size_t pos = 0;
		auto linecount = 0;
		float key, value;

		while (std::getline(stream, line))
		{
			if (!line.length())
			{
				stream.close();
				return std::map<T, T>();
			}
			++linecount;
			auto keycount = 0;
			while ((pos = line.find(" ")) != std::string::npos and keycount < 1)
			{
				key = (T)std::stod(line.substr(0, pos));
				++keycount;
				line.erase(0, line.find(" ") + 1);
			}
			value = (T)std::stod(line);
			result.insert({ key * 100 , value });
		}

		return result;
	}

	template<typename T>
	std::map<T, T> Normalize(const std::map<T, T>& map, float coeff)
	{
		auto copy = map;
		float sum = 0.0f;
		for (auto& val : copy | std::views::values)
		{
			sum += val;
		}
		for (auto& val : copy | std::views::values)
		{
			val /= (sum*coeff);
		}
		return copy;
	}

}
#pragma once


#include "Core.h"

#include "Graphics/Internal/Objects.h"

#include "MSDMath.h"
#include "../Database/PhysicsData.h"

#include "UserInterface/Languages.h"

import EnergyDistribution;

namespace MSD {

	using namespace Database;

	enum AngMSD_ObjectType : int { ANGMSD_OBJECT_MAGNETRON, ANGMSD_OBJECT_SUBSTRATE };
	enum CalculationParams : int { ANGMSD_USE_FILE, ANGMSD_CALC_RAW };
	enum MagneticFieldVariant : int { ANGMSD_MAGFIELD_STANDARD, ANGMSD_MAGFIELD_FROMFILE };
	enum SputteringYieldType : int { ANGMSD_YIELD_CUSTOM, ANGMSD_YIELD_SIGMUND };

	template <typename T>
	std::map<T, T>	InputFromFile(const char* filepath);
	template <typename T>
	std::map<T, T>	Normalize(const std::map<T, T>& map, float coeff = 1);

	class AngMSDObject
	{
	public:
		AngMSDObject(const vec3& pos, const vec3& normal);
		~AngMSDObject();

		inline int					GetID() const				{ return m_ID; }
		inline float*				GetPosX()					{ return &(msdpos.x); }
		inline float*				GetPosZ()					{ return &(msdpos.z); }
		inline float*				GetPosY()					{ return &(msdpos.y); }
		inline float*				GetNormalX()				{ return &(msdnormal.x); }
		inline float*				GetNormalY()				{ return &(msdnormal.y); }
		inline float*				GetNormalZ()				{ return &(msdnormal.z); }
		inline virtual std::string	GetType() const				{ return m_Type; }
		vec3&						GetPos()					{ return msdpos; }
		vec3&						GetNormal()					{ return msdnormal; }
		Arrow*						GetArrow()					{ return m_NormalVectorArrow.get(); }
		Object*						GetGraphicsObject() const	{ return m_Object; }
		static AngMSDObject*		GetObject(unsigned int id);

		void						SetID(unsigned int val);
		void						SetGraphicsObject(Object* obj) { m_Object = obj; }
		virtual void				RotateAroundCenter(float rotationangle = 0) = 0;
		virtual void				Rotate(float rotationangle = 0, glm::vec3 axis = glm::vec3(0, 0, 1)) = 0;
		static void					DeleteObject(unsigned int val);
		void						Delete();
		float						CalcAngle();

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

		void Clear();

		void RotateAroundCenter(float rotationangle = 0) override;
		void Rotate(float rotationangle = 0, glm::vec3 axis = glm::vec3(0,0,1)) override;

		void					WriteDepRate();
		void					WriteGamma(const float& gamma);
		void					WritePhi(const float& phi);
		void					InputSputRates(const char* filepath, const float& integrationDelta);
		void					RawCalcSputRates(float integrationDelta, Element gas);
		void					CalcMeanDepRate();
		void					CalcEnergyDistribution(Element gas);
		float					FindSputRate(const float& radius);

		std::vector<float>&		GetDepRates()						{ return m_DepRates; }
		int						GetSputRatesCalculationType() const { return m_CalculationParameters; }
		virtual std::string		GetType() const override			{ return m_Type; };
		float&					GetRadius()							{ return m_Radius; }
		float&					GetRotationAngle()					{ return m_RotationAngle; }
		float&					GetCurrentDepRate()					{ return m_CurrentDepRate; }
		float&					GetPD()								{ return m_pd; }
		float					GetPower()							{ return m_Voltage * m_Current; }
		float					GetMeanIonEnergy()					{ return m_Voltage * m_CoeffVoltage; }
		float					GetMeanDepRate()					{ return m_MeanDepRate; }
		float					GetOverallSputRate()				{ return m_OverallSputteringRate; }
		char**					GetInputFilePath()					{ return &m_InputFilePath; }
		std::string				GetErrorMessage()					{ return m_ErrorMsg;}
		bool&					GetFilePathErr()					{ return m_FilePathErr; }
		Element&				GetElement()						{ return m_Element; }
		EnergyDistribution*		GetEnergyDistribution()				{ return &m_EnergyDistribution; }

	private:
		bool m_FilePathErr = false;

		std::string m_ErrorMsg = "";

		std::string m_Type = MSDSTR_MAGNETRON;

		unsigned int m_Index = 0; // OBSOLETE

		// Geometry
		float m_Radius;

		// Element
		Element m_Element = Cr;

		// Sput rates used for calculations
		std::map<float, float> m_InputSputRates;
		std::map<float, float> m_SputRates;

		char* m_InputFilePath = new char();

		// For raw calculations
		int m_CalculationParameters = ANGMSD_USE_FILE;
		int m_SputteringYieldType = ANGMSD_YIELD_CUSTOM;
		int m_MagneticField = ANGMSD_MAGFIELD_STANDARD;
		float m_SputteringYield = 1.0f;
		int m_Voltage = 300;
		float m_Current = 5.0f;
		float m_Power = m_Current * m_Voltage;
		float m_CoeffVoltage = 0.733f;
		float m_CoeffCurr = 0.9f;
		std::map<float, float> m_MagneticFieldDistributionInput = InputFromFile<float>("../assets/data/standartmagneticfield.txt");
		std::map<float, float> m_MagneticFieldDistribution;

		// Flux scattering
		float m_pd = 0;

		// Reactive Model
		float m_Coverage = 0.0f;

		float m_CurrentDepRate = 0;
		float m_RotationAngle = 0;

		// Result containers
		std::vector<float> m_DepRates; // Deposition rates onto substrate;
		std::vector<float> m_GammaAngles;
		std::vector<float> m_PhiAngles;
		float m_OverallSputteringRate = 0.0f;
		float m_MeanDepRate = 0.0f;

		// Energy Distribution
		EnergyDistribution m_EnergyDistribution;

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
		
		void						Update();
		void						Clear();
		void						WriteDepEvolution();
		void						CalcMeanAngle();
		void						CalcComposition();

		float&						GetRPM()					{ return RPM; }
		float&						GetSubRPM()					{ return subRPM; }
		float&						GetRotationAngle()			{ return m_RotationAngle; }
		float&						GetTotalAngle()				{ return m_TotalAngle; }
		float&						GetTotalAngleDelta()		{ return m_TotalAngleDelta; }
		float&						GetTotalSubAngle()			{ return m_TotalSubAngle; }
		float&						GetTotalSubAngleDelta()		{ return m_TotalSubAngleDelta; }
		float&						TotalDeposited()			{ return m_TotalDeposited; }
		float&						TotalDepositedRaw()			{ return m_TotalDepositedRaw; }
		std::map<Element, float>&	Composition()				{ return m_Composition; }
		std::map<Element, float>&	CompositionRaw()			{ return m_CompositionRaw; }
		std::vector<float>&			GetDepEvolution()			{ return m_DepEvolution; }
		std::string					GetType() const override	{ return m_Type; };

		float MeanIncidentAngle_raw;
		float MeanIncidentAngle;

	private:
		float RPM, subRPM; // Rotations per minute
		float m_RotationAngle = 0;
		float m_TotalAngle = 0;
		float m_TotalAngleDelta = 0;
		float m_TotalSubAngle = 0;
		float m_TotalSubAngleDelta = 0;
		float m_TotalDeposited = 0;
		float m_TotalDepositedRaw = 0;

		std::vector<float> m_TimeEvolution;
		std::vector<float> m_DepEvolution;
		std::map<Element, float> m_CompositionRaw;
		std::map<Element, float> m_Composition;

		std::string m_Type = MSDSTR_SUBSTRATE;
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
#pragma once


#include "Core.h"

#include "Graphics/Internal/Objects.h"

#include "MSDMath.h"
#include "../Database/PhysicsData.h"

namespace MSD {

	using namespace Database;

	enum CalculationParams : int { MSD_USE_FILE, MSD_CALC_RAW };

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
		void Clear();

		virtual std::string GetType() const override { return m_Type; };

		const int AngleContainerSize() const { return (int)m_PhiAngles.size(); }
		float FindSputRate(const float& radius);

		float* GetRadius() { return &m_Radius; }
		float* GetRotationAngle() { return &(m_RotationAngle); }
		float& GetCurrentDepRate() { return m_CurrentDepRate; }
		float& GetPD() { return m_pd; }
		char** GetInputFilePath() { return &m_InputFilePath; }

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

		int m_CalculationParameters = MSD_USE_FILE;
		int m_Voltage = 300;
		float m_Current = 5.0;
		float m_Power = m_Current * m_Voltage;


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
}
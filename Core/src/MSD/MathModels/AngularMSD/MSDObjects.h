#pragma once


#include "Core.h"

#include "Graphics/Internal/Objects.h"

#include "MSDMath.h"
#include "../Database/PhysicsData.h"

namespace MSD {

	using namespace Database;

	class AngMSDObject
	{
	public:
		virtual float* GetPosY() = 0;
		virtual float* GetPosZ() = 0;
		virtual float* GetPosX() = 0;
		virtual float* GetNormalX() = 0;
		virtual float* GetNormalY() = 0;
		virtual float* GetNormalZ() = 0;
	};

	class MSD_API Magnetron : public AngMSDObject
	{
	public:
		vec3 integrationvectorI;
		vec3 integrationvectorJ;

		Magnetron(const vec3& pos = { 0.0, 25.0, 0.0 }, const vec3& normal = { 0.0, -1.0, 0.0 }, const float& radius = 4.5);
		~Magnetron();

		void SetIndex(unsigned int val) { m_Index = val; }
		void SetGraphicsObject(Object* obj) { m_Object = obj;  }
		void Rotate();
		void WriteDepRate();
		void WriteGamma(const float& gamma);
		void WritePhi(const float& phi);
		void InputSputRates(const char* filepath, const float& integrationDelta);
		void Clear();

		float CalcAngle();

		float FindSputRate(const float& radius);

		int GetIndex() { return m_Index; }
		float* GetRadius() { return &m_Radius; }
		float* GetPosX() { return &(msdpos.x); }
		float* GetPosY() { return &(msdpos.y); }
		float* GetPosZ() { return &(msdpos.z); }
		float* GetNormalX() { return &(msdnormal.x); }
		float* GetNormalY() { return &(msdnormal.y); }
		float* GetNormalZ() { return &(msdnormal.z); }
		float* GetRotationAngle() { return &(m_RotationAngle); }
		float& GetCurrentDepRate() { return m_CurrentDepRate; }
		char** GetInputFilePath() { return &m_InputFilePath; }

		Object* GetGraphicsObject() const { return m_Object; }

		std::vector<float>& GetDepRates() { return m_DepRates; }

		std::string GetErrorMessage() { return m_ErrorMsg;}

		bool& GetFilePathErr() { return m_FilePathErr; }

		vec3 GetPos() { return msdpos; }
		vec3 GetNormal() const { return msdnormal; }

		Element& GetElement() { return m_Element; }

		extern friend class MainLayer;
		extern friend class AngMSD;

	private:
		// Geometry
		vec3 msdpos, msdnormal;
		float m_Radius;

		// Element
		Element m_Element = Cr;

		// Sput rates used for calculations
		std::map<float, float> m_InputSputRates;
		std::map<float, float> m_SputRates;

		float m_CurrentDepRate = 0;

		// Result containers
		std::vector<float> m_DepRates; // Deposition rates onto substrate;
		std::vector<float> m_GammaAngles; 
		std::vector<float> m_PhiAngles;

		char* m_InputFilePath = new char();

		unsigned int m_Index = 0;
		float m_RotationAngle = 0;

		bool m_FilePathErr = false;
		std::string m_ErrorMsg = "";

		// Graphics container
		Object* m_Object = nullptr;
	};

	class MSD_API Substrate : public AngMSDObject
	{
	public:
		Substrate(const vec3& pos = { 0.0, 15.0, 0.0 }, const vec3& normal = { 0.0, 1.0, 0.0 },
			const float& rpm = 0, const float& subrpm = 0);

		void Rotate();
		void Update();
		void WriteDepEvolution();

		void SetGraphicsObject(Object* obj) { m_Object = obj; }

		float CalcAngle();

		float* GetPosX() { return &(subpos.x); }
		float* GetPosY() { return &(subpos.y); }
		float* GetPosZ() { return &(subpos.z); }
		float* GetNormalX() { return &(subnormal.x); }
		float* GetNormalY() { return &(subnormal.y); }
		float* GetNormalZ() { return &(subnormal.z); }
		float* GetRPM() { return &RPM; }
		float* GetSubRPM() { return &subRPM; }
		float* GetRotationAngle() { return &m_RotationAngle; }
		float& GetTotalAngle() { return m_TotalAngle; }
		float& GetTotalAngleDelta() { return m_TotalAngleDelta; }
		float& GetTotalSubAngle() { return m_TotalSubAngle; }
		float& GetTotalSubAngleDelta() { return m_TotalSubAngleDelta; }
		float& GetTotalDeposited() { return m_TotalDeposited; }

		std::vector<float>& GetDepEvolution() { return m_DepEvolution; }

		Object* GetGraphicsObject() { return m_Object; }

		vec3 GetPos() const { return subpos; };
		vec3 GetNormal() const { return subnormal; };

	private:
		// Graphics container
		Object* m_Object = nullptr;

		vec3 subpos, subnormal;
		float RPM, subRPM; // Rotations per minute
		float m_RotationAngle = 0;
		float m_TotalAngle = 0;
		float m_TotalAngleDelta = 0;
		float m_TotalSubAngle = 0;
		float m_TotalSubAngleDelta = 0;
		float m_TotalDeposited = 0;

		std::vector<float> m_TimeEvolution;
		std::vector<float> m_DepEvolution;
	};
}
#pragma once


#include "Core.h"

#include "MSDMath.h"

namespace MSD {
	class MSD_API Magnetron
	{
	public:
		vec3 integrationvectorI;
		vec3 integrationvectorJ;

		Magnetron(const vec3& pos = { 0.0, 25.0, 0.0 }, const vec3& normal = { 0.0, -1.0, 0.0 }, const double& radius = 5);
		~Magnetron();

		void setLocalPos(const vec3& pos) { msdlocalpos = pos; }
		void SetIndex(unsigned int val) { m_Index = val; }

		int GetIndex() const { return m_Index; }
		vec3 getPos() const { return msdpos; }
		vec3 getNormal() const { return msdnormal; }
		vec3 getLocalPos() const { return msdlocalpos; }
		double getRadius() const { return msdradius; }

		void Rotate(const double& angle);

	private:
		unsigned int m_Index = 0;
		vec3 msdpos, msdnormal;
		vec3 msdlocalpos;
		const double msdradius;
	};
}
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

	void Magnetron::InputSputRates(const char* str)
	{
	}

	float Magnetron::FindSputRate(const float& radius)
	{
		return 0.0f;
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
}

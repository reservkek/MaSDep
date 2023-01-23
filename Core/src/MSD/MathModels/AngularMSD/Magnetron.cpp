#include "Magnetron.h"

namespace MSD {

	MSD::Magnetron::Magnetron(const vec3& pos, const vec3& normal, const double& radius)
		: msdpos(pos), msdnormal(normal), msdradius(radius)
	{
		integrationvectorI = FindOrthogonal(msdnormal);
		integrationvectorJ = CrossProduct(msdnormal, integrationvectorI);
	}

	MSD::Magnetron::~Magnetron()
	{
	}

	void MSD::Magnetron::Rotate(const double& angle)
	{
		msdpos = RotateAroundZ(msdpos, angle);
		msdnormal = RotateAroundZ(msdnormal, angle);
	}
}

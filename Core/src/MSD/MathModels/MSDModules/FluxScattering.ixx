#include "msdpch.h"

export module FluxScattering;

namespace MSD
{

export namespace FluxScattering
{
	// m1, m2, r1, r2, T, en_bind
	float CalcPD(float m1, float m2, float r1, float r2, float T, float en_bind)
	{
		return (0.107f * pow((m1 / m2), 0.69f) * (0.62f + 0.23f * log(7736.4f * en_bind / T)) * T / (pow((r1 + r2), 2.0f)));
	}

	float GeometryFactor(float r, float d)
	{
		return r * r / (r * r + d * d);
	}

	float FluxPressureInteractionCoeff(float distance, float pressure, float pd, float geom_fact)
	{
		return geom_fact*pd/(pressure*distance)*(1-exp(-(pressure * distance)/pd));
	}

}

}
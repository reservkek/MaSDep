#include "msdpch.h"

#include "MathModels/Database/PhysicsData.h"
#include "MathModels/AngularMSD/MSDObjects.h"

#pragma warning(disable:5201)

export module SputteringRates;

export namespace MSD
{

export namespace SputteringRates
{
	using namespace Database;

	float SputteringYield(Element gas, Magnetron* magnetron)
	{
		float m1 = GetAtomicMass(gas);
		float m2 = GetAtomicMass(magnetron->GetElement());
		float y = 4.0f * m1 * m2 / (pow((m1 + m2), 2.0f));
		float alpha = 0.1f + 0.16f * pow((m2 / m1), 0.7f);

		float yield = 0.0539535f * alpha * y * magnetron->GetPower() / GetBindingEnergy(magnetron->GetElement());
		return yield;
	}

}

}
#include "msdpch.h"

#include "MathModels/Database/PhysicsData.h"

#pragma warning(disable:5201)

export module EnergyDistribution;

namespace MSD
{
	export class EnergyDistribution
	{
	private:
		float MeanEnergy;

		std::map<float, float> DistributionOfEnergy;

		float ThompsonDistribution(float en, float u_bind, float en_ion, float y)
		{
			return (float)(1 - sqrt((u_bind + en) / (y * en_ion))) * en / pow((u_bind + en), 3);
		}

	public:

		void Calculate(float m1, float m2, float u_bind, float en_ion)
		{

			float y = 4.0f * m1 * m2 / (pow((m1 + m2), 2.0f));

			DistributionOfEnergy.clear();

			float sum = 0;
			float functionsum = 0;
			float distrib = 0;

			for (float energy = 0; energy < en_ion; energy += 0.1)
			{
				distrib = ThompsonDistribution(energy, u_bind, en_ion, y);
				if (distrib < 0) distrib = 0;
				sum += distrib;
				functionsum += energy * distrib;
				DistributionOfEnergy.insert({ energy, distrib });
			}

			MeanEnergy = functionsum / sum;

			for (auto pair : DistributionOfEnergy)
			{
				pair.second /= (sum*0.1);
			}

		}

		std::map<float, float>& GetData()
		{
			return DistributionOfEnergy;
		}

		float GetMeanEnergy()
		{
			return MeanEnergy;
		}
	};
}
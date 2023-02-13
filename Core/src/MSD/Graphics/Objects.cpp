#include "msdpch.h"

#include "Objects.h"

namespace MSD {

	MSSubstrate::MSSubstrate(const float* pos)
	{
		coords[0] = pos[0] - 30.0f;  coords[1] = pos[1] - 3.0f;  coords[2] = 0.0f; // 0
		coords[3] = pos[0] + 30.0f;  coords[4] = pos[1] - 3.0f;  coords[5] = 0.0f; // 1
		coords[6] = pos[0] + 30.0f;  coords[7] = pos[1] + 3.0f;  coords[8] = 0.0f; // 2
		coords[9] = pos[0] - 30.0f; coords[10] = pos[1] + 3.0f; coords[11] = 0.0f; // 3
	}

	MSMagnetron::MSMagnetron(const float* pos, const float& radius)
	{
		coords[0] = pos[0] - radius * 10.0f;  coords[1] = pos[1] - 5.0f;  coords[2] = 0.0f; // 0
		coords[3] = pos[0] + radius * 10.0f;  coords[4] = pos[1] - 5.0f;  coords[5] = 0.0f; // 1
		coords[6] = pos[0] + radius * 10.0f;  coords[7] = pos[1] + 5.0f;  coords[8] = 0.0f; // 2
		coords[9] = pos[0] - radius * 10.0f; coords[10] = pos[1] + 5.0f; coords[11] = 0.0f; // 3
	}

}
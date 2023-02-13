#pragma once

namespace MSD {

	class MSSubstrate {
	private:
	public:
		float coords[4 * 3];
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0
		};
		unsigned int indicescount = 6;
		MSSubstrate(const float* pos);
	};

	class MSMagnetron {
	private:
	public:
		float coords[4 * 3];
		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0
		};
		unsigned int indicescount = 6;
		MSMagnetron(const float* pos, const float& radius);
	};


}
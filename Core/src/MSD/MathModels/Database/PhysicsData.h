#pragma once

namespace MSD {

	namespace Database {

		enum Element
		{
			None = 0,
				Cr = 1 << 0,
				Al = 1 << 1,
				Cu = 1 << 2,
				Ti = 1 << 3,
		};

		float FindAtomicDensity(Element element);

	}

}
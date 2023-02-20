#pragma once

namespace MSD {

	enum class Elements
	{
		None = 0,
		Cr = 1 << 0
	};

	float FindDensity(Elements element)
	{
		switch (element)
		{
		case Elements::None:     return 0;
		case Elements::Cr:       return 8.33e28f;
		}
		return 0;
	}

}
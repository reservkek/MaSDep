#include "msdpch.h"

#include "PhysicsData.h"

namespace MSD {

	namespace Database {

		// (number of particles/m3)
		float GetAtomicDensity(Element element)
		{
			switch (element)
			{
			case None:     return 0;
			case Cr:       return 8.327e28f;
			case Al:	   return 6.023e28f;
			case Cu:	   return 8.497e28f;
			case Ti:	   return 5.633e28f;
			}
			return 0;
		}

		// (kg/m3)
		float GetDensity(Element element)
		{
			switch (element)
			{
			case None:     return 0;
			case Cr:       return 7190.0f;
			case Al:	   return 2700.0f;
			case Cu:	   return 8960.0f;
			case Ti:	   return 4505.0f;
			}
			return 0;
		}

		int GetAtomicNumber(Element element)
		{
			return (int)element;
		}

		const char* GetName(Element element)
		{
			switch (element)
			{
			case None:     return "None";
			case Cr:       return "Chromium";
			case Al:	   return "Aluminium";
			case Cu:	   return "Copper";
			case Ti:	   return "Titanium";
			}
			return "Null";
		}

		const char* GetSymbol(Element element)
		{
			switch (element)
			{
			case None:     return "None";
			case Cr:       return "Cr";
			case Al:	   return "Al";
			case Cu:	   return "Cu";
			case Ti:	   return "Ti";
			}
			return "Null";
		}

	}
}
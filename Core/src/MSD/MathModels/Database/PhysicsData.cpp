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
			case Ti:	   return 5.633e28f;
			case Cu:	   return 8.497e28f;
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
			case Ti:	   return 4505.0f;
			case Cu:	   return 8960.0f;
			}
			return 0;
		}

		float GetAtomicMass(Element element)
		{
			switch (element)
			{
			case None: return 0.0f;
			case Cr: return 51.996f;
			case Al: return 26.982f;
			case Ti: return 47.867f;
			case Cu: return 63.546f;
			case Ar: return 39.948f;
			}
			return 0.0f;
		}

		// in Angstrems
		float GetAtomicRadius(Element element)
		{
			switch (element)
			{
			case None: return 0.0f;
			case Cr: return 1.28f;
			case Al: return 1.43f;
			case Ti: return 1.47f;
			case Cu: return 1.28f;
			case Ar: return 1.51f;
			}
			return 0.0f;
		}

		float GetBindingEnergy(Element element)
		{
			switch (element)
			{
			case None: return 0.0f;
			case Cr: return 4.10f;
			case Al: return 3.39f;
			case Ti: return 3.8f;
			case Cu: return 3.58f;
			case Ar: return 0.0f;
			}
			return 0.0f;
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
			case Ti:	   return "Titanium";
			case Cu:	   return "Copper";
			case Ar:	   return "Argon";
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
			case Ti:	   return "Ti";
			case Cu:	   return "Cu";
			case Ar:	   return "Ar";
			}
			return "Null";
		}

	}
}
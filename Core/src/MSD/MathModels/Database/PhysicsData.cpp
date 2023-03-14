#include "msdpch.h"

#include "PhysicsData.h"

namespace MSD {

	namespace Database {

		float FindAtomicDensity(Element element)
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

	}
}
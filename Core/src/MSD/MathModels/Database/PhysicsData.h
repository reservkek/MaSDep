#pragma once

namespace MSD {

	namespace Database {

		enum Element : int
		{
			None = 0,
			H, He, // 1 period 
			Li, Be, B, C, N, O, F, Ne, // 2 period
			Na, Mg, Al, Si, P, S, Cl, Ar, // 3 period
			K, Ca, Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr // 4 period
		};

		float GetAtomicDensity(Element element);
		float GetDensity(Element element);
		float GetAtomicMass(Element element);
		int GetAtomicNumber(Element element);
		const char* GetName(Element element);
		const char* GetSymbol(Element element);

	}

}
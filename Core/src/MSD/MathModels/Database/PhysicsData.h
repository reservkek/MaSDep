#pragma once

namespace MSD {
	
	constexpr float q_electron = 1.60217e-19f;
	constexpr float k_boltzmann = 1.38e23f;

	namespace Database {


		enum Element : int
		{
			None = 0,
			H, He, // 1 period 
			Li, Be, B, C, N, O, F, Ne, // 2 period
			Na, Mg, Al, Si, P, S, Cl, Ar, // 3 period
			K, Ca, Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr, // 4 period
			Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe, // 5 period
			Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb,
			Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au, Hg, Tl, Pb, Bi, Po, At, Rn // 6 period
		};

		float GetAtomicDensity(Element element);
		float GetDensity(Element element);
		float GetAtomicMass(Element element);
		float GetAtomicRadius(Element element);
		float GetBindingEnergy(Element element);
		int GetAtomicNumber(Element element);
		const char* GetName(Element element);
		const char* GetSymbol(Element element);


		// RESTRUCTURING DATABASE THROUGH STRUCTS
		namespace Elements
		{

		enum ChElState : int
		{
			Gas = 1, Solid
		};

		enum ChElSeries : int
		{
			Reactive = 1, NobleGas, Alkali, Alkaline, Metalloid, PostTransMetal, TransMetal, Lanthanoid, Actinoid
		};

		struct ChEl
		{
			int AtomicNumber = 0;
			const char* Name = 0;
			const char* Symbol = 0;
			
			int period = 0;
			ChElState state; 
			ChElSeries series;

			float AtomicDensity = 0;
			float Density = 0;
			float AtomicMass = 0;
			float AtomicRadius = 0;
			float BindingEnergy = 0;
		};
					 // Atomic,   Name,   Symbol, Period,  State,       Series,       Atomic ,  Density, Atomic, Atomic, Binding
					 // Number													      Density			   mass   radius  energy
		constexpr ChEl Al (13, "Aluminium", "Al",   3,     Solid,    PostTransMetal, 6.023e28f, 2700.0f, 26.982f,  1.43f,  3.39f);
		constexpr ChEl Ar (18, "Argon",     "Ar",   3,     Gas,      NobleGas,       0,         0,       39.948f,  1.51f,  0    );
		constexpr ChEl Ti (22, "Titanium",  "Ti",   4,     Solid,    TransMetal,     5.633e28f, 4505.0f, 47.867f,  1.47f,  3.8f );
		constexpr ChEl Cr (24, "Chromium",	"Cr",   4,     Solid,    TransMetal,     8.327e28f, 7190.0f, 51.996f,  1.28f,  4.10f);
		constexpr ChEl Cu (29, "Copper",    "Cu",   4,     Solid,    TransMetal,     8.497e28f, 8960.0f, 63.546f,  1.28f,  3.58f);
		constexpr ChEl Y  (39, "Yttrium",    "Y",   5,     Solid,    TransMetal,     3.029e28f, 4472.0f, 88.906f,  1.8f,   3.801f);
		constexpr ChEl Ce (58, "Cerium",    "Ce",   6,     Solid,    Lanthanoid,     2.904e28f, 6757.0f, 140.116f, 1.818f, 4.13f);
		}
	}

}
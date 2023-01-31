#pragma once

#include "Core.h"

#include "imgui.h"

namespace MSD {
	constexpr float PI = 3.14159265358979f;

	// 3D VECTOR DATATYPE /////////////////
	struct MSD_API vec3
	{
		float x, y, z;
		
		vec3(double x = 0, double y = 0, double z = 0);

		vec3& operator=(const vec3& a);
		vec3 operator*(const double& a);
		vec3 operator+(const double& a) const;
		vec3 operator+(const vec3& a) const;
		vec3 operator-(const vec3& a) const;
		vec3 operator-() const;
	};

	// MATH FUNCTIONS //////////////////// 
	double Magnitude(const vec3& a);
	double DotProduct(const vec3& a, const vec3& b);
	double Angle(const vec3& a, const vec3& b);
	float Approx(const float& radius, std::map<float, float> map);
	vec3 Normalize(const vec3& a);
	vec3 CrossProduct(const vec3& a, const vec3& b);
	vec3 FindVector(const vec3& a, const vec3& b, bool reflected = false);
	vec3 FindOrthogonal(const vec3& a);
	vec3 RotateAroundZ(const vec3& a, const double& angle);

}
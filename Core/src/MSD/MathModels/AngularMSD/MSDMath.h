#pragma once

#include "Core.h"

#include "imgui.h"

#include "../glm/glm.hpp"

namespace MSD {
	constexpr float PI = 3.14159265358979f;

	// 2D VECTOR DATATYPE /////////////////
	struct vec2
	{
		float x, y;

		vec2(float x = 0, float y = 0);

		vec2& operator=(const vec2& a);
		vec2 operator*(const float& a);
		vec2 operator+(const float& a) const;
		vec2 operator+(const vec2& a) const;
		vec2 operator-(const vec2& a) const;
		vec2 operator-() const;

		operator glm::vec2() const { return { x,y }; }
	};

	// 3D VECTOR DATATYPE /////////////////
	struct vec3
	{
		float x, y, z;
		
		vec3(float x = 0, float y = 0, float z = 0);

		vec3& operator=(const vec3& a);
		vec3& operator=(const glm::vec3& a);
		vec3 operator*(const float& a);
		vec3 operator+(const float& a) const;
		vec3 operator+(const vec3& a) const;
		vec3 operator-(const vec3& a) const;
		vec3 operator-() const;

		operator glm::vec3() const { return { x,y,z }; }
	};

	// MATH FUNCTIONS //////////////////// 
	float Magnitude(const vec3& a);
	float DotProduct(const vec3& a, const vec3& b);
	float Angle(vec3 a, vec3 b);
	float Angle2D(const vec2& a, const vec2& b);
	float AngleProjectionXY(vec3 a, vec3 b);
	float AngleProjectionYZ(vec3 a, vec3 b);
	float AngleProjectionXZ(vec3 a, vec3 b);
	float Approx(const float& radius, std::map<float, float> map);
	vec3 Normalize(const vec3& a);
	vec3 CrossProduct(const vec3& a, const vec3& b);
	vec3 FindVector(const vec3& a, const vec3& b, bool reflected = false);
	vec3 FindOrthogonal(const vec3& a);
	vec3 RotateAroundZ(const vec3& a, const float& angle);

}
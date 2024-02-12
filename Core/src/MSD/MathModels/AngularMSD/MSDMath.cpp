#include "msdpch.h"

#include "MSDMath.h"

namespace MSD {

	// VEC 2D //

	vec2::vec2(float x, float y)
		: x(x), y(y)
	{
	}

	vec2& vec2::operator=(const vec2& a)
	{
		x = a.x;
		y = a.y;

		return *this;
	}

	vec2 vec2::operator*(const float& a)
	{
		return vec2(x * a, y * a);
	}

	vec2 vec2::operator+(const float& a) const
	{
		return vec2(x + a, y + a);
	}

	vec2 vec2::operator+(const vec2& a) const
	{
		return vec2(x + a.x, y + a.y);
	}
	vec2 vec2::operator-(const vec2& a) const
	{
		return vec2(x - a.x, y - a.y);
	}
	vec2 vec2::operator-() const
	{
		return vec2(-x, -y);
	}

	// VEC 3D //

	vec3::vec3(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}

	vec3& vec3::operator=(const vec3& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	vec3& vec3::operator=(const glm::vec3& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	vec3 vec3::operator*(const float& a)
	{
		return vec3(x * a, y * a, z * a);
	}

	vec3 vec3::operator+(const float& a) const
	{
		return vec3(x + a, y + a, z + a);
	}

	vec3 vec3::operator+(const vec3& a) const
	{
		return vec3(x + a.x, y + a.y, z + a.z);
	}
	vec3 vec3::operator-(const vec3& a) const
	{
		return vec3(x - a.x, y - a.y - z - a.z);
	}
	vec3 vec3::operator-() const
	{
		return vec3(-x, -y, -z);
	}

	bool vec3::operator==(const vec3& a) const
	{
		if (x == a.x && y == a.y && z == a.z) return true;
		return false;
	}

	float Magnitude(const MSD::vec3& a)
	{
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	MSD::vec3 Normalize(const MSD::vec3& a)
	{
		MSD::vec3 normalized;
		float magnitude = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		normalized.x = a.x / magnitude;
		normalized.y = a.y / magnitude;
		normalized.z = a.z / magnitude;
		return normalized;
	}

	float DotProduct(const MSD::vec3& a, const MSD::vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	MSD::vec3 CrossProduct(const MSD::vec3& a, const MSD::vec3& b)
	{
		// A x B =  |A|*|B|*sinTheta
		// A x B = C;
		MSD::vec3 c;
		c.x = a.y * b.z - a.z * b.y;
		c.y = a.z * b.x - a.x * b.z;
		c.z = a.x * b.y - a.y * b.x;
		return c;
	}

	float Angle(MSD::vec3 a, MSD::vec3 b)
	{
		float cosTheta = MSD::DotProduct(a, b) / (MSD::Magnitude(a) * MSD::Magnitude(b));
		if (acos(cosTheta) != acos(cosTheta)) return 0;
		return acos(cosTheta);
	}

	float Angle2D(const vec2& a, const vec2& b)
	{
		float dotProduct = a.x * b.x + a.y * b.y;
		float cosTheta = dotProduct / (sqrt(a.x * a.x + a.y * a.y) * sqrt(b.x * b.x + b.y * b.y));
		if (acos(cosTheta) != acos(cosTheta)) return 0;
		return acos(cosTheta);
	}

	float AngleProjectionXY(vec3 a, vec3 b)
	{
		auto A = vec2(a.x, a.y);
		auto B = vec2(b.x, b.y);
		return Angle2D(A, B);
	}

	float AngleProjectionYZ(vec3 a, vec3 b)
	{
		auto A = vec2(a.y, a.z);
		auto B = vec2(b.y, b.z);
		return Angle2D(A, B);
	}

	float AngleProjectionXZ(vec3 a, vec3 b)
	{
		auto A = vec2(a.x, a.z);
		auto B = vec2(b.x, b.z);
		return Angle2D(A, B);
	}

	MSD::vec3 FindVector(const MSD::vec3& a, const MSD::vec3& b, bool reflected)
	{
		MSD::vec3 c;
		if (reflected == true)
		{
			c.x = a.x - b.x;
			c.y = a.y - b.y;
			c.z = a.z - b.z;
		}
		else
		{
			c.x = b.x - a.x;
			c.y = b.y - a.y;
			c.z = b.z - a.z;
		}
		return c;
	}

	MSD::vec3 FindOrthogonal(const MSD::vec3& a)
	{
		int vec3zeros = (int(a.x == 0) + int(a.y == 0) + int(a.z == 0));
		if (vec3zeros == 3)
		{
			std::cout << "ОШИБКА: В качестве нормали мишени задан нулевой вектор." << std::endl;
			return MSD::vec3();
		};
		if (vec3zeros > 1)
		{
			if (a.x != 0) { return MSD::vec3(0, 0, 1); };
			if (a.y != 0) { return MSD::vec3(0, 0, 1); };
			if (a.z != 0) { return MSD::vec3(0, 1, 0); };
		};
		if (a.x == 0) { return MSD::vec3(1, 0, 0); };
		if (a.y == 0) { return MSD::vec3(0, 1, 0); };
		if (a.z == 0) { return MSD::vec3(0, 0, 1); };
		return MSD::Normalize(MSD::vec3(0, 1, -a.z / a.y));
	}

	MSD::vec3 RotateAroundZ(const MSD::vec3& a, const float& angle)
	{
		MSD::vec3 b;
		b.x = cos(angle) * a.x - sin(angle) * a.y;
		b.y = sin(angle) * a.x + cos(angle) * a.y;
		b.z = a.z;
		return b;
	}

	float Approx(const float& radius, std::map<float, float> map)
	{
		if (map.begin() == map.end())
		{
			return 0.0f;
		}

		std::map<float, float>::iterator iterUpper, iterLower;
		float res;
		iterUpper = map.upper_bound(radius);
		if (iterUpper == map.end())
		{
			--iterUpper;
		}
		else if (iterUpper == map.begin())
		{
			++iterUpper;
		};
		iterLower = iterUpper;
		--iterLower;
		res = iterLower->second + (iterUpper->second - iterLower->second) / (iterUpper->first - iterLower->first) * (radius - iterLower->first);
		if (res < 0) return 0;
		return res;
	}

}
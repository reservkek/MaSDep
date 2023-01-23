#include "MSDMath.h"

namespace MSD {

	vec3::vec3(double x, double y, double z)
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

	vec3 vec3::operator*(const double& a)
	{
		return vec3(x * a, y * a, z * a);
	}

	vec3 vec3::operator+(const double& a) const
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

	double Magnitude(const MSD::vec3& a)
	{
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	MSD::vec3 Normalize(const MSD::vec3& a)
	{
		MSD::vec3 normalized;
		double magnitude = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		normalized.x = a.x / magnitude;
		normalized.y = a.y / magnitude;
		normalized.z = a.z / magnitude;
		return normalized;
	}

	double DotProduct(const MSD::vec3& a, const MSD::vec3& b)
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

	double Angle(const MSD::vec3& a, const MSD::vec3& b)
	{
		double cosTheta = MSD::DotProduct(a, b) / (MSD::Magnitude(a) * MSD::Magnitude(b));
		return acos(cosTheta);
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

	MSD::vec3 RotateAroundZ(const MSD::vec3& a, const double& angle)
	{
		MSD::vec3 b;
		b.x = cos(angle) * a.x - sin(angle) * a.y;
		b.y = sin(angle) * a.x + cos(angle) * a.y;
		b.z = a.z;
		return b;
	}

	double Approx(const double& value, std::map<double, double> map)
	{
		if (map.begin() == map.end())
		{
			return 100.0;
		}

		std::map<double, double>::iterator iterUpper, iterLower;
		double res;
		iterUpper = map.upper_bound(value);
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
		res = iterLower->second + (iterUpper->second - iterLower->second) / (iterUpper->first - iterLower->first) * (value - iterLower->first);
		if (res < 0) res = 0;
		return res;
	}

}
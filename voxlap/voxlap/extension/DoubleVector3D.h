#pragma once

#include "..\voxlap.h"

namespace voxlap { namespace extension {

#pragma pack(push, 1)

	struct DoubleVector3D : voxlap::dpoint3d
	{
	public:
		DoubleVector3D();
		DoubleVector3D(double);
		DoubleVector3D(double, double, double);
		DoubleVector3D(const voxlap::dpoint3d&);

		double length();
		double lengthsquared();
		DoubleVector3D normalized();
		DoubleVector3D absolute();

		DoubleVector3D operator+(const voxlap::dpoint3d&);
		DoubleVector3D operator-(const voxlap::dpoint3d&);
		DoubleVector3D operator*(const voxlap::dpoint3d&);
		DoubleVector3D operator/(const voxlap::dpoint3d&);
		DoubleVector3D operator+(const voxlap::lpoint3d&);
		DoubleVector3D operator-(const voxlap::lpoint3d&);
		DoubleVector3D operator*(const voxlap::lpoint3d&);
		DoubleVector3D operator/(const voxlap::lpoint3d&);
		DoubleVector3D operator+(const voxlap::point3d&);
		DoubleVector3D operator-(const voxlap::point3d&);
		DoubleVector3D operator*(const voxlap::point3d&);
		DoubleVector3D operator/(const voxlap::point3d&);

		DoubleVector3D operator*(double);
		DoubleVector3D operator/(double);

		void operator+=(const voxlap::dpoint3d&);
		void operator-=(const voxlap::dpoint3d&);
		void operator*=(const voxlap::dpoint3d&);
		void operator/=(const voxlap::dpoint3d&);
		void operator+=(const voxlap::lpoint3d&);
		void operator-=(const voxlap::lpoint3d&);
		void operator*=(const voxlap::lpoint3d&);
		void operator/=(const voxlap::lpoint3d&);
		void operator+=(const voxlap::point3d&);
		void operator-=(const voxlap::point3d&);
		void operator*=(const voxlap::point3d&);
		void operator/=(const voxlap::point3d&);

		bool operator==(const voxlap::dpoint3d&);
		bool operator==(const voxlap::point3d&);
		bool operator==(const voxlap::lpoint3d&);

		bool operator!=(const voxlap::dpoint3d&);
		bool operator!=(const voxlap::point3d&);
		bool operator!=(const voxlap::lpoint3d&);

		void operator*=(double);
		void operator/=(double);

		DoubleVector3D operator-();
	};

#pragma pack(pop)

	inline DoubleVector3D DoubleVector3D::operator+(const voxlap::dpoint3d& other)
	{
		return DoubleVector3D(x + other.x, y + other.y, z + other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator-(const voxlap::dpoint3d& other)
	{
		return DoubleVector3D(x - other.x, y - other.y, z - other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator*(const voxlap::dpoint3d& other)
	{
		return DoubleVector3D(x * other.x, y * other.y, z * other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator/(const voxlap::dpoint3d& other)
	{
		return DoubleVector3D(x / other.x, y / other.y, z / other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator+(const voxlap::lpoint3d& other)
	{
		return DoubleVector3D(x + other.x, y + other.y, z + other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator-(const voxlap::lpoint3d& other)
	{
		return DoubleVector3D(x - other.x, y - other.y, z - other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator*(const voxlap::lpoint3d& other)
	{
		return DoubleVector3D(x * other.x, y * other.y, z * other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator/(const voxlap::lpoint3d& other)
	{
		return DoubleVector3D(x / other.x, y / other.y, z / other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator+(const voxlap::point3d& other)
	{
		return DoubleVector3D(x + other.x, y + other.y, z + other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator-(const voxlap::point3d& other)
	{
		return DoubleVector3D(x - other.x, y - other.y, z - other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator*(const voxlap::point3d& other)
	{
		return DoubleVector3D(x * other.x, y * other.y, z * other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator/(const voxlap::point3d& other)
	{
		return DoubleVector3D(x / other.x, y / other.y, z / other.z);
	}

	inline DoubleVector3D DoubleVector3D::operator*(double mul)
	{
		return DoubleVector3D(x * mul, y * mul, z * mul);
	}

	inline DoubleVector3D DoubleVector3D::operator/(double div)
	{
		return DoubleVector3D(x / div, y / div, z / div);
	}

	inline void DoubleVector3D::operator+=(const voxlap::dpoint3d& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	inline void DoubleVector3D::operator-=(const voxlap::dpoint3d& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	inline void DoubleVector3D::operator*=(const voxlap::dpoint3d& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}

	inline void DoubleVector3D::operator/=(const voxlap::dpoint3d& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
	}

	inline void DoubleVector3D::operator+=(const voxlap::lpoint3d& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	inline void DoubleVector3D::operator-=(const voxlap::lpoint3d& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	inline void DoubleVector3D::operator*=(const voxlap::lpoint3d& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}

	inline void DoubleVector3D::operator/=(const voxlap::lpoint3d& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
	}

	inline void DoubleVector3D::operator+=(const voxlap::point3d& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	inline void DoubleVector3D::operator-=(const voxlap::point3d& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	inline void DoubleVector3D::operator*=(const voxlap::point3d& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}

	inline void DoubleVector3D::operator/=(const voxlap::point3d& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
	}

	inline bool DoubleVector3D::operator==(const voxlap::dpoint3d& other)
	{
		return (x == other.x &&
				y == other.y &&
				z == other.z);
	}

	inline bool DoubleVector3D::operator==(const voxlap::point3d& other)
	{
		return ((float)x == other.x &&
				(float)y == other.y &&
				(float)z == other.z);
	}

	inline bool DoubleVector3D::operator==(const voxlap::lpoint3d& other)
	{
		return ((long)x == other.x &&
				(long)y == other.y &&
				(long)z == other.z);
	}

	inline bool DoubleVector3D::operator!=(const voxlap::dpoint3d& other)
	{
		return !(this->operator==(other));
	}

	inline bool DoubleVector3D::operator!=(const voxlap::point3d& other)
	{
		return !(this->operator==(other));
	}

	inline bool DoubleVector3D::operator!=(const voxlap::lpoint3d& other)
	{
		return !(this->operator==(other));
	}

	inline void DoubleVector3D::operator*=(double mul)
	{
		x *= mul;
		y *= mul;
		z *= mul;
	}

	inline void DoubleVector3D::operator/=(double div)
	{
		x /= div;
		y /= div;
		z /= div;
	}

	inline DoubleVector3D DoubleVector3D::operator-()
	{
		return DoubleVector3D(-x, -y, -z);
	}

} }
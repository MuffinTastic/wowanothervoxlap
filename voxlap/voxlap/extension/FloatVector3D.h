#pragma once

#include "..\voxlap.h"

namespace voxlap { namespace extension {

#pragma pack(push, 1)

		struct FloatVector3D : voxlap::point3d
		{
		public:
			FloatVector3D();
			FloatVector3D(float);
			FloatVector3D(float, float, float);
			FloatVector3D(const voxlap::point3d&);

			float length();
			float lengthsquared();
			FloatVector3D normalized();
			FloatVector3D absolute();

			FloatVector3D operator+(const voxlap::dpoint3d&);
			FloatVector3D operator-(const voxlap::dpoint3d&);
			FloatVector3D operator*(const voxlap::dpoint3d&);
			FloatVector3D operator/(const voxlap::dpoint3d&);
			FloatVector3D operator+(const voxlap::lpoint3d&);
			FloatVector3D operator-(const voxlap::lpoint3d&);
			FloatVector3D operator*(const voxlap::lpoint3d&);
			FloatVector3D operator/(const voxlap::lpoint3d&);
			FloatVector3D operator+(const voxlap::point3d&);
			FloatVector3D operator-(const voxlap::point3d&);
			FloatVector3D operator*(const voxlap::point3d&);
			FloatVector3D operator/(const voxlap::point3d&);

			FloatVector3D operator*(double);
			FloatVector3D operator/(double);

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

			void operator*=(double);
			void operator/=(double);

			bool operator==(const voxlap::dpoint3d&);
			bool operator==(const voxlap::lpoint3d&);
			bool operator==(const voxlap::point3d&);

			bool operator!=(const voxlap::dpoint3d&);
			bool operator!=(const voxlap::lpoint3d&);
			bool operator!=(const voxlap::point3d&);

			FloatVector3D operator-();
		};

#pragma pack(pop)

		inline FloatVector3D FloatVector3D::operator+(const voxlap::dpoint3d& other)
		{
			return FloatVector3D(x + (float)other.x, y + (float)other.y, z + (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator-(const voxlap::dpoint3d& other)
		{
			return FloatVector3D(x - (float)other.x, y - (float)other.y, z - (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator*(const voxlap::dpoint3d& other)
		{
			return FloatVector3D(x * (float)other.x, y * (float)other.y, z * (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator/(const voxlap::dpoint3d& other)
		{
			return FloatVector3D(x / (float)other.x, y / (float)other.y, z / (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator+(const voxlap::lpoint3d& other)
		{
			return FloatVector3D(x + (float)other.x, y + (float)other.y, z + (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator-(const voxlap::lpoint3d& other)
		{
			return FloatVector3D(x - (float)other.x, y - (float)other.y, z - (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator*(const voxlap::lpoint3d& other)
		{
			return FloatVector3D(x * (float)other.x, y * (float)other.y, z * (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator/(const voxlap::lpoint3d& other)
		{
			return FloatVector3D(x / (float)other.x, y / (float)other.y, z / (float)other.z);
		}

		inline FloatVector3D FloatVector3D::operator+(const voxlap::point3d& other)
		{
			return FloatVector3D(x + other.x, y + other.y, z + other.z);
		}

		inline FloatVector3D FloatVector3D::operator-(const voxlap::point3d& other)
		{
			return FloatVector3D(x - other.x, y - other.y, z - other.z);
		}

		inline FloatVector3D FloatVector3D::operator*(const voxlap::point3d& other)
		{
			return FloatVector3D(x * other.x, y * other.y, z * other.z);
		}

		inline FloatVector3D FloatVector3D::operator/(const voxlap::point3d& other)
		{
			return FloatVector3D(x / other.x, y / other.y, z / other.z);
		}

		inline FloatVector3D FloatVector3D::operator*(double mul)
		{
			return FloatVector3D(x * (float)mul, y * (float)mul, z * (float)mul);
		}

		inline FloatVector3D FloatVector3D::operator/(double div)
		{
			return FloatVector3D(x / (float)div, y / (float)div, z / (float)div);
		}

		inline void FloatVector3D::operator+=(const voxlap::dpoint3d& other)
		{
			x += (float)other.x;
			y += (float)other.y;
			z += (float)other.z;
		}

		inline void FloatVector3D::operator-=(const voxlap::dpoint3d& other)
		{
			x -= (float)other.x;
			y -= (float)other.y;
			z -= (float)other.z;
		}

		inline void FloatVector3D::operator*=(const voxlap::dpoint3d& other)
		{
			x *= (float)other.x;
			y *= (float)other.y;
			z *= (float)other.z;
		}

		inline void FloatVector3D::operator/=(const voxlap::dpoint3d& other)
		{
			x /= (float)other.x;
			y /= (float)other.y;
			z /= (float)other.z;
		}

		inline void FloatVector3D::operator+=(const voxlap::lpoint3d& other)
		{
			x += (float)other.x;
			y += (float)other.y;
			z += (float)other.z;
		}

		inline void FloatVector3D::operator-=(const voxlap::lpoint3d& other)
		{
			x -= (float)other.x;
			y -= (float)other.y;
			z -= (float)other.z;
		}

		inline void FloatVector3D::operator*=(const voxlap::lpoint3d& other)
		{
			x *= (float)other.x;
			y *= (float)other.y;
			z *= (float)other.z;
		}

		inline void FloatVector3D::operator/=(const voxlap::lpoint3d& other)
		{
			x /= (float)other.x;
			y /= (float)other.y;
			z /= (float)other.z;
		}

		inline void FloatVector3D::operator+=(const voxlap::point3d& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		inline void FloatVector3D::operator-=(const voxlap::point3d& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		inline void FloatVector3D::operator*=(const voxlap::point3d& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		inline void FloatVector3D::operator/=(const voxlap::point3d& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		inline bool FloatVector3D::operator==(const voxlap::dpoint3d& other)
		{
			return (x == (float)other.x &&
					y == (float)other.y &&
					z == (float)other.z);
		}

		inline bool FloatVector3D::operator==(const voxlap::point3d& other)
		{
			return (x == other.x &&
					y == other.y &&
					z == other.z);
		}

		inline bool FloatVector3D::operator==(const voxlap::lpoint3d& other)
		{
			return ((long)x == other.x &&
					(long)y == other.y &&
					(long)z == other.z);
		}

		inline bool FloatVector3D::operator!=(const voxlap::dpoint3d& other)
		{
			return !(this->operator==(other));
		}

		inline bool FloatVector3D::operator!=(const voxlap::point3d& other)
		{
			return !(this->operator==(other));
		}

		inline bool FloatVector3D::operator!=(const voxlap::lpoint3d& other)
		{
			return !(this->operator==(other));
		}

		inline void FloatVector3D::operator*=(double mul)
		{
			x *= (float)mul;
			y *= (float)mul;
			z *= (float)mul;
		}

		inline void FloatVector3D::operator/=(double div)
		{
			x /= (float)div;
			y /= (float)div;
			z /= (float)div;
		}

		inline FloatVector3D FloatVector3D::operator-()
		{
			return FloatVector3D(-x, -y, -z);
		}

} }
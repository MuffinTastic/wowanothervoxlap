#pragma once


#include "..\voxlap.h"

namespace voxlap { namespace extension {

#pragma pack(push, 1)

		struct LongVector3D : voxlap::lpoint3d
		{
		public:
			LongVector3D();
			LongVector3D(long);
			LongVector3D(long, long, long);
			LongVector3D(const voxlap::lpoint3d&);

			float length();
			float lengthsquared();
			LongVector3D normalized();
			LongVector3D absolute();

			LongVector3D operator+(const voxlap::dpoint3d&);
			LongVector3D operator-(const voxlap::dpoint3d&);
			LongVector3D operator*(const voxlap::dpoint3d&);
			LongVector3D operator/(const voxlap::dpoint3d&);
			LongVector3D operator+(const voxlap::lpoint3d&);
			LongVector3D operator-(const voxlap::lpoint3d&);
			LongVector3D operator*(const voxlap::lpoint3d&);
			LongVector3D operator/(const voxlap::lpoint3d&);
			LongVector3D operator+(const voxlap::point3d&);
			LongVector3D operator-(const voxlap::point3d&);
			LongVector3D operator*(const voxlap::point3d&);
			LongVector3D operator/(const voxlap::point3d&);

			LongVector3D operator*(double);
			LongVector3D operator/(double);

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

			LongVector3D operator-();
		};

#pragma pack(pop)

		inline LongVector3D LongVector3D::operator+(const voxlap::dpoint3d& other)
		{
			return LongVector3D(x + other.x, y + other.y, z + other.z);
		}

		inline LongVector3D LongVector3D::operator-(const voxlap::dpoint3d& other)
		{
			return LongVector3D(x - other.x, y - other.y, z - other.z);
		}

		inline LongVector3D LongVector3D::operator*(const voxlap::dpoint3d& other)
		{
			return LongVector3D(x * other.x, y * other.y, z * other.z);
		}

		inline LongVector3D LongVector3D::operator/(const voxlap::dpoint3d& other)
		{
			return LongVector3D(x / other.x, y / other.y, z / other.z);
		}

		inline LongVector3D LongVector3D::operator+(const voxlap::lpoint3d& other)
		{
			return LongVector3D(x + other.x, y + other.y, z + other.z);
		}

		inline LongVector3D LongVector3D::operator-(const voxlap::lpoint3d& other)
		{
			return LongVector3D(x - other.x, y - other.y, z - other.z);
		}

		inline LongVector3D LongVector3D::operator*(const voxlap::lpoint3d& other)
		{
			return LongVector3D(x * other.x, y * other.y, z * other.z);
		}

		inline LongVector3D LongVector3D::operator/(const voxlap::lpoint3d& other)
		{
			return LongVector3D(x / other.x, y / other.y, z / other.z);
		}

		inline LongVector3D LongVector3D::operator+(const voxlap::point3d& other)
		{
			return LongVector3D(x + other.x, y + other.y, z + other.z);
		}

		inline LongVector3D LongVector3D::operator-(const voxlap::point3d& other)
		{
			return LongVector3D(x - other.x, y - other.y, z - other.z);
		}

		inline LongVector3D LongVector3D::operator*(const voxlap::point3d& other)
		{
			return LongVector3D(x * other.x, y * other.y, z * other.z);
		}

		inline LongVector3D LongVector3D::operator/(const voxlap::point3d& other)
		{
			return LongVector3D(x / other.x, y / other.y, z / other.z);
		}

		inline LongVector3D LongVector3D::operator*(double mul)
		{
			return LongVector3D(x * mul, y * mul, z * mul);
		}

		inline LongVector3D LongVector3D::operator/(double div)
		{
			return LongVector3D(x / div, y / div, z / div);
		}

		inline void LongVector3D::operator+=(const voxlap::dpoint3d& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		inline void LongVector3D::operator-=(const voxlap::dpoint3d& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		inline void LongVector3D::operator*=(const voxlap::dpoint3d& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		inline void LongVector3D::operator/=(const voxlap::dpoint3d& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		inline void LongVector3D::operator+=(const voxlap::lpoint3d& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		inline void LongVector3D::operator-=(const voxlap::lpoint3d& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		inline void LongVector3D::operator*=(const voxlap::lpoint3d& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		inline void LongVector3D::operator/=(const voxlap::lpoint3d& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		inline void LongVector3D::operator+=(const voxlap::point3d& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		inline void LongVector3D::operator-=(const voxlap::point3d& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		inline void LongVector3D::operator*=(const voxlap::point3d& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
		}

		inline void LongVector3D::operator/=(const voxlap::point3d& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		inline bool LongVector3D::operator==(const voxlap::dpoint3d& other)
		{
			return (x == (long)other.x &&
					y == (long)other.y &&
					z == (long)other.z);
		}

		inline bool LongVector3D::operator==(const voxlap::point3d& other)
		{
			return (x == (long)other.x &&
					y == (long)other.y &&
					z == (long)other.z);
		}

		inline bool LongVector3D::operator==(const voxlap::lpoint3d& other)
		{
			return (x == other.x &&
					y == other.y &&
					z == other.z);
		}

		inline bool LongVector3D::operator!=(const voxlap::dpoint3d& other)
		{
			return !(this->operator==(other));
		}

		inline bool LongVector3D::operator!=(const voxlap::point3d& other)
		{
			return !(this->operator==(other));
		}

		inline bool LongVector3D::operator!=(const voxlap::lpoint3d& other)
		{
			return !(this->operator==(other));
		}

		inline void LongVector3D::operator*=(double mul)
		{
			x *= mul;
			y *= mul;
			z *= mul;
		}

		inline void LongVector3D::operator/=(double div)
		{
			x /= div;
			y /= div;
			z /= div;
		}

		inline LongVector3D LongVector3D::operator-()
		{
			return LongVector3D(-x, -y, -z);
		}

	}
}
#include "LongVector3D.h"

#include <math.h>

namespace voxlap {
	namespace extension {

		LongVector3D::LongVector3D()
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}

		LongVector3D::LongVector3D(long i)
		{
			x = i;
			y = i;
			z = i;
		}

		LongVector3D::LongVector3D(long nx, long ny, long nz)
		{
			x = nx;
			y = ny;
			z = nz;
		}

		LongVector3D::LongVector3D(const voxlap::lpoint3d& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		float LongVector3D::length()
		{
			return sqrtf((x * x) + (y * y) + (z * z));
		}

		float LongVector3D::lengthsquared()
		{
			return (x * x) + (y * y) + (z * z);
		}

		LongVector3D LongVector3D::normalized()
		{
			return (*this / length());
		}

		LongVector3D LongVector3D::absolute()
		{
			return LongVector3D{ abs(x), abs(y), abs(z) };
		}
	}
}
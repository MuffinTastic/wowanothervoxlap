#include "FloatVector3D.h"

#include <math.h>

namespace voxlap {
	namespace extension {

		FloatVector3D::FloatVector3D()
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}

		FloatVector3D::FloatVector3D(float i)
		{
			x = i;
			y = i;
			z = i;
		}

		FloatVector3D::FloatVector3D(float nx, float ny, float nz)
		{
			x = nx;
			y = ny;
			z = nz;
		}

		FloatVector3D::FloatVector3D(const voxlap::point3d& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		float FloatVector3D::length()
		{
			return sqrtf((x * x) + (y * y) + (z * z));
		}

		float FloatVector3D::lengthsquared()
		{
			return (x * x) + (y * y) + (z * z);
		}

		FloatVector3D FloatVector3D::normalized()
		{
			return (*this / length());
		}

		FloatVector3D FloatVector3D::absolute()
		{
			return FloatVector3D{ abs(x), abs(y), abs(z) };
		}

	}
}
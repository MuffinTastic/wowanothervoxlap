#include "DoubleVector3D.h"

#include <math.h>

namespace voxlap { namespace extension {

	DoubleVector3D::DoubleVector3D()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	DoubleVector3D::DoubleVector3D(double i)
	{
		x = i;
		y = i;
		z = i;
	}

	DoubleVector3D::DoubleVector3D(double nx, double ny, double nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}

	DoubleVector3D::DoubleVector3D(const dpoint3d& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	double DoubleVector3D::length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double DoubleVector3D::lengthsquared()
	{
		return (x * x) + (y * y) + (z * z);
	}

	DoubleVector3D DoubleVector3D::normalized()
	{
		return (*this / length());
	}

	DoubleVector3D DoubleVector3D::absolute()
	{
		return DoubleVector3D{ abs(x), abs(y), abs(z) };
	}

} }
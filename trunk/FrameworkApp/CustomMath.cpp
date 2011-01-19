#include "CustomMath.h"

Vector3 crossProduct(Vector3 v0, Vector3 v1)
{
	Vector3 result;

	result.m_x = v0.m_y * v1.m_z - v0.m_z * v1.m_y;
	result.m_y = v0.m_z * v1.m_x - v0.m_x * v1.m_z;
	result.m_z = v0.m_x * v1.m_y - v0.m_y * v1.m_x;

	return result;
}

void normalise(Vector3 &v)
{
	float len = length(v);
	v.m_x /= len;
	v.m_y /= len;
	v.m_z /= len;
}

float length(Vector3 v)
{
	float result = 0;
	
	result = sqrt(v.m_x*v.m_x + v.m_y*v.m_y + v.m_z*v.m_z);

	return result;
}

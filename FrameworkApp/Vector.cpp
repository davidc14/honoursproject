#include "Vector.h"

///////////////////////////////////VECTOR3////////////////////////////////

Vector3::Vector3()
{
	m_x = m_y = m_z = 0.0f;
}

Vector3::Vector3(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3::~Vector3()
{
}

/////////////////////////////////////VECTOR2//////////////////////////////

Vector2::Vector2()
{
	m_x = m_y = 0.0f;
}

Vector2::Vector2(float x, float y)
{
	m_x = x;
	m_y = y;
}

Vector2::~Vector2()
{
}


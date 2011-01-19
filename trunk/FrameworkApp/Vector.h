#ifndef _VECTOR_H
#define _VECTOR_H

class Vector3
{
public:
	Vector3();
	Vector3(float, float, float);
	~Vector3();

	float m_x, m_y, m_z;
};

class Vector2
{
public:
	Vector2();
	Vector2(float, float);
	~Vector2();

	float m_x, m_y;
};


#endif
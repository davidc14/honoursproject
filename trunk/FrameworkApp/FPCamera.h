#ifndef _FPCamera_H
#define _FPCamera_H

#include "D3D9Window.h"
#include "CustomMath.h"

//Pi
static const float Pi = 3.1415f;

class FPCamera
{
public:
	//Default constructor
	FPCamera();
	//Defining constructor
	FPCamera(D3DXVECTOR3 position, D3DXVECTOR3 lookAt, D3DXVECTOR3 up);
	~FPCamera();

	//Release the FPCamera from memory
	void release();

	//The update function
	void Update(const float dtSeconds);
	void Update(const float dtSeconds, int windowWidth, int windowHeight);

	//Returns the value of the vectors
	Vector3 getPosition();
	Vector3 getLookAt();
	Vector3 getUp();
	Vector3 getForward();

	D3DXVECTOR3* getPositionDX();
	D3DXVECTOR3* getLookAtDX();
	D3DXVECTOR3* getUpDX();
	D3DXVECTOR3* getForwardDX();

	//The move and strafe functions for the FPCamera
	void Move(D3DXVECTOR3 acceleration);
	void Strafe(D3DXVECTOR3 acceleration);
	void Move(float xAcceleration, float yAcceleration, float zAcceleration);
	void Strafe(float xAcceleration, float yAcceleration, float zAcceleration);

	//The mouse movement functions
	void mouseMove(Mouse *MousePos, POINT mousePos, int window_width, int window_height);
	void mouseMove();
	//The angle test collision detection test
	bool angleTest(Mouse* MousePos);

	//The mouse zoom function
	void mouseZoom(int windowWidth, int windowHeight);

	//The first frame get and set functions
	void First(bool value){ first = value; }
	bool First(){ return first; }

	//The first frame get and set functions
	//For the zoom function
	void ZoomFirst(bool value) { zoomFirst = value; }
	bool ZoomFirst() { return zoomFirst; }

	void WindowWidth (int value) { m_WindowWidth = value; }
	void WIndowHeight (int value) { m_WindowHeight = value; }

private:
	//The associated vectors of the FPCamera
	
	D3DXVECTOR3 m_Position, m_LookAt, m_Forward, m_Up, m_Right;

	void UpdateDXVectors();

	//The angles of the FPCamera
	float m_Pitch, m_Roll, m_Yaw;

	//variables created for
	//the calculations
	float cosRoll, cosPitch, cosYaw; 
	float sinRoll, sinPitch, sinYaw;

	//The zoom float store
	float m_Zoom;

	//Bools used for mouse movement so that 
	//the FPCamera doesn't move whilst the mouse
	//is set into the correct position
	bool first, zoomFirst;

	//The structs for the mouse
	Mouse *m_Mouse;
	POINT *m_mousePos;

	int m_WindowWidth, m_WindowHeight;
};

#endif

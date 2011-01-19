#ifndef _CAMERA_H
#define _CAMERA_H

#include "D3D9Window.h"
#include "CustomMath.h"

//Pi
static const float Pi = 3.1415f;

class Camera
{
public:
	//Default constructor
	Camera();
	//Defining constructor
	Camera(Vector3*, Vector3*, Vector3*);
	~Camera();

	//Release the camera from memory
	void release();

	//The update function
	void Update(const float dtSeconds);
	void Update(const float, int, int);

	//Returns the value of the position vector
	Vector3 getPosition();
	Vector3 getForward();

	//The move and strafe functions for the camera
	void Move(Vector3*);
	void Strafe(Vector3*);
	void Move(float, float, float);
	void Strafe(float, float, float);

	//The mouse movement functions
	void mouseMove(Mouse*, POINT, int, int);
	void mouseMove();
	//The angle test collision detection test
	bool angleTest(Mouse*);

	//The mouse zoom function
	void mouseZoom(int, int);

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
	//The associated vectors of the camera
	Vector3* m_Position,* m_LookAt,* m_Forward,* m_Up,* m_Right;

	//The angles of the camera
	float m_Pitch, m_Roll, m_Yaw;

	//variables created for
	//the calculations
	float cosRoll, cosPitch, cosYaw; 
	float sinRoll, sinPitch, sinYaw;

	//The zoom float store
	float m_Zoom;

	//Bools used for mouse movement so that 
	//the camera doesn't move whilst the mouse
	//is set into the correct position
	bool first, zoomFirst;

	//The structs for the mouse
	Mouse *m_Mouse;
	POINT *m_mousePos;

	int m_WindowWidth, m_WindowHeight;
};

#endif

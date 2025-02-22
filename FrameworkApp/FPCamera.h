#ifndef _FPCamera_H
#define _FPCamera_H

//#include "D3D9Window.h"
#include "CustomMath.h"

//Pi
static const float Pi = 3.1415f;

class FPCamera
{
public:
	//Default constructor
	FPCamera();
	//Defining constructor
	FPCamera(D3DXVECTOR3 position, D3DXVECTOR3 lookAt, D3DXVECTOR3 up, int WindowWidth, int WindowHeight);
	~FPCamera();

	//Release the FPCamera from memory
	void release();

	//The update function
	void Update(const float dtSeconds);
	void Update(const float dtSeconds, int windowWidth, int windowHeight);

	float GetFarPlane() { return mFarPlane; }

	////Returns the value of the vectors
	//Vector3 getPosition();
	//Vector3 getLookAt();
	//Vector3 getUp();
	//Vector3 getForward();

	D3DXVECTOR3* getPosition();
	D3DXVECTOR3* getLookAt();
	D3DXVECTOR3* getUp();
	D3DXVECTOR3* getForward();

	//The move and strafe functions for the FPCamera
	void Move(D3DXVECTOR3 acceleration);
	void Strafe(D3DXVECTOR3 acceleration);
	void Move(float xAcceleration, float yAcceleration, float zAcceleration);
	void Strafe(float xAcceleration, float yAcceleration, float zAcceleration);
	void Raise(float x, float y, float z);

	//The mouse movement functions
	void Rotate(Mouse *MousePos, POINT mousePos, int window_width, int window_height);
	void Rotate();
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

	bool GetActiveFlag() { return mActive; }
	void SetActiveFlag(bool activeFlag) { mActive = activeFlag; }

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

	float mFarPlane, mNearPlane;

	//The active flag that dictates whether the camera is in use
	bool mActive;
};

#endif

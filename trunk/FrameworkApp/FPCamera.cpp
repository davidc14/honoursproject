#include "FPCamera.h"


Camera::Camera()
{
	m_Pitch = m_Roll = m_Yaw = 0.0f;

	this->m_WindowWidth = 800;
	this->m_WindowHeight = 600;

	this->m_Mouse = new Mouse();
	this->m_mousePos = new POINT();

	this->m_Zoom = 45.0f;
	this->m_Position = new Vector3();
	this->m_LookAt = new Vector3();
	this->m_Up = new Vector3();
	this->m_Forward = new Vector3();
	this->m_Right = new Vector3();
	this->first = true;
	this->zoomFirst = true;
}

Camera::Camera(Vector3* position, Vector3* lookAt, Vector3* up)
{
	m_Pitch = m_Roll = m_Yaw = 0.0f;

	this->m_WindowWidth = 800;
	this->m_WindowHeight = 600;

	this->m_Mouse = new Mouse();
	this->m_mousePos = new POINT();

	this->m_Zoom = 45.0f;
	this->m_Position = position;
	this->m_LookAt = lookAt;
	this->m_Up = up;
	//this->m_Forward = m_LookAt;
	this->m_Forward = new Vector3();
	this->m_Right = new Vector3();
	*this->m_Right = crossProduct(*m_Forward, *m_Up);
	this->first = true;
	this->zoomFirst = true;
}

Camera::~Camera()
{
}

void Camera::Update(const float dtSeconds, int windowWidth, int windowHeight)
{
	if(GetAsyncKeyState(VK_SPACE))
		m_Zoom = 45.0f;
	
	////Make sure and do this before modifying gluPerspective
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	//glViewport(0,0, m_WindowWidth, m_WindowHeight);
	////calculate aspect ratio
	//gluPerspective(m_Zoom,(GLfloat)m_WindowWidth/(GLfloat)m_WindowHeight, 1 ,10000.0f);
	//
	////And make sure you set it back!
	//glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
	//glLoadIdentity();// Reset The Modelview Matrix

	//// Where we are, What we look at, and which way is up
	//gluLookAt(m_Position->m_x, m_Position->m_y, m_Position->m_z,
	//	m_LookAt->m_x, m_LookAt->m_y, m_LookAt->m_z,
	//	m_Up->m_x, m_Up->m_y, m_Up->m_z); 

	//Camera is the object (class) created to store the 
	//variables and vectors needed to manipulate
	//the gluLookAt() function
	cosYaw = cosf(m_Yaw*Pi/180);
	cosPitch = cosf(m_Pitch*Pi/180);
	cosRoll = cosf(m_Roll*Pi/180);
	sinYaw = sinf(m_Yaw*Pi/180);
	sinPitch = sinf(m_Pitch*Pi/180);
	sinRoll = sinf(m_Roll*Pi/180);
	
	//Updating the forward vector
	m_Forward->m_x = sinYaw * cosPitch*360;
	m_Forward->m_y = sinPitch*360;
	m_Forward->m_z = cosPitch * -cosYaw*360;

	//These update the look direction of the camera
	m_LookAt->m_x = m_Position->m_x + m_Forward->m_x;
	m_LookAt->m_y = m_Position->m_y + m_Forward->m_y;
	m_LookAt->m_z = m_Position->m_z + m_Forward->m_z;

	//Up Vector
	m_Up->m_x = -cosYaw * sinRoll - sinYaw * sinPitch * cosRoll;
	m_Up->m_y = cosPitch * cosRoll;
	m_Up->m_z = -sinYaw * sinRoll - sinPitch * cosRoll * -cosYaw;

	/*if(m_Pitch > 360) m_Pitch = 1;
	if(m_Pitch < 0) m_Pitch = 359;

	if(m_Yaw > 360) m_Yaw = 1;
	if(m_Yaw < 0) m_Yaw = 359;

	if(m_Roll > 360) m_Roll = 1;
	if(m_Roll < 0) m_Roll = 359;*/
}

void Camera::Update(const float dtSeconds)
{
	cosYaw = cosPitch = cosRoll = sinYaw = sinPitch = sinRoll = 0;
	// Where we are, What we look at, and which way is up
	/*gluLookAt(m_Position->m_x, m_Position->m_y, m_Position->m_z,
		m_LookAt->m_x, m_LookAt->m_y, m_LookAt->m_z,
		m_Up->m_x, m_Up->m_y, m_Up->m_z); */

	//Camera is the object (class) created to store the 
	//variables and vectors needed to manipulate
	//the gluLookAt() function
	cosYaw = cosf(m_Yaw*Pi/180);
	cosPitch = cosf(m_Pitch*Pi/180);
	cosRoll = cosf(m_Roll*Pi/180);
	sinYaw = sinf(m_Yaw*Pi/180);
	sinPitch = sinf(m_Pitch*Pi/180);
	sinRoll = sinf(m_Roll*Pi/180);
	
	//Updating the forward vector
	m_Forward->m_x = sinYaw * cosPitch*360;
	m_Forward->m_y = sinPitch*360;
	m_Forward->m_z = cosPitch * -cosYaw*360;

	//These update the look direction of the camera
	m_LookAt->m_x = m_Position->m_x + m_Forward->m_x;
	m_LookAt->m_y = m_Position->m_y + m_Forward->m_y;
	m_LookAt->m_z = m_Position->m_z + m_Forward->m_z;

	//Up Vector
	m_Up->m_x = -cosYaw * sinRoll - sinYaw * sinPitch * cosRoll;
	m_Up->m_y = cosPitch * cosRoll;
	m_Up->m_z = -sinYaw * sinRoll - sinPitch * cosRoll * -cosYaw;
}

void Camera::Move(Vector3* acceleration)
{
	//Adding the forward vector to the Position vector moves the camera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position->m_x += m_Forward->m_x * acceleration->m_x;
	m_Position->m_y += m_Forward->m_y * acceleration->m_y;
	m_Position->m_z += m_Forward->m_z * acceleration->m_z;
}
	
void Camera::Strafe(Vector3* acceleration)
{
	*m_Right = crossProduct(*m_Forward, *m_Up);

	//Adding the right vector to the Position vector moves the camera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position->m_x += m_Right->m_x * acceleration->m_x;
	m_Position->m_y += m_Right->m_y * acceleration->m_y;
	m_Position->m_z += m_Right->m_z * acceleration->m_z;
}

void Camera::Move(float x, float y, float z)
{
	//Adding the forward vector to the Position vector moves the camera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position->m_x += m_Forward->m_x * x;
	m_Position->m_y += m_Forward->m_y * y;
	m_Position->m_z += m_Forward->m_z * z;
}

void Camera::Strafe(float x, float y, float z)
{
	*m_Right = crossProduct(*m_Forward, *m_Up);

	//Adding the right vector to the Position vector moves the camera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position->m_x += m_Right->m_x * x;
	m_Position->m_y += m_Right->m_y * y;
	m_Position->m_z += m_Right->m_z * z;
}

void Camera::mouseMove(Mouse *MousePos, POINT mousePos, int window_width, int window_height)
{
	//Turning off the cursor
	//ShowCursor(false);
	//Get the cursor position 
	//These will be whatever SetCursorPos() has made them
	GetCursorPos(&mousePos);
	//Setting the cursor positions to the middle of the window
	//SetCursorPos(window_width/2,window_height/2);

	if(!first)
	{
		//Set the Mouse to the centre of the screen according to 
		//Set/GetCursorPos functions found in WinMain
		MousePos->x = mousePos.x;
		MousePos->y = mousePos.y;

		//Set the old to the the middle of the screen
		MousePos->old_x = window_width/2;
		MousePos->old_y = window_height/2;

		//Measure the distance between the two
		MousePos->drag_x = (MousePos->old_x - MousePos->x);
		MousePos->drag_y = (MousePos->old_y - MousePos->y);

		//Use the result to calculate the angle
		if(!angleTest(MousePos))	
		{
			//Else allow movement
			m_Pitch += (MousePos->drag_y*0.05f);	
		}
		m_Yaw -= (MousePos->drag_x*0.05f);
	}
	else
		first = false;
}

void Camera::mouseMove()
{
	//Turning off the cursor
	//ShowCursor(false);
	//Get the cursor position 
	//These will be whatever SetCursorPos() has made them
	GetCursorPos(m_mousePos);
	//Setting the cursor positions to the middle of the window
	SetCursorPos(this->m_WindowWidth/2,this->m_WindowHeight/2);

	if(!first)
	{
		//Set the Mouse to the centre of the screen according to 
		//Set/GetCursorPos functions found in WinMain
		m_Mouse->x = m_mousePos->x;
		m_Mouse->y = m_mousePos->y;

		//Set the old to the the middle of the screen
		m_Mouse->old_x = m_WindowWidth/2;
		m_Mouse->old_y = m_WindowHeight/2;

		//Measure the distance between the two
		m_Mouse->drag_x = (m_Mouse->old_x - m_Mouse->x);
		m_Mouse->drag_y = (m_Mouse->old_y - m_Mouse->y);

		//Use the result to calculate the angle
		if(!angleTest(m_Mouse))	
		{
			//Else allow movement
			m_Pitch += (m_Mouse->drag_y*0.05f);	
		}
		m_Yaw -= (m_Mouse->drag_x*0.05f);
	}
	else
		first = false;
}

void Camera::mouseZoom(int windowWidth, int windowHeight)
{
	const float topRange = 90.0f;
	const float bottomRange = 15.0f;
	bool inRange = true;

	//Turning off the cursor
	//ShowCursor(false);
	//Get the cursor position 
	//These will be whatever SetCursorPos() has made them
	GetCursorPos(m_mousePos);
	//Setting the cursor positions to the middle of the window
	SetCursorPos(m_WindowWidth/2,m_WindowHeight/2);

	if(!zoomFirst)
	{
		//Set the Mouse to the centre of the screen according to 
		//Set/GetCursorPos functions found in WinMain
		m_Mouse->x = m_mousePos->x;
		m_Mouse->y = m_mousePos->y;

		//Set the old to the the middle of the screen
		m_Mouse->old_x = m_WindowWidth/2;
		m_Mouse->old_y = m_WindowHeight/2;

		//Measure the distance between the two
		m_Mouse->drag_x = (m_Mouse->old_x - m_Mouse->x);
		m_Mouse->drag_y = (m_Mouse->old_y - m_Mouse->y);

		//Check if the zoom is within range
		if(m_Zoom < topRange && m_Zoom > bottomRange)
			inRange = true;
		else
			inRange = false;

		//If it is
		if(inRange)
			//Adjust the zoom depending on the drag
			m_Zoom -= m_Mouse->drag_y * 0.1f;
		else
			//If it is out of range
			if(m_Zoom > bottomRange)
				//Pop it back into range
				m_Zoom -= 1.0f;
			else
				m_Zoom += 1.0f;
	}
	else
		zoomFirst = false;
}

//The angle test to stop the camera doing barrel rolls
bool Camera::angleTest(Mouse *MousePos)
{
	if((m_Pitch > 90.0f || MousePos->drag_y < 0) && (m_Pitch < -90.0f || MousePos->drag_y > 0))
		return true;

	return false;
}

Vector3 Camera::getPosition()
{
	return *m_Position;
}

Vector3 Camera::getForward()
{
	return *m_Forward;
}

void Camera::release()
{
	delete m_Position, 
		m_Mouse, 
		m_mousePos, 
		m_Position, 
		m_LookAt, 
		m_Forward, 
		m_Up,
		m_Right;
}
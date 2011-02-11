#include "FPCamera.h"


FPCamera::FPCamera()
{
	farPlane = 30000.0f;
	m_Pitch = m_Roll = m_Yaw = 0.0f;

	m_WindowWidth = 800;
	m_WindowHeight = 600;

	m_Mouse = new Mouse();
	m_mousePos = new POINT();

	m_Zoom = 45.0f;
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_LookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Up = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Forward = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	first = true;
	zoomFirst = true;
}

FPCamera::FPCamera(D3DXVECTOR3 position, D3DXVECTOR3 lookAt, D3DXVECTOR3 up, int WindowWidth, int WindowHeight)
{
	farPlane = 1000.0f;
	m_Pitch = m_Roll = m_Yaw = 0.0f;

	m_WindowWidth = WindowWidth;
	m_WindowHeight = WindowHeight;

	m_Mouse = new Mouse();
	m_mousePos = new POINT();

	m_Zoom = 45.0f;
	m_Position = position;
	m_LookAt = lookAt;
	m_Up = up;
	//m_Forward = m_LookAt;
	m_Forward = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3Cross(&m_Right, &m_Forward, &m_Up);
	first = true;
	zoomFirst = true;
}

FPCamera::~FPCamera()
{
}

void FPCamera::Update(const float dtSeconds, int windowWidth, int windowHeight)
{
	/*if(GetAsyncKeyState(VK_SPACE))
		m_Zoom = 45.0f;*/
	
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
	//gluLookAt(m_Position.x, m_Position.y, m_Position.z,
	//	m_LookAt.x, m_LookAt.y, m_LookAt.z,
	//	m_Up.x, m_Up.y, m_Up.z); 

	//FPCamera is the object (class) created to store the 
	//variables and vectors needed to manipulate
	//the gluLookAt() function
	cosYaw = cosf(m_Yaw*Pi/180);
	cosPitch = cosf(m_Pitch*Pi/180);
	cosRoll = cosf(m_Roll*Pi/180);
	sinYaw = sinf(m_Yaw*Pi/180);
	sinPitch = sinf(m_Pitch*Pi/180);
	sinRoll = sinf(m_Roll*Pi/180);
	
	//Updating the forward vector
	m_Forward.x = sinYaw * cosPitch*360;
	m_Forward.y = sinPitch*360;
	m_Forward.z = cosPitch * -cosYaw*360;

	//These update the look direction of the FPCamera
	m_LookAt.x = m_Position.x + m_Forward.x;
	m_LookAt.y = m_Position.y + m_Forward.y;
	m_LookAt.z = m_Position.z + m_Forward.z;

	//Up Vector
	m_Up.x = -cosYaw * sinRoll - sinYaw * sinPitch * cosRoll;
	m_Up.y = cosPitch * cosRoll;
	m_Up.z = -sinYaw * sinRoll - sinPitch * cosRoll * -cosYaw;

	/*if(m_Pitch > 360) m_Pitch = 1;
	if(m_Pitch < 0) m_Pitch = 359;

	if(m_Yaw > 360) m_Yaw = 1;
	if(m_Yaw < 0) m_Yaw = 359;

	if(m_Roll > 360) m_Roll = 1;
	if(m_Roll < 0) m_Roll = 359;*/
}

void FPCamera::Update(const float dtSeconds)
{
	cosYaw = cosPitch = cosRoll = sinYaw = sinPitch = sinRoll = 0;
	// Where we are, What we look at, and which way is up
	/*gluLookAt(m_Position.x, m_Position.y, m_Position.z,
		m_LookAt.x, m_LookAt.y, m_LookAt.z,
		m_Up.x, m_Up.y, m_Up.z); */

	//FPCamera is the object (class) created to store the 
	//variables and vectors needed to manipulate
	//the gluLookAt() function
	/*cosYaw = cosf(m_Yaw*Pi/180);
	cosPitch = cosf(m_Pitch*Pi/180);
	cosRoll = cosf(m_Roll*Pi/180);
	sinYaw = sinf(m_Yaw*Pi/180);
	sinPitch = sinf(m_Pitch*Pi/180);
	sinRoll = sinf(m_Roll*Pi/180);*/

	cosYaw = cosf(m_Yaw*Pi/180);
	cosPitch = cosf(m_Pitch*Pi/180);
	cosRoll = cosf(m_Roll*Pi/180);
	sinYaw = sinf(m_Yaw*Pi/180);
	sinPitch = sinf(m_Pitch*Pi/180);
	sinRoll = sinf(m_Roll*Pi/180);
	
	//Updating the forward vector
	m_Forward.x = sinYaw * cosPitch*360;
	m_Forward.y = sinPitch*360;
	m_Forward.z = cosPitch * -cosYaw*360;

	//These update the look direction of the FPCamera
	m_LookAt.x = m_Position.x - m_Forward.x;
	m_LookAt.y = m_Position.y - m_Forward.y;
	m_LookAt.z = m_Position.z - m_Forward.z;

	//Up Vector
	m_Up.x = -cosYaw * sinRoll - sinYaw * sinPitch * cosRoll;
	m_Up.y = cosPitch * cosRoll;
	m_Up.z = -sinYaw * sinRoll - sinPitch * cosRoll * -cosYaw;
}

void FPCamera::Move(D3DXVECTOR3 acceleration)
{
	//Adding the forward vector to the Position vector moves the FPCamera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position.x -= m_Forward.x * acceleration.x;
	m_Position.y -= m_Forward.y * acceleration.y;
	m_Position.z -= m_Forward.z * acceleration.z;
}
	
void FPCamera::Strafe(D3DXVECTOR3 acceleration)
{
	D3DXVec3Cross(&m_Right, &m_Forward, &m_Up);

	//Adding the right vector to the Position vector moves the FPCamera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position.x -= m_Right.x * acceleration.x;
	m_Position.y -= m_Right.y * acceleration.y;
	m_Position.z -= m_Right.z * acceleration.z;
}

void FPCamera::Move(float x, float y, float z)
{
	//Adding the forward vector to the Position vector moves the FPCamera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position.x -= m_Forward.x * x;
	m_Position.y -= m_Forward.y * y;
	m_Position.z -= m_Forward.z * z;
}

void FPCamera::Strafe(float x, float y, float z)
{
	D3DXVec3Cross(&m_Right, &m_Forward, &m_Up);

	//Adding the right vector to the Position vector moves the FPCamera
	//The multipliers at the end of the calculation define speed
	//I.E. The larger the multiplier the faster it goes
	m_Position.x -= m_Right.x * x;
	m_Position.y -= m_Right.y * y;
	m_Position.z -= m_Right.z * z;
}

void FPCamera::mouseMove(Mouse *MousePos, POINT mousePos, int window_width, int window_height)
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
			m_Pitch -= (MousePos->drag_y*0.05f);	
		}
		m_Yaw += (MousePos->drag_x*0.05f);
	}
	else
		first = false;
}

void FPCamera::mouseMove()
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
		if(angleTest(m_Mouse))	
		{
			//Else allow movement
			m_Pitch -= (m_Mouse->drag_y*0.05f);	
		}
		m_Yaw += (m_Mouse->drag_x*0.05f);
	}
	else
		first = false;
}

void FPCamera::mouseZoom(int windowWidth, int windowHeight)
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

//The angle test to stop the FPCamera doing barrel rolls
bool FPCamera::angleTest(Mouse *MousePos)
{
	if((m_Pitch < 90.0f || MousePos->drag_y > 0) && (m_Pitch > -90.0f || MousePos->drag_y < 0))
		return true;

	return false;
}

D3DXVECTOR3* FPCamera::getForward()
{
	return &m_Forward;
}

D3DXVECTOR3* FPCamera::getLookAt()
{
	return &m_LookAt;
}

D3DXVECTOR3* FPCamera::getUp()
{
	return &m_Up;
}

D3DXVECTOR3* FPCamera::getPosition()
{
	return &m_Position;
}

void FPCamera::release()
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
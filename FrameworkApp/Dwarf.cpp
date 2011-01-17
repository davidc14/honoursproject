#include "Dwarf.h"

const float PI = 3.14159265f;

Dwarf::Dwarf()
{
	//Zero the matrices
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRotationY);
	D3DXMatrixIdentity(&m_matTranslation);
	D3DXMatrixIdentity(&m_matWorldInverseTranspose);

	//Zero the vectors
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	ResultVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ResultPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//Zero the other variables
	m_Angle = 0.0f;
	m_AngularVelocity = 0.0f;
	m_ID = 0;

	m_DeadReckoningTicker = 0.0f;
}

Dwarf::Dwarf(IDirect3DDevice9* Device)
{
	//Assign memory for the model objects
	m_Model = new XModel(Device);

	//If the model failed don't attempt to load it
	if(m_Model != NULL)
	{
		//Attempt to load the model
		if(!LoadModel())
		{
			//If model fails to load, freak out
			::MessageBox(0, "Ship model failed to load", "Ship error", 0);		
			return;
		}
	}
	else
	{
		//If the object can't be abstantiated, freak out
		::MessageBox(0, "Ship pointer failed to instantiate", "Ship error", 0);
	}

	//Zero the matrices
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRotationY);
	D3DXMatrixIdentity(&m_matTranslation);
	D3DXMatrixIdentity(&m_matWorldInverseTranspose);

	//Zero the vectors
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	ResultVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ResultPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//Zero the other variables
	m_Angle = 0.0f;
	m_AngularVelocity = 0.0f;
	m_ID = 0;

	m_DeadReckoningTicker = 0.0f;
}

Dwarf::~Dwarf()
{
	//m_Model->Release();
}

void Dwarf::Release()
{
	m_Model->Release();
}

//Move the ship to any location by passing in a vector
void Dwarf::MoveToLocation(D3DXVECTOR3 newPosition)
{
	m_Position = newPosition;
}

//Draw the ship
void Dwarf::Draw(ID3DXEffect* effect, D3DXHANDLE texture)
{
	m_Model->Draw(effect, texture);
}

//Update the ship
void Dwarf::Update(D3DXVECTOR3 newVelocity, float newAngularVelocity)
{
	UpdateVelocity(newVelocity);
	UpdatePosition();
	UpdateAngularVelocity(newAngularVelocity);
	UpdateAngle();
	UpdateWorldMatrix();
}

//Update the ship
void Dwarf::UpdateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity)
{
	//UpdateVelocity(CalculateDeadReckoning(deltaTime, position, velocity));
	MoveToLocation(CalculateDeadReckoning(deltaTime, position, velocity));
	UpdatePosition();
	UpdateAngularVelocity(0.0f);
	UpdateAngle();
	UpdateWorldMatrix();	
}

//Calculate the dead reckoning
D3DXVECTOR3 Dwarf::CalculateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity)
{
	//Zero the velocity every frame
	Acceleration = D3DXVECTOR3(0,0,0);

	//Calculate the acceleration based on the new velocity and the old velocity
	Acceleration = (velocity - m_Velocity) / deltaTime;

	//Perform the calculation
	ResultPosition = position + (velocity * deltaTime) + (Acceleration*(pow(deltaTime, 2)));

	//Swap the velocities to ensure a correct calculation
	m_Velocity = velocity;

	return ResultPosition;
}

//Update the ship
void Dwarf::Update()
{
	UpdatePosition();
	UpdateAngle();
	UpdateWorldMatrix();
}

void Dwarf::UpdatePosition()
{
	//Add the velocity to the position
	m_Position += m_Velocity;
}

void Dwarf::UpdateVelocity(D3DXVECTOR3 newVelocity)
{
	//Update the velocity
	m_Velocity = newVelocity;
}

bool Dwarf::LoadModel()
{
	//("Models/Dwarf", "Dwarf.x")
	//("Models/SpaceShip", "SpaceShip.x")
	//Attempt to load the model
	if(!m_Model->SetModel("Models/Dwarf", "Dwarf.x"))
		//Freak out if it fails
		return false;

	//No problem
	return true;
}

void Dwarf::UpdateAngle()
{
	m_Angle += m_AngularVelocity;
}

void Dwarf::UpdateAngularVelocity(float newAngularVelocity)
{
	//Check that the angle isn't out of range
	if(m_Angle > 2*PI)
		//Reset it if it is
		m_Angle = 0.0f;

	//Update the angle
	m_AngularVelocity = newAngularVelocity;	
}

void Dwarf::UpdateWorldMatrix()
{
	//Zero the matrices
	//D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixScaling(&m_matScale, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationY(&m_matRotationY, m_Angle);
	D3DXMatrixTranslation(&m_matTranslation, m_Position.x, m_Position.y, m_Position.z);

	m_matWorld = m_matScale * m_matRotationY * m_matTranslation;

	//Update the world inverse transpose of the model
	D3DXMatrixInverse(&m_matWorldInverseTranspose, NULL, &m_matWorld);
	D3DXMatrixTranspose(&m_matWorldInverseTranspose, &m_matWorldInverseTranspose);
}

//This is seperate from the standard update, because of the DX9 state machine
void Dwarf::UpdateShaderVariables(BasicLighting* LightingContainer)
{
	LightingContainer->matWorld = m_matWorld;
	LightingContainer->matWorldInverseTranspose = m_matWorldInverseTranspose;
}

D3DXVECTOR3 Dwarf::GetPosition()
{
	return m_Position;
}

D3DXVECTOR3 Dwarf::GetVelocity()
{
	return m_Velocity;
}

D3DXMATRIX Dwarf::GetWorld()
{
	return m_matWorld;
}

float Dwarf::GetAngle()
{
	return m_Angle;
}

float Dwarf::GetAngularVelocity()
{
	return m_AngularVelocity;
}
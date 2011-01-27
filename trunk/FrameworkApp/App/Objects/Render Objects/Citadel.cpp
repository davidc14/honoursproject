#include "Citadel.h"

const float PI = 3.14159265f;

Citadel::Citadel()
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

	mWhiteMtrl.ambient = WHITE*0.9f;
	mWhiteMtrl.diffuse = WHITE*0.6f;
	mWhiteMtrl.spec    = WHITE*0.6f;
	mWhiteMtrl.specPower = 48.0f;
}

Citadel::Citadel(IDirect3DDevice9* Device)
{
	//Assign memory for the model objects
	m_Model = new XModel(Device);

	Device->Release();

	//If the model failed don't attempt to load it
	if(m_Model != NULL)
	{
		//Attempt to load the model
		if(!LoadModel())
		{
			//If model fails to load, freak out
			::MessageBox(0, "Citadel model failed to load", "Ship error", 0);		
			return;
		}
	}
	else
	{
		//If the object can't be abstantiated, freak out
		::MessageBox(0, "Citadel pointer failed to instantiate", "Ship error", 0);
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

	mWhiteMtrl.ambient = WHITE*0.9f;
	mWhiteMtrl.diffuse = WHITE*0.6f;
	mWhiteMtrl.spec    = WHITE*0.6f;
	mWhiteMtrl.specPower = 48.0f;
}

Citadel::~Citadel()
{
	//m_Model->Release();
}

void Citadel::Release()
{
	m_Model->Release();
}

//Move the ship to any location by passing in a vector
void Citadel::MoveToLocation(D3DXVECTOR3 newPosition)
{
	m_Position = newPosition;
}

//Draw the ship
void Citadel::Draw(ID3DXEffect* effect, D3DXHANDLE texture)
{
	m_Model->Draw(effect, texture);
}

//Update the ship
void Citadel::Update(D3DXVECTOR3 newVelocity, float newAngularVelocity)
{
	UpdateVelocity(newVelocity);
	UpdatePosition();
	UpdateAngularVelocity(newAngularVelocity);
	UpdateAngle();
	UpdateWorldMatrix();
}

//Update the ship
void Citadel::UpdateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity)
{
	//UpdateVelocity(CalculateDeadReckoning(deltaTime, position, velocity));
	MoveToLocation(CalculateDeadReckoning(deltaTime, position, velocity));
	UpdatePosition();
	UpdateAngularVelocity(0.0f);
	UpdateAngle();
	UpdateWorldMatrix();	
}

//Calculate the dead reckoning
D3DXVECTOR3 Citadel::CalculateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity)
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
void Citadel::Update()
{
	UpdatePosition();
	UpdateAngle();
	UpdateWorldMatrix();
}

void Citadel::UpdatePosition()
{
	//Add the velocity to the position
	m_Position += m_Velocity;
}

void Citadel::UpdateVelocity(D3DXVECTOR3 newVelocity)
{
	//Update the velocity
	m_Velocity = newVelocity;
}

bool Citadel::LoadModel()
{
	//("Models/Citadel", "Citadel.x")
	//("Models/SpaceShip", "SpaceShip.x")
	//Attempt to load the model
	//if(!m_Model->SetModel("Models/Dwarf", "Dwarf.X"))
	if(!m_Model->SetModel("Models/Atrium", "sponza.X"))
		//Freak out if it fails
		return false;

	//No problem
	return true;
}

void Citadel::UpdateAngle()
{
	m_Angle += m_AngularVelocity;
}

void Citadel::UpdateAngularVelocity(float newAngularVelocity)
{
	//Check that the angle isn't out of range
	if(m_Angle > 2*PI)
		//Reset it if it is
		m_Angle = 0.0f;

	//Update the angle
	m_AngularVelocity = newAngularVelocity;	
}

void Citadel::UpdateWorldMatrix()
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
void Citadel::UpdateShaderVariables(BasicLighting* LightingContainer)
{
	LightingContainer->matWorld = m_matWorld;
	LightingContainer->matWorldInverseTranspose = m_matWorldInverseTranspose;
}

//This is seperate from the standard update, because of the DX9 state machine
void Citadel::UpdateShaderVariables(PhongLighting* LightingContainer)
{
	LightingContainer->m_World = m_matWorld;
	LightingContainer->m_WorldInvTrans = m_matWorldInverseTranspose;
	LightingContainer->m_Material = mWhiteMtrl;
}

D3DXVECTOR3 Citadel::GetPosition()
{
	return m_Position;
}

D3DXVECTOR3 Citadel::GetVelocity()
{
	return m_Velocity;
}

D3DXMATRIX Citadel::GetWorld()
{
	return m_matWorld;
}

float Citadel::GetAngle()
{
	return m_Angle;
}

float Citadel::GetAngularVelocity()
{
	return m_AngularVelocity;
}
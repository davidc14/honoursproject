#ifndef _CITADEL_H
#define _CITADEL_H

#include <d3d9.h>
#include <d3dx9.h>
#include "../../../XModel.h"
#include "../../../BasicLightingInterface.h"
#include "../../../App Framework/Shader Interface/PhongLightingInterface.h"
#include "../../../App Framework/Utilities/d3dUtil.h"

class Citadel
{
public: //Public methods
	Citadel();
	Citadel(IDirect3DDevice9* Device);
	~Citadel();

	//Move the Citadel to any location by passing in a vector
	void MoveToLocation(D3DXVECTOR3 newPosition);
	//Draw the Citadel
	void Draw(ID3DXEffect* effect, D3DXHANDLE texture);
	//Update the Citadel
	void Update();
	void Update(D3DXVECTOR3 newVelocity, float newAngularVelocity);
	void UpdateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity);
	//Update the shader variables
	void UpdateShaderVariables(BasicLighting* LightingContainer);
	void UpdateShaderVariables(PhongLighting* LightingContainer);

	void SetVelocity(D3DXVECTOR3 newVelocity){ UpdateVelocity(newVelocity); }
	void SetAngularVelocity(float newAngularVelocity) { UpdateAngularVelocity(newAngularVelocity); }

	void Release();

	Mtrl* GetMaterial() { return &mWhiteMtrl; }

	//Accessors for the data required by the network
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetVelocity();
	D3DXMATRIX GetWorld();
	D3DXMATRIX* GetWorldPointer();
	float GetAngle();
	float GetAngularVelocity();

public: //Public members
	

private: //Private member variables

	//The Citadel position
	D3DXVECTOR3 m_Position;
	//The Citadel velocity
	D3DXVECTOR3 m_Velocity;
	//The Citadel model
	XModel *m_Model;
	//The Citadel rotation
	float m_Angle, m_AngularVelocity;
	//The Citadel world matrix
	D3DXMATRIX m_matWorld, m_matWorldInverseTranspose;
	//The various components of the world matrix
	D3DXMATRIX m_matScale, m_matRotationY, m_matTranslation;
	//The Citadel ID in the game world
	int m_ID;
	//The material
	Mtrl mWhiteMtrl;

	//Values for Dead Reckoning
	D3DXVECTOR3 ResultVelocity;
	D3DXVECTOR3 ResultPosition;
	D3DXVECTOR3 Acceleration;
	float m_DeadReckoningTicker;

private: //Private methods

	D3DXVECTOR3 CalculateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity);
	void UpdatePosition();
	void UpdateVelocity(D3DXVECTOR3 newVelocity);	
	void UpdateAngle();
	void UpdateAngularVelocity(float newAngularVelocity);
	void UpdateWorldMatrix();
	
	bool LoadModel();
};

#endif
#ifndef _DWARF_H
#define _DWARF_H

#include <d3d9.h>
#include <d3dx9.h>
#include "XModel.h"
#include "BasicLightingInterface.h"
#include "App Framework/Shader Interface/PhongLightingInterface.h"

class Dwarf
{
public: //Public methods
	Dwarf();
	Dwarf(IDirect3DDevice9* Device);
	~Dwarf();

	//Move the dwarf to any location by passing in a vector
	void MoveToLocation(D3DXVECTOR3 newPosition);
	//Draw the dwarf
	void Draw(ID3DXEffect* effect, D3DXHANDLE texture);
	void DrawToShadowMap(ID3DXEffect* effect, D3DXHANDLE texture);
	//Update the dwarf
	void Update();
	void Update(D3DXVECTOR3 newVelocity, float newAngularVelocity);
	void UpdateDeadReckoning(float deltaTime, D3DXVECTOR3 position, D3DXVECTOR3 velocity);
	//Update the shader variables
	void UpdateShaderVariables(BasicLighting* LightingContainer);
	void UpdateShaderVariables(PhongLighting* LightingContainer);

	void SetVelocity(D3DXVECTOR3 newVelocity){ UpdateVelocity(newVelocity); }
	void SetAngularVelocity(float newAngularVelocity) { UpdateAngularVelocity(newAngularVelocity); }

	void Release();

	//Accessors for the data required by the network
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetVelocity();
	D3DXMATRIX GetWorld();
	float GetAngle();
	float GetAngularVelocity();

public: //Public members
	

private: //Private member variables

	//The dwarf position
	D3DXVECTOR3 m_Position;
	//The dwarf velocity
	D3DXVECTOR3 m_Velocity;
	//The dwarf model
	XModel *m_Model;
	//The dwarf rotation
	float m_Angle, m_AngularVelocity;
	//The dwarf world matrix
	D3DXMATRIX m_matWorld, m_matWorldInverseTranspose;
	//The various components of the world matrix
	D3DXMATRIX m_matScale, m_matRotationY, m_matTranslation;
	//The dwarf ID in the game world
	int m_ID;

	//Values for Dead Reckoning
	D3DXVECTOR3 ResultVelocity;
	D3DXVECTOR3 ResultPosition;
	D3DXVECTOR3 Acceleration;
	float m_DeadReckoningTicker;

	//The material
	Mtrl mWhiteMtrl;

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
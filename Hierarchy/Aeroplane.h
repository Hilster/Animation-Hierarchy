#ifndef AEROPLANE_H
#define AEROPLANE_H

//*********************************************************************************************
// File:			Aeroplane.h
// Description:		A very simple class to represent an aeroplane as one object with all the
//					hierarchical components stored internally within the class.
// Module:			Real-Time 3D Techniques for Games
// Created:			Jake - 2010-2011
// Notes:
//*********************************************************************************************

#include "Entity.h"
#include "Bullet.h"

ALLIGN_CLASS class Aeroplane : public Entity
{
  public:
	Aeroplane(char* file, float rotY);

	void Update(bool bPlayerControl); // Player only has control of plane when flag is set
	void Draw(void);

	void SetWorldPosition(float fX, float fY, float fZ);

  private:
	void UpdateMatrices(void);

	static bool s_bResourcesReady;

	Component* cRoot, *cProp, *cTurret, *cGun;

	XMFLOAT4 m_v4Rot; // Euler rotation angles
	XMFLOAT4 m_v4Pos; // World position

	XMVECTOR m_vForwardVector; // Forward Vector for Plane
	XMVECTOR m_vForwardVelocity; // Translation taken OR forward vector * speed
	float m_fSpeed; // Forward speed

	XMMATRIX m_mWorldTrans; // World translation matrix
	XMMATRIX m_mWorldMatrix; // World transformation matrix

	XMFLOAT4 m_v4PropRot; // Local rotation angles
	XMFLOAT4 m_v4PropOff; // Local offset

	XMFLOAT4 m_v4TurretRot; // Local rotation angles
	XMFLOAT4 m_v4TurretOff; // Local offset

	XMFLOAT4 m_v4GunRot; // Local rotation angles
	XMFLOAT4 m_v4GunOff; // Local offset

	XMFLOAT4 m_v4CamRot; // Local rotation angles
	XMFLOAT4 m_v4CamOff; // Local offset

	XMVECTOR m_vCamWorldPos; // World position
	XMMATRIX m_mCamWorldMatrix; // Camera's world transformation matrix

	bool m_bGunCam;

  public:
	float GetXPosition(void) { return m_v4Pos.x; }
	float GetYPosition(void) { return m_v4Pos.y; }
	float GetZPosition(void) { return m_v4Pos.z; }
	XMFLOAT4 GetFocusPosition(void) { return GetPosition(); }
	XMFLOAT4 GetCameraPosition(void)
	{
		XMFLOAT4 v4Pos;
		XMStoreFloat4(&v4Pos, m_vCamWorldPos);
		return v4Pos;
	}
	XMFLOAT4 GetPosition(void) { return m_v4Pos; }
	XMMATRIX GetFirePosition(void) { return cGun->GetWorldMatrix(); }
	XMVECTOR GetForwardVelocity(void) { return m_vForwardVelocity; }
	void SetGunCamera(bool value) { m_bGunCam = value; }
};

#endif
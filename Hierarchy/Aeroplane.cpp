//*********************************************************************************************
// File:			Aeroplane.cpp
// Description:		A very simple class to represent an aeroplane as one object with all the
//					hierarchical components stored internally within the class.
// Module:			Real-Time 3D Techniques for Games
// Created:			Jake - 2010-2011
// Notes:
//*********************************************************************************************

#include "Aeroplane.h"

bool Aeroplane::s_bResourcesReady = false;

Aeroplane::Aeroplane(char* file, float fRotY) : Entity(file, true)
{
	cRoot = GetComponent("root");
	cProp = GetComponent("prop");
	cTurret = GetComponent("turret");
	cGun = GetComponent("gun");

	m_v4Pos = cRoot->GetPosition();
	m_v4Rot = XMFLOAT4(0.0f, fRotY, 0.0f, 0.0f);

	m_v4PropOff = cProp->GetPosition();
	m_v4PropRot = cProp->GetRotation();

	m_v4TurretOff = cTurret->GetPosition();
	m_v4TurretRot = cTurret->GetRotation();

	m_v4GunOff = cGun->GetPosition();
	m_v4GunRot = cGun->GetRotation();

	m_v4CamOff = XMFLOAT4(0.0f, 4.5f, -15.0f, 0.0f);
	m_v4CamRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_vCamWorldPos = XMVectorZero();
	m_vForwardVector = XMVectorZero();

	m_fSpeed = 0.0f;

	m_bGunCam = false;
}

void Aeroplane::SetWorldPosition(float fX, float fY, float fZ)
{
	m_v4Pos = XMFLOAT4(fX, fY, fZ, 0.0f);
	UpdateMatrices();
}

void Aeroplane::UpdateMatrices(void)
{
	XMMATRIX mTrans;
	XMMATRIX mPlaneCameraMatrix;

	// Root matrix setter
	cRoot->SetPosition(m_v4Pos);
	cRoot->SetRotation(m_v4Rot);

	// Prop matrix setter
	cProp->SetPosition(m_v4PropOff);
	cProp->SetRotation(m_v4PropRot);

	// Turret matrix setter
	cTurret->SetPosition(m_v4TurretOff);
	cTurret->SetRotation(m_v4TurretRot);

	// Gun matrix setter
	cGun->SetPosition(m_v4GunOff);
	cGun->SetRotation(m_v4GunRot);

	cRoot->Update(1.f, true);

	// Matrix for camera to parent so that it ignores the planes x and z
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4Pos));
	XMFLOAT4 v4Axis = XMFLOAT4(0.f, 1.f, 0.f, 0.f);
	XMVECTOR vecAxis = XMLoadFloat4(&v4Axis);
	mPlaneCameraMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vecAxis, XMConvertToRadians(m_v4Rot.y))) * mTrans;

	// Forward vector
	m_vForwardVector = cRoot->GetForward();

	// Calculate m_mCameraWorldMatrix for camera based on rotation and position data.
	mTrans = XMMatrixTranslationFromVector(XMLoadFloat4(&m_v4CamOff));

	// Change camera parent based upon selected view
	if (m_bGunCam) {
		m_mCamWorldMatrix = mTrans * cGun->GetWorldMatrix();	// parent = gun
	}
	else {
		m_mCamWorldMatrix = mTrans * mPlaneCameraMatrix;	// parent = plane/camera matrix w/ x and z devised earlier
	}

	// Get the camera's world position (m_vCamWorldPos) out of m_mCameraWorldMatrix
	m_vCamWorldPos = m_mCamWorldMatrix.r[3];
}

void Aeroplane::Update(bool bPlayerControl)
{
	if (bPlayerControl)
	{
		// Check q pressed
		if (GetAsyncKeyState('Q')) {
			// Not reached minimum pitch? Pitch lower
			if (m_v4Rot.x < 60.0f)
				m_v4Rot.x += 2.0f;
		}
		// Check a pressed
		else if (GetAsyncKeyState('A')) {
			// Not reached maximum pitch? Pitch higher
			if (m_v4Rot.x > -60.0f)
				m_v4Rot.x -= 2.0f;
		}
		else if (m_v4Rot.x != 0.0f) {
			// No keys pressed? Return to zero
			if (m_v4Rot.x < 0)
				m_v4Rot.x += 3.0f;
			else if (m_v4Rot.x > 0.0f)
				m_v4Rot.x -= 3.0f;
			if (m_v4Rot.x + 1.5f < 3.0f && m_v4Rot.x + 1.5f > 0.0f)
				m_v4Rot.x = 0;
		}

		// Check o pressed
		if (GetAsyncKeyState('O')) {
			// Not reached maximum roll? Roll more
			if (m_v4Rot.z < 20.0f) {
				m_v4Rot.z += 2.0f;
			}
			// Never stop changing y
			m_v4Rot.y -= 2.0f;
		}
		// Check p pressed
		else if (GetAsyncKeyState('P')) {
			// Not reached maximum roll? Roll more
			if (m_v4Rot.z > -20.0f) {
				m_v4Rot.z -= 2.0f;
			}
			// Never stop changing y
			m_v4Rot.y += 2.0f;
		}
		else if (m_v4Rot.z != 0.0f) {
			// No keys pressed? Return to zero, but not y, y is the movement outcome, z is just for show
			if (m_v4Rot.z < 0.0f) {
				m_v4Rot.z += 3.0f;
			}
			else if (m_v4Rot.z > 0.0f) {
				m_v4Rot.z -= 3.0f;
			}
			if (m_v4Rot.z + 1.5f < 3.0f && m_v4Rot.z + 1.5f > 0.0f) {
				m_v4Rot.z = 0.0f;
			}
		}
	}

	// Apply a forward thrust and limit to a maximum speed of 1
	m_fSpeed += 0.001f;

	if (m_fSpeed > 1)
		m_fSpeed = 1;

	// Rotate propeller and turret
	m_v4PropRot.z += 100 * m_fSpeed;
	m_v4TurretRot.y += 0.1f;

	// Tilt gun up and down as turret rotates
	m_v4GunRot.x = (sin((float)XMConvertToRadians(m_v4TurretRot.y * 4.0f)) * 10.0f) - 10.0f;

	// Move Forward
	if (!GetAsyncKeyState('0')) {
		XMVECTOR vCurrPos = XMLoadFloat4(&m_v4Pos);
		m_vForwardVelocity = m_vForwardVector * m_fSpeed;
		vCurrPos += m_vForwardVelocity;
		XMStoreFloat4(&m_v4Pos, vCurrPos);
	}

	UpdateMatrices();
}

void Aeroplane::Draw(void)
{
	Entity::Draw();
}

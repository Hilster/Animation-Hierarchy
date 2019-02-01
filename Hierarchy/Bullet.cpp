#include "bullet.h"

// Initialise static class variables.
CommonMesh* Bullet::s_pBulletMesh = NULL;

bool Bullet::s_bResourcesReady = false;

Bullet::Bullet(const XMMATRIX& startingMatrix, const XMVECTOR additionalForward) {
	m_fSpeed = 2.f;

	// Forward vector
	m_vForwardVector = startingMatrix.r[2];

	m_vPos =  startingMatrix.r[3];

	XMFLOAT4 temp = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_mRotation = XMMATRIX(startingMatrix.r[0], startingMatrix.r[1], startingMatrix.r[2], XMLoadFloat4(&temp));

	m_vAdditionalForward = additionalForward;

	Update();
}

Bullet::~Bullet(void) {

}

void Bullet::Update() {
	XMMATRIX mTrans = XMMatrixTranslationFromVector(m_vPos);

	XMMATRIX scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	m_mBulletMatrix = m_mRotation * scale * mTrans;

	// Move Forward
	m_vPos += m_vForwardVector * m_fSpeed + m_vAdditionalForward;

}

void Bullet::LoadResources(void)
{
	s_pBulletMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Plane/bullet.x");
}

void Bullet::ReleaseResources(void)
{
	delete s_pBulletMesh;
}

void Bullet::Draw(void)
{
	Application::s_pApp->SetWorldMatrix(m_mBulletMatrix);
	s_pBulletMesh->Draw();
}

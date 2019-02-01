#ifndef BULLET_H
#define BULLET_H

#include "application.h"

ALLIGN_CLASS class Bullet {
public:
	Bullet(const XMMATRIX &startingMatrix, const XMVECTOR additionalForward);
	~Bullet(void);

	static void LoadResources(void); // Only load the resources once for all instances
	static void ReleaseResources(void); // Only free the resources once for all instances
	void Update();
	void Draw(void);

private:

	XMVECTOR m_vPos;
	XMVECTOR m_vForwardVector, m_vAdditionalForward;
	XMMATRIX m_mRotation, m_mBulletMatrix;

	float m_fSpeed;

	static CommonMesh * s_pBulletMesh;

	static bool s_bResourcesReady;
};

#endif
#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

#include "application.h"
#include "Bullet.h"
#include <vector>

using namespace std;

ALLIGN_CLASS class BulletManager {
public:
	BulletManager();
	~BulletManager(void);

	static void LoadResources(void);
	static void ReleaseResources(void);
	void Update();
	void Draw(void);
	void Fire(const XMMATRIX& startingMatrix, const XMVECTOR additionalForward);

private:
	vector<Bullet*> fired;
};

#endif
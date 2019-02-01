#include "BulletManager.h"

BulletManager::BulletManager() {

}

BulletManager::~BulletManager() {

}

void BulletManager::Fire(const XMMATRIX& startingMatrix, const XMVECTOR additionalForward) {
	fired.push_back(new Bullet(startingMatrix, additionalForward));
}

void BulletManager::Update() {
	for (int i = 0; i < fired.size(); ++i) {
		fired[i]->Update();
	}
}

void BulletManager::LoadResources(void) {
	Bullet::LoadResources();
}

void BulletManager::ReleaseResources(void) {
	Bullet::ReleaseResources();
}

void BulletManager::Draw(void) {
	for (int i = 0; i < fired.size(); ++i) {
		fired[i]->Draw();
	}
}
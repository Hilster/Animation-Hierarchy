#ifndef APPLICATION_H
#define APPLICATION_H

#define WIN32_LEAN_AND_MEAN
#define ALLIGN_CLASS _declspec(align(16))

#include <assert.h>

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>

#include "CommonApp.h"
#include "CommonMesh.h"

class Entity;
class Aeroplane;
class HeightMap;
class BulletManager;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Application : public CommonApp
{
  public:
	static Application* s_pApp;

  protected:
	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();

  private:
	float m_rotationAngle, m_cameraZ, m_fFrameCount;
	bool m_bWireframe;

	int m_cameraState;

	Entity* m_pRobot;
	Aeroplane* m_pAeroplane;
	BulletManager* m_pBulletManager;
	HeightMap* m_pHeightMap;
};

#endif

#include "Application.h"
#include "Heightmap.h"
#include "Aeroplane.h"
#include "Entity.h"
#include "BulletManager.h"

Application* Application::s_pApp = NULL;

const int CAMERA_MAP = 0;
const int CAMERA_PLANE = 1;
const int CAMERA_GUN = 2;
const int CAMERA_MAX = 3;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool Application::HandleStart()
{
	s_pApp = this;

	m_fFrameCount = 0.0f;

	this->SetWindowTitle("Hierarchy");

	m_bWireframe = false;

	m_pHeightMap = new HeightMap("Resources/heightmap.bmp", 2.0f);
	m_pHeightMap->ReloadShader();

	m_pAeroplane = new Aeroplane("Resources/planeHierarchy.txt", 105.f);
	m_pBulletManager = new BulletManager();
	m_pRobot = new Entity("Resources/robotHierarchy.txt", true);

	BulletManager::LoadResources();

	m_pAeroplane->LoadResources("Resources/Plane/");

	m_pRobot->LoadResources("Resources/Robot/");
	m_pRobot->LoadAnimation("Resources/RobotAttackAnim.dae", true);
	m_pRobot->LoadAnimation("Resources/RobotIdleAnim.dae", true);
	m_pRobot->LoadAnimation("Resources/RobotDieAnim.dae", false);

	m_cameraZ = 50.0f;
	m_rotationAngle = 0.f;

	if(!this->CommonApp::HandleStart())
		return false;

	this->SetRasterizerState(false, m_bWireframe);

	m_cameraState = CAMERA_MAP;

	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleStop()
{
	delete m_pHeightMap;
	m_pAeroplane->ReleaseResources();
	delete m_pAeroplane;
	BulletManager::ReleaseResources();
	delete m_pBulletManager;
	m_pRobot->ReleaseResources();
	delete m_pRobot;

	this->CommonApp::HandleStop();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleUpdate()
{
	m_rotationAngle += .01f;

	if(m_cameraState == CAMERA_MAP)
	{
		if(this->IsKeyPressed('Q'))
			m_cameraZ -= 2.0f;

		if(this->IsKeyPressed('A'))
			m_cameraZ += 2.0f;
	}

	static bool dbC = false;

	if(this->IsKeyPressed('C'))
	{
		if(!dbC)
		{
			if(++m_cameraState == CAMERA_MAX)
				m_cameraState = CAMERA_MAP;

			dbC = true;
		}
	}
	else
	{
		dbC = false;
	}

	static bool dbW = false;
	if(this->IsKeyPressed('W'))
	{
		if(!dbW)
		{
			m_bWireframe = !m_bWireframe;
			this->SetRasterizerState(false, m_bWireframe);
			dbW = true;
		}
	}
	else
	{
		dbW = false;
	}

	if (this->IsKeyPressed('1'))
	{
		m_pRobot->ChangeToAnimation(1);
	}
	else if(this->IsKeyPressed('2'))
	{
		m_pRobot->ChangeToAnimation(0);
	}
	else if (this->IsKeyPressed('3'))
	{
		m_pRobot->ChangeToAnimation(2);
	}

	m_pAeroplane->Update(m_cameraState != CAMERA_MAP);
	m_pBulletManager->Update();
	m_pRobot->Update(0.1f);

	//In control of plane and 'F' pressed
	if (this->IsKeyPressed('F') && m_cameraState != CAMERA_MAP) {
		//Bullet firing
		m_pBulletManager->Fire(m_pAeroplane->GetFirePosition(), m_pAeroplane->GetForwardVelocity());
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleRender()
{
	XMFLOAT3 vUpVector(0.0f, 1.0f, 0.0f);
	XMFLOAT3 vCamera, vLookat;

	switch(m_cameraState)
	{
		case CAMERA_MAP:
			vCamera = XMFLOAT3(sin(m_rotationAngle) * m_cameraZ, m_cameraZ / 4, cos(m_rotationAngle) * m_cameraZ);
			vLookat = XMFLOAT3(0.0f, 4.0f, 0.0f);
			break;
		case CAMERA_PLANE:
			m_pAeroplane->SetGunCamera(false);
			vCamera = XMFLOAT3(m_pAeroplane->GetCameraPosition().x, m_pAeroplane->GetCameraPosition().y, m_pAeroplane->GetCameraPosition().z);
			vLookat = XMFLOAT3(m_pAeroplane->GetFocusPosition().x, m_pAeroplane->GetFocusPosition().y, m_pAeroplane->GetFocusPosition().z);
			break;
		case CAMERA_GUN:
			m_pAeroplane->SetGunCamera(true);
			vCamera = XMFLOAT3(m_pAeroplane->GetCameraPosition().x, m_pAeroplane->GetCameraPosition().y, m_pAeroplane->GetCameraPosition().z);
			vLookat = XMFLOAT3(m_pAeroplane->GetFocusPosition().x, m_pAeroplane->GetFocusPosition().y, m_pAeroplane->GetFocusPosition().z);
			break;
	}

	XMMATRIX matView;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&vCamera), XMLoadFloat3(&vLookat), XMLoadFloat3(&vUpVector));

	XMMATRIX matProj;
	matProj = XMMatrixPerspectiveFovLH(float(XM_PI / 4), 2, 1.5f, 5000.0f);

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->EnablePointLight(0, XMFLOAT3(100.0f, 100.f, -100.f), XMFLOAT3(1.f, 1.f, 1.f));
	this->SetLightAttenuation(0, 200.f, 2.f, 2.f, 2.f);
	this->EnableDirectionalLight(1, XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT3(0.65f, 0.55f, 0.65f));

	this->Clear(XMFLOAT4(.2f, .2f, .6f, 1.f));

	XMMATRIX matWorld;
	matWorld = XMMatrixIdentity();
	this->SetWorldMatrix(matWorld);

	m_pHeightMap->Draw(m_fFrameCount);
	m_pAeroplane->Draw();
	m_pBulletManager->Draw();
	m_pRobot->Draw();

	m_fFrameCount++;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include "Component.h"

Component::Component(const string& sName, const XMFLOAT4& v4Pos, const XMFLOAT4& v4Rot) : m_sName(sName), m_v4Pos(v4Pos), m_v4Rot(v4Rot) {

}

Component::~Component(void) {

}

// Setters
void Component::AddChild(Component* cChild) {
	m_vChildren.push_back(cChild);
}

void Component::SetPosition(const XMFLOAT4& v4Pos) {
	m_v4Pos = v4Pos;
}

void Component::SetRotation(const XMFLOAT4& v4Rot) {
	m_v4Rot = v4Rot;
}

void Component::SetRotationX(const float& x) {
	m_v4Rot = XMFLOAT4(x, m_v4Rot.y, m_v4Rot.z, 0.f);
}

void Component::SetRotationY(const float& y) {
	m_v4Rot = XMFLOAT4(m_v4Rot.x, y, m_v4Rot.z, 0.f);
}

void Component::SetRotationZ(const float& z) {
	m_v4Rot = XMFLOAT4(m_v4Rot.x, m_v4Rot.y, z, 0.f);
}

// Each frame update component with parent matrix
void Component::Update(const XMMATRIX& mParentMatrix, float fTransformScale, bool bPlaneRotations) {
	XMMATRIX mRotX, mRotY, mRotZ, mTrans;

	GetUpdatedRotations(mRotX, mRotY, mRotZ);
	mTrans = XMMatrixTranslationFromVector(XMVectorScale(XMLoadFloat4(&m_v4Pos), fTransformScale));
	if (bPlaneRotations)
		m_mWorldMatrix = mRotZ * mRotX * mRotY * mTrans * mParentMatrix;
	else
		m_mWorldMatrix = mRotX * mRotY * mRotZ * mTrans * mParentMatrix;
	// Update children with this components matrix
	for (int i = 0; i < m_vChildren.size(); ++i) {
		m_vChildren[i]->Update(m_mWorldMatrix, fTransformScale, bPlaneRotations);
	}
}

// Each frame update component without parent matrix
void Component::Update(float fTransformScale, bool bPlaneRotations) {
	XMMATRIX mRotX, mRotY, mRotZ, mTrans;

	GetUpdatedRotations(mRotX, mRotY, mRotZ);
	mTrans = XMMatrixTranslationFromVector(XMVectorScale(XMLoadFloat4(&m_v4Pos), fTransformScale));
	if(bPlaneRotations)
		m_mWorldMatrix = mRotZ * mRotX * mRotY * mTrans;
	else
		m_mWorldMatrix = mRotX * mRotY * mRotZ * mTrans;
	// Update children with this components matrix
	for (int i = 0; i < m_vChildren.size(); ++i) {
		m_vChildren[i]->Update(m_mWorldMatrix, fTransformScale, bPlaneRotations);
	}
}

// Set rotation variables with quaternion rotations
void Component::GetUpdatedRotations(XMMATRIX& mRotX, XMMATRIX& mRotY, XMMATRIX& mRotZ) {
	XMFLOAT4 v4Axis = XMFLOAT4(1.f, 0.f, 0.f, 0.f);
	XMVECTOR vecAxis = XMLoadFloat4(&v4Axis);
	mRotX = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vecAxis, XMConvertToRadians(m_v4Rot.x)));

	v4Axis = XMFLOAT4(0.f, 1.f, 0.f, 0.f);
	vecAxis = XMLoadFloat4(&v4Axis);
	mRotY = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vecAxis, XMConvertToRadians(m_v4Rot.y)));

	v4Axis = XMFLOAT4(0.f, 0.f, 1.f, 0.f);
	vecAxis = XMLoadFloat4(&v4Axis);
	mRotZ = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vecAxis, XMConvertToRadians(m_v4Rot.z)));
}

void Component::LoadResources(const char* rootFolder) {
	if (m_sName != "root") {
		string file = rootFolder + m_sName + ".x";
		m_pComponentMesh = CommonMesh::LoadFromXFile(Application::s_pApp, file.c_str());
	}
}

void Component::ReleaseResources(void) {
	if (m_sName != "root") {
		delete m_pComponentMesh;
	}
}

void Component::Draw(void) {
	if (m_sName != "root") {
		Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
		m_pComponentMesh->Draw();
	}
}

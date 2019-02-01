#ifndef COMP_H
#define COMP_H

#include "Application.h"
#include <iterator>
#include <string>
#include <vector>

using namespace std;

ALLIGN_CLASS class Component {
public:
	Component(const string& name, const XMFLOAT4& v4Pos, const XMFLOAT4& v4Rot);
	~Component(void);

	void LoadResources(const char* rootFolder);
	void ReleaseResources(void);
	void Update(const XMMATRIX& parentMatrix, float fTransformScale, bool bPlaneRotations);
	void Update(float fTransformScale, bool bPlaneRotations);
	void Draw(void);

	void AddChild(Component* child);
	void SetPosition(const XMFLOAT4& v4Pos);
	void SetRotation(const XMFLOAT4& v4Rot);
	void SetRotationX(const float& x);
	void SetRotationY(const float& y);
	void SetRotationZ(const float& z);

private:
	void GetUpdatedRotations(XMMATRIX& mRotX, XMMATRIX& mRotY, XMMATRIX& mRotZ);

	CommonMesh * m_pComponentMesh;

	vector<Component*> m_vChildren;

	string m_sName;
	XMFLOAT4 m_v4Pos, m_v4Rot;

	XMMATRIX m_mWorldMatrix;

public:
	// Getters
	const string GetName() { return m_sName; }
	const XMVECTOR GetForward() { return m_mWorldMatrix.r[2]; }
	const XMFLOAT4 GetPosition() { return m_v4Pos; }
	const XMFLOAT4 GetRotation() { return m_v4Rot; }
	const XMMATRIX GetWorldMatrix() { return m_mWorldMatrix; }

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

};

#endif
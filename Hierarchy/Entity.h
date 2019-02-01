#ifndef ENTITY_H
#define ENTITY_H

#include "application.h"
#include "Component.h"
#include "Animation.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Animation;

ALLIGN_CLASS class Entity {
public:
	Entity(char* file, bool fileIsHierarchyFile);
	~Entity(void);

	void LoadResources(const char* rootFolder);
	void ReleaseResources(void);
	void Update(float fTransformScale);
	void Draw(void);
	void LoadAnimation(const char* file, bool repeat);
	void ChangeToAnimation(int animationNumber);

private:
	void LoadHierarchyFile(const char* file);

	vector<Component*> m_vComponents;
	vector<Animation*> m_vAnimations;
	int m_iCurrentAnimation, m_iPreviousAnimation;
	high_resolution_clock::duration m_cTimer, m_cBlendTimer;

public:
	//Getters
	Component* GetComponent(const string& name);

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
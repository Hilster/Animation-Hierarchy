#ifndef ANI_H
#define ANI_H

#include "Entity.h"
#include "Component.h"
#include "tinyxml/tinyxml.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

enum AnimationAction {
	TRANSLATE = 0,
	ROTATEX,
	ROTATEY,
	ROTATEZ,
	VISIBILITY
};

struct AnimationChannel {
	Component* object;
	AnimationAction action;
	vector<float> input, output;
};

ALLIGN_CLASS class Animation {
public:
	Animation(const char* file, bool repeat, Entity* parent);
	~Animation(void);
	void Update(float fTimeInSeconds);
	void Update(float fTimeInSeconds, float fBlendTime, Animation* aPrevious);
	void Update(Component* cComponent, float fTimeInSeconds, XMFLOAT4& v4Pos, float& fRotx, float& fRoty, float& fRotz);

private:
	vector<AnimationChannel> m_vChannels;
	AnimationChannel previousKeyframe;
	bool m_bRepeat;
	float m_fLastKeyframeTime;

	void animationArray(vector<float>& output, const TiXmlHandle& source, const string& waste, bool input);
	XMFLOAT4 AnimationTranslate(const AnimationChannel& kf, float t);
	float AnimationRotate(const AnimationChannel& kf, float t);
	XMFLOAT4 TranslationLerp(const XMFLOAT4& a, const XMFLOAT4& b, float t);
	float lerp(float x, float y, float t);

public:
	// Getters
	bool GetRepeat() { return m_bRepeat; }

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
#include "Animation.h"

Animation::Animation(const char* file, bool repeat, Entity* parent) :
	m_bRepeat(repeat)
{
	m_fLastKeyframeTime = -1.f;
	// Load file
	TiXmlDocument doc(file);
	bool loadSuccessful = doc.LoadFile();

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pRootChild;
	TiXmlHandle hRoot(0);
	string waste;

	if (loadSuccessful) {
		// First COLLADA element
		pElem = hDoc.FirstChildElement().ToElement();
		hRoot = TiXmlHandle(pElem);

		// Second COLLADA child which is library_animations
		pElem = hRoot.Child(1).ToElement();
		hRoot = TiXmlHandle(pElem);

		pRootChild = hRoot.FirstChild().ToElement();
		do {
			AnimationChannel c;

			// Set up object and action for the keyframe
			string ID = pRootChild->Attribute("id");
			waste = ".";
			size_t pos = ID.find(waste);
			// Place object into channel
			c.object = parent->GetComponent(ID.substr(0, pos));
			ID.erase(0, pos + waste.length());
			if (ID == "translate")
				c.action = TRANSLATE;
			else if (ID == "rotateX")
				c.action = ROTATEX;
			else  if (ID == "rotateY")
				c.action = ROTATEY;
			else  if (ID == "rotateZ")
				c.action = ROTATEZ;
			else
				c.action = VISIBILITY;

			waste = " ";
			hRoot = TiXmlHandle(pRootChild);

			// Input
			TiXmlHandle pSource = hRoot.FirstChild();
			animationArray(c.input, pSource, waste, true);

			// Output
			pSource = TiXmlHandle(pSource.ToElement()->NextSibling());
			animationArray(c.output, pSource, waste, false);

			// Push frame to storage vector
			m_vChannels.push_back(c);
			pRootChild = pRootChild->NextSiblingElement();
		} while (pRootChild);
	}

}

Animation::~Animation(void) {

}

// Generic function for the processes used for both input and output vectors
void Animation::animationArray(vector<float>& vfOutput, const TiXmlHandle& hSource, const string& sWaste, bool bIsInput) {
	TiXmlElement *pFloatArray = hSource.FirstChild().ToElement();
	int count = stoi(string(pFloatArray->Attribute("count")));
	string input = pFloatArray->GetText();
	// Foreach variable needed to be stored
	for (int i = 0; i < count; ++i) {
		// If the last variable
		if (i == count - 1) {
			vfOutput.push_back(stof(input));
			// If input and keyframe with the latest time
			if (bIsInput && m_fLastKeyframeTime < vfOutput[i])
				m_fLastKeyframeTime = vfOutput[i];	// Set time to last keyframe
		}
		else {
			// Store variable in output vector
			size_t pos = input.find(sWaste);
			vfOutput.push_back(stof(input.substr(0, pos)));
			input.erase(0, pos + sWaste.length());
		}
	}
}

// Each frame for animation without blending
void Animation::Update(float fTimeInSeconds) {
	float timeUnits;

	// If a looping animation
	if (m_bRepeat) {
		timeUnits = fmod(fTimeInSeconds, m_fLastKeyframeTime);	// Get the time through current loop
	}
	else {
		timeUnits = fTimeInSeconds;	// No need for time through loop
	}

	// foreach channel
	for (int i = 0; i < m_vChannels.size(); ++i) {
		// Set respective variables on respective components
		switch (m_vChannels[i].action) {
		case TRANSLATE:
			m_vChannels[i].object->SetPosition(AnimationTranslate(m_vChannels[i], timeUnits));
			break;
		case ROTATEX:
			m_vChannels[i].object->SetRotationX(AnimationRotate(m_vChannels[i], timeUnits));
			break;
		case ROTATEY:
			m_vChannels[i].object->SetRotationY(AnimationRotate(m_vChannels[i], timeUnits));
			break;
		case ROTATEZ:
			m_vChannels[i].object->SetRotationZ(AnimationRotate(m_vChannels[i], timeUnits));
			break;
		}
	}
}

// Each frame for returning products of the animation
void Animation::Update(Component* pComponent, float fTimeInSeconds, XMFLOAT4& v4Pos, float& fRotX, float& fRotY, float& fRotZ) {
	float timeUnits;

	// If a looping animation
	if (m_bRepeat) {
		timeUnits = fmod(fTimeInSeconds, m_fLastKeyframeTime);	// Get the time through current loop
	}
	else {
		timeUnits = fTimeInSeconds;	// No need for time through loop
	}

	// foreach channel
	for (int i = 0; i < m_vChannels.size(); ++i) {
		if (m_vChannels[i].object == pComponent) {
			// Set respective variables for specific component
			switch (m_vChannels[i].action) {
			case TRANSLATE:
				v4Pos = AnimationTranslate(m_vChannels[i], timeUnits);
				break;
			case ROTATEX:
				fRotX = AnimationRotate(m_vChannels[i], timeUnits);
				break;
			case ROTATEY:
				fRotY = AnimationRotate(m_vChannels[i], timeUnits);
				break;
			case ROTATEZ:
				fRotZ = AnimationRotate(m_vChannels[i], timeUnits);
				break;
			}
		}
	}
}

// Each frame for blending with another animation
void Animation::Update(float fTimeInSeconds, float fBlendTime, Animation* aPrevious) {
	XMFLOAT4 prevPos = XMFLOAT4(0.f, 0.f, 0.f, 0.f), translationOutput;
	float timeUnits, blendingTimeUnits = 0.5f - fBlendTime,
		prevRotX = 0.f, rotXOutput = 0.f,
		prevRotY = 0.f, rotYOutput = 0.f,
		prevRotZ = 0.f, rotZOutput = 0.f;

	// If a looping animation
	if (m_bRepeat) {
		timeUnits = fmod(fTimeInSeconds, m_fLastKeyframeTime);	// Get the time through current loop
	}
	else {
		timeUnits = fTimeInSeconds;	// No need for time through loop
	}

	for (int i = 0; i < m_vChannels.size(); ++i) {
		// Get previous animations variables
		aPrevious->Update(m_vChannels[i].object, timeUnits, prevPos, prevRotX, prevRotY, prevRotZ);

		// Set respective variables on respective components
		switch (m_vChannels[i].action) {
		case TRANSLATE:
			translationOutput = TranslationLerp(prevPos, AnimationTranslate(m_vChannels[i], blendingTimeUnits), blendingTimeUnits * 2.f);
			m_vChannels[i].object->SetPosition(translationOutput);
			break;

		case ROTATEX:
			rotXOutput = lerp(prevRotX, AnimationRotate(m_vChannels[i], blendingTimeUnits), blendingTimeUnits * 2.f);
			m_vChannels[i].object->SetRotationX(rotXOutput);
			break;

		case ROTATEY:
			rotYOutput = lerp(prevRotY, AnimationRotate(m_vChannels[i], blendingTimeUnits), blendingTimeUnits * 2.f);
			m_vChannels[i].object->SetRotationY(rotYOutput);
			break;

		case ROTATEZ:
			rotZOutput = lerp(prevRotZ, AnimationRotate(m_vChannels[i], blendingTimeUnits), blendingTimeUnits * 2.f);
			m_vChannels[i].object->SetRotationZ(rotZOutput);
			break;
		}
	}
}

// Get translate
XMFLOAT4 Animation::AnimationTranslate(const AnimationChannel& kf, float t) {
	XMFLOAT4 v4Output;

	// Default point1 as end and point2 as beginning
	XMFLOAT4 v4Point1 = XMFLOAT4(kf.output[kf.output.size() - 3], kf.output[kf.output.size() - 2], kf.output[kf.output.size() - 1], 0.f),
		v4Point2 = XMFLOAT4(kf.output[0], kf.output[1], kf.output[2], 0.f);

	// If input vector has more than one variable in the vector...
	if (kf.input.size() != 1) {
		// ...calculate position between previous and next keyframes
		float time = 0.f;	// Time between last frame and next frame
		for (int i = 0; i < (kf.input.size() / 3); ++i) {
			// Output will be calculated with incorrect data until the last time this statement is true, meaning output returns correct
			// TODO: Improve efficiency by only calculating the final data that we want instead of those prior to the ones we want as well
			if (t > kf.input[i]) {
				// point1 = point in array
				v4Point1 = XMFLOAT4(kf.output[(i * 3)], kf.output[(i * 3) + 1], kf.output[(i * 3) + 2], 0.f);

				int nextPoint = 0;
				// If this isn't the last point of the keyframes
				if (i != (kf.output.size() / 3) - 1) {
					nextPoint = i + 1;
					time = (t - kf.input[i]) / (kf.input[nextPoint] - kf.input[i]);
				}
				else if (!m_bRepeat) {
					// If animation doesn't loop, don't loop
					nextPoint = i;
					time = 1.f;
				}
				else {
					// Loop
					time = kf.input[nextPoint];
				}
				// point2 = next point in array
				v4Point2 = XMFLOAT4(kf.output[(nextPoint * 3)], kf.output[(nextPoint * 3) + 1], kf.output[(nextPoint * 3) + 2], 0.f);
			}
		}
		v4Output = TranslationLerp(v4Point1, v4Point2, time);	// Lerp between the two points using time taken through these two keyframes
	}
	else {
		// Put the one variable as the output
		v4Output = v4Point1;
	}

	return v4Output;
}

// Get rotate
float Animation::AnimationRotate(const AnimationChannel& kf, float t) {
	float fOutput;
	// Default point1 as end and point2 as beginning
	float fPoint1 = kf.output[kf.output.size() - 1], fPoint2 = kf.output[0];

	// If input vector has more than one variable in the vector...
	if (kf.input.size() != 1) {
		// ...calculate position between previous and next keyframes
		float time = 0.f;
		for (int i = 0; i < kf.output.size(); ++i) {
			// Output will be calculated with incorrect data until the last time this statement is true, meaning output returns correct
			// TODO: Improve efficiency by only calculating the final data that we want instead of those prior to the ones we want as well
			if (t > kf.input[i]) {
				// point1 = point in array
				fPoint1 = kf.output[i];

				int nextPoint = 0;
				// If this isn't the last point of the keyframes
				if (i != ((kf.output.size()) - 1)) {
					nextPoint = i + 1;
					time = (t - kf.input[i]) / (kf.input[nextPoint] - kf.input[i]);
				}
				else if (!m_bRepeat) {
					// If animation doesn't loop, don't loop
					nextPoint = i;
					time = 1.f;
				}
				else {
					// Loop
					time = kf.input[nextPoint];
				}
				// point2 = next point in array
				fPoint2 = kf.output[nextPoint];
			}
		}
		fOutput = lerp(fPoint1, fPoint2, time);	// Lerp between the two points using time taken through these two keyframes
	}
	else {
		// Put the one variable as the output
		fOutput = fPoint1;
	}

	return fOutput;
}

// Blend two vectors
XMFLOAT4 Animation::TranslationLerp(const XMFLOAT4& a, const XMFLOAT4& b, float t) {
	return XMFLOAT4(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), 0.f);
}

// Linear interpolation between two floats using a time variable
float Animation::lerp(float x, float y, float t) {
	t = (t < 0 ? 0 : t) > 1 ? 1 : t;
	return (1 - t) * x + t * y;
}

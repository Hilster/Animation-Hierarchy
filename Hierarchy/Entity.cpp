#include "Entity.h"

Entity::Entity(char* file, bool fileIsHierarchyFile) :
	m_cTimer(chrono::seconds(0)),
	m_cBlendTimer(chrono::seconds(0)),
	m_iCurrentAnimation(0),
	m_iPreviousAnimation(0)
{
	if(fileIsHierarchyFile)
		// Hierarchy of models
		LoadHierarchyFile(file);
	else {
		// Single model
		Component * temp = new Component(file,
			XMFLOAT4(0.f, 0.f, 0.f, 0.f),
			XMFLOAT4(0.f, 0.f, 0.f, 0.f));
		m_vComponents.push_back(temp);
	}
}

Entity::~Entity(void) {

}

void Entity::LoadHierarchyFile(const char* file) {
	string line;
	ifstream hierarchyFile(file);

	// Open file
	if (hierarchyFile.is_open()) {
		int counter = 0;
		string component, parent, waste;
		size_t pos;
		while (getline(hierarchyFile, line)) {
			++counter;
			switch (counter) {
			case 1:
				// Name line
				waste = "\"";
				pos = line.find(waste);
				line.erase(0, pos + waste.length());
				pos = line.find(waste);
				component = line.substr(0, pos);
				break;
			case 2:
				// Parent line
				waste = "\"";
				pos = line.find(waste);
				line.erase(0, pos + waste.length());
				pos = line.find(waste);
				parent = line.substr(0, pos);
				break;
			case 3:
				// Position line
				string value;
				waste = ", ";
				vector<float> outputCords;
				pos = 0;
				while ((pos = line.find(waste)) != string::npos) {
					value = line.substr(0, pos);
					outputCords.push_back(stof(value));
					line.erase(0, pos + waste.length());
				}
				outputCords.push_back(stof(line));

				// Create current component and add it to the components vector
				Component * temp = new Component(component,
					XMFLOAT4(outputCords[0], outputCords[1], outputCords[2], 1.f),
					XMFLOAT4(0.f, 0.f, 0.f, 0.f));
				m_vComponents.push_back(temp);

				// Add current component to the children of it's parent
				for (int i = 0; i < m_vComponents.size(); ++i) {
					if (m_vComponents[i]->GetName() == parent)
						m_vComponents[i]->AddChild(m_vComponents.back());
				}

				counter = 0;
				break;
			}
		}
	}
}

void Entity::LoadResources(const char* rootFolder) {
	for (int i = 0; i < m_vComponents.size(); ++i) {
		m_vComponents[i]->LoadResources(rootFolder);
	}
}

// Create animation component and add it to the holding vector
void Entity::LoadAnimation(const char* file, bool repeat) {
	m_vAnimations.push_back(new Animation(file, repeat, this));
}

// If animation is different to the current animation, change
void Entity::ChangeToAnimation(int animationNumber) {
	if (animationNumber != m_iCurrentAnimation) {
		m_iPreviousAnimation = m_iCurrentAnimation;
		m_cBlendTimer = chrono::milliseconds(500);
		m_iCurrentAnimation = animationNumber;
	}
}

// Return component with name
Component* Entity::GetComponent(const string& sName) {
	for (int i = 0; i < m_vComponents.size(); ++i) {
		if (m_vComponents[i]->GetName() == sName) {
			return m_vComponents[i];
		}
	}
	return nullptr;
}

void Entity::ReleaseResources(void) {
	for (int i = 0; i < m_vComponents.size(); ++i) {
		m_vComponents[i]->ReleaseResources();
	}
}

// Each frame
void Entity::Update(float fTransformScale) {
	if (m_vAnimations.size() > 0) {
		static high_resolution_clock::time_point lastTime = high_resolution_clock::now();
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();

		m_cTimer += (currentTime - lastTime);
		float TimeInSeconds = (float)(m_cTimer.count()) / 1.0e9f;

		// If we are transitioning from one animation to another...
		if (m_iPreviousAnimation != m_iCurrentAnimation) {
			// ... do transition processes
			m_cBlendTimer -= (currentTime - lastTime);	// Count down blend timer
			// If we are still blending
			if (m_cBlendTimer.count() > 0) {
				const float BlendTimeInSeconds = (float)(m_cBlendTimer.count()) / 1.0e9f;	// Get blend timer in seconds
				m_vAnimations[m_iCurrentAnimation]->Update(TimeInSeconds, BlendTimeInSeconds, m_vAnimations[m_iPreviousAnimation]);
			}
			else {
				m_iPreviousAnimation = m_iCurrentAnimation;
				m_cTimer = chrono::milliseconds(500);
				TimeInSeconds = (float)(m_cTimer.count()) / 1.0e9f;
				m_vAnimations[m_iCurrentAnimation]->Update(TimeInSeconds);
			}
		}
		else {
			// If not, do regular animation update
			m_vAnimations[m_iCurrentAnimation]->Update(TimeInSeconds);
		}
		// Set last time for the next frame
		lastTime = currentTime;
	}

	// Update each components matrix with their new position and rotation values
	m_vComponents[0]->Update(fTransformScale, false);
}

void Entity::Draw(void) {
	for (int i = 0; i < m_vComponents.size(); ++i) {
		m_vComponents[i]->Draw();
	}
}
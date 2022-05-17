#include "ofMain.h"

class Force {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(glm::vec3*) = 0;
};

class ImpulseForce : public Force {
	float magnitude;
	glm::vec3 direction;
public:
	ImpulseForce(float magnitude, glm::vec3 direction);
	void set(float magnitude, glm::vec3 direction) {
		setMagnitude(magnitude);
		setDirection(direction);
	}
	void setMagnitude(float magnitude);
	void setDirection(glm::vec3 direction); // Unit vector direction.
	void updateForce(glm::vec3*);
};

class GForce : public Force {
	float magnitude;
	glm::vec3 direction = glm::vec3(0, -1, 0);
public:
	GForce(float magnitude);
	void set(float magnitude) {
		setMagnitude(magnitude);
	}
	void setMagnitude(float magnitude);
	void updateForce(glm::vec3*);
};
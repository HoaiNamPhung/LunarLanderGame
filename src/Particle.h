#pragma once

#include "ofMain.h"
#include "TransformObject.h"

class ParticleForceField;

class Particle : public TransformObject {
public:
	Particle();
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 netForce = glm::vec3(0, 0, 0);
	float	damping = 1;
	float   mass = 1;
	float   lifespan = 1;
	float   radius = 1;
	float   birthtime = 0;
	void    integrate();
	void    draw();
	float   age();        // seconds
	ofColor color = ofColor::white;
};



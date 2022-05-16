#pragma once

#include "ofMain.h"
#include "TransformObject.h"

class ParticleForceField;

class Particle : public TransformObject {
public:
	Particle();
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 netForce;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	void    integrate();
	void    draw();
	float   age();        // sec
	ofColor color;
};



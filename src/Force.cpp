#include "Force.h"

ImpulseForce::ImpulseForce(float magnitude, glm::vec3 direction) {
	this->magnitude = magnitude;
	this->direction = direction;
	applyOnce = true;
}

void ImpulseForce::setMagnitude(float magnitude) {
	this->magnitude = magnitude;
}

void ImpulseForce::setDirection(glm::vec3 direction) {
	this->direction = direction;
}

void ImpulseForce::updateForce(glm::vec3* force) {
	force->x += direction.x * magnitude;
	force->y += direction.y * magnitude;
	force->z += direction.z * magnitude;
}
#pragma once
#include "Player.h"

glm::vec3 Player::heading(float len) {
	glm::vec3 dir = glm::vec3(sin(glm::radians(rotation.y)), 0, -cos(glm::radians(rotation.y)));
	return len * glm::normalize(dir);
}

void Player::drawHeadingVector() {
	if (!showHeadingVector) return;
	ofSetColor(ofColor::green);
	ofDrawLine(position, position + heading(75.0f));
	ofSetColor(ofColor::white);
}

// TODO:
bool Player::inside(glm::vec3 p) {
	return false;
}

void Player::integrate() {
	// init current framerate (or you can use ofGetLastFrameTime())
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	// linear motion
	position += (velocity * dt);
	glm::vec3 accel = acceleration;
	accel += (force * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion about y-axis
	rotation.y += (angularVelocity * dt);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;

	// Zero out forces.
	force = glm::vec3(0, 0, 0);
}

void Player::addForce(Force* f) {
	forces.push_back(f);
}

void Player::updateForces() {
	// Apply forward force based on player movement direction.
	if (aDir != ms::accelDir::NONE) {
		glm::vec3 dirVector = heading(1.0f);
		if (aDir == ms::accelDir::BACKWARD) {
			dirVector.x *= -1;
			dirVector.y *= -1;
		}
		ImpulseForce* f = new ImpulseForce(thrust, dirVector);
		addForce(f);
	}
	// Apply rotational force based on player rotation direction.
	if (rDir != ms::rotateDir::NONE) {
		angularForce = torque;
		if (rDir == ms::rotateDir::LEFT) {
			angularForce *= -1;
		}
	}
	// Immediately stop rotation when let go.
	else {
		angularForce = 0;
	}

	// Update forces on ship.
	for (int k = 0; k < forces.size(); k++) {
		if (!forces[k]->applied)
			forces[k]->updateForce(&force);
	}
	// Turn off impulse forces so that they aren't reapplied.
	for (int i = 0; i < forces.size(); i++) {
		if (forces[i]->applyOnce)
			forces[i]->applied = true;
	}
}

void Player::removeSideSlipping() {
	// Get heading vector.
	glm::vec3 headingVector = heading(1.0f);
	if (aDir == ms::accelDir::BACKWARD) {
		headingVector *= -1;
	}
	// Set residual velocity to be entirely directed in direction of heading vector.
	float forwardVectorLength = glm::dot(velocity, headingVector);
	glm::vec3 forwardVector = heading(forwardVectorLength);
	if (aDir == ms::accelDir::BACKWARD) {
		forwardVector *= -1;
	}
	velocity = forwardVector;
}

// TODO:
//

void Player::destroy(ParticleEmitter deathEmitter) {
	return;
}

float Player::getNearestAltitude(Octree oct) {
	return 0;
}

glm::vec3 Player::getCollisionPoint(Octree oct) {
	return glm::vec3(0, 0, 0);
}

glm::vec3 Player::getBounceForce(glm::vec3 collisionPt) {
	return glm::vec3(0, 0, 0);
}
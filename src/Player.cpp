#pragma once
#include "Player.h"

glm::vec3 Player::heading(float len) {
	glm::vec3 dir = glm::vec3(-sin(glm::radians(rotation.y)), 0, -cos(glm::radians(rotation.y)));
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
	accel += (netForce * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion about y-axis
	rotation.y += (angularVelocity * dt);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;

	// Zero out forces.
	netForce = glm::vec3(0, 0, 0);
	cout << "Position: " << velocity << " | UpThrust?: " << isThrustingUpward << endl;
}          

void Player::addForce(Force* f) {
	forces.push_back(f);
}

void Player::updateForces() {
	// Apply forward force based on player movement direction.
	if (aDir != ms::accelDir::NONE) {
		glm::vec3 dirVector = heading(1.0f);
		if (aDir == ms::accelDir::BACKWARD) {
			dirVector *= -1;
		}
		ImpulseForce* f = new ImpulseForce(thrust, dirVector);
		addForce(f);
	}
	// Apply upward force if applicable.
	if (isThrustingUpward) {
		glm::vec3 dirVector = glm::vec3(0, 1.0f, 0);
		ImpulseForce* f = new ImpulseForce(thrust, dirVector);
		addForce(f);
	}
	// Apply rotational force based on player rotation direction.
	if (rDir != ms::rotateDir::NONE) {
		angularForce = torque;
		if (rDir == ms::rotateDir::RIGHT) {
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
   			forces[k]->updateForce(&netForce);
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
	// However, we maintain the y-velocity as it is unaffected by rotation.
	float yVelocity = velocity.y;
	float forwardVectorLength = glm::dot(velocity, headingVector);
	glm::vec3 forwardVector = heading(forwardVectorLength);
	if (aDir == ms::accelDir::BACKWARD) {
		forwardVector *= -1;
	}
	velocity.y = yVelocity;
}

void Player::removeResidualRotation() {
	if (rDir == ms::rotateDir::NONE) {
		angularAcceleration = 0;
		angularVelocity = 0;
	}
}

void Player::destroy(ParticleEmitter* deathEmitter) {
	deathEmitter->setPosition(position);
	deathEmitter->sys->reset();
	deathEmitter->start();
	isAlive = false;
	return;
}

void Player::toggleGravity(bool gravityOn) {
	hasGravity = gravityOn;
	// Apply gravity force on player.
	if (hasGravity) {
		gravityForce->set(gravity);
	}
	// No gravity.
	else {
		gravityForce->set(0);
	}
}

void Player::reset() {
	// Values
	fuel = 0;
	thrust = 0;
	torque = 0;
	restitution = 0;
	// Physics Values
	forces.clear();
	forces.push_back(gravityForce);
	angularVelocity = 0;
	angularAcceleration = 0;
	angularForce = 0;
	gravity = 1;
	// States
	aDir = ms::accelDir::NONE;
	rDir = ms::rotateDir::NONE;
	hasGravity = false;
	isThrustingUpward = false;
	isAlive = false;
	isLanded = false;
	isCollided = false;
	// Toggles
	showHeadingVector = false;
	// Particle values
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	netForce = glm::vec3(0, 0, 0);
	damping = 1;
	mass = 1;
	lifespan = 1;
	radius = 1;
	birthtime = 0;
	color = ofColor::white;
	// TransformObject values
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(0, 0, 0);
	bSelected = false;
}


// TODO:
//
float Player::getNearestAltitude(Octree oct) {
	return 0;
}

glm::vec3 Player::getCollisionPoint(Octree oct) {
	return glm::vec3(0, 0, 0);
}

glm::vec3 Player::getBounceForce(glm::vec3 collisionPt) {
	return glm::vec3(0, 0, 0);
}
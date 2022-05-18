#pragma once
#include "Player.h"

glm::vec3 Player::getDimensions() {
	glm::vec3 min = model.getSceneMin();
	glm::vec3 max = model.getSceneMax();
	return max - min;
}

glm::vec3 Player::getModelOffset() {
	glm::vec3 dim = getDimensions();
	return glm::vec3(2.5 - dim.x / 2, -dim.y / 2, -dim.z);
}

glm::vec3 Player::heading(float len) {
	glm::vec3 dir = glm::vec3(-sin(glm::radians(rotation.y)), 0, -cos(glm::radians(rotation.y)));
	return len * glm::normalize(dir);
}

void Player::drawHeadingVector() {
	if (!showHeadingVector) return;
	ofSetColor(ofColor::green);
	ofDrawLine(getCenter(), getCenter() + heading(75.0f));
	ofSetColor(ofColor::white);
}

// TODO:
bool Player::inside(glm::vec3 p) {
	return false;
}

glm::vec3 Player::getCenter() {
	glm::vec3 min = model.getSceneMin();
	glm::vec3 max = model.getSceneMax();
	return glm::vec3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2) + position;
}

glm::vec3 Player::getBottomCenter() {
	glm::vec3 min = model.getSceneMin();
	glm::vec3 max = model.getSceneMax();
	return glm::vec3((max.x + min.x) / 2, min.y, (max.z + min.z) / 2) + position;
}

Box Player::updateBoundingBox() {
	glm::vec3 min = model.getSceneMin();
	glm::vec3 max = model.getSceneMax();
	bBox = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	glm::vec3 minWorld = min + position;
	glm::vec3 maxWorld = max + position;
	bBoxWorldSpace = Box(Vector3(minWorld.x, minWorld.y, minWorld.z), Vector3(maxWorld.x, maxWorld.y, maxWorld.z));
	return bBox;
}

Box Player::drawBoundingBox() {
	ofNoFill();
	ofSetColor(ofColor::white);
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	Octree::drawBox(bBox);
	ofPopMatrix();
	return bBox;
}

void Player::move(Octree* oct, ParticleEmitter* deathEmitter) {
	// Update physics.
	updateForces();
	integrate();
	removeSideSlipping();
	removeResidualRotation();
	collide(oct, deathEmitter);
	updateBoundingBox();
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
	//cout << "Position: " << position << endl;
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
	deathEmitter->setPosition(getCenter());
	deathEmitter->sys->reset();
	deathEmitter->start();
	isAlive = false;
	// Place model outside of scene to emulate destruction.
	model.setPosition(-1000, -1000, -1000);
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

float Player::getNearestAltitude(Octree* oct) {
	glm::vec3 bottomCenter = getBottomCenter();
	Ray ray = Ray(Vector3(bottomCenter.x, bottomCenter.y, bottomCenter.z), Vector3(0, -1, 0));
	TreeNode intersectedNode;
	bool pointExists = oct->intersect(ray, oct->root, intersectedNode);
	// No terrain below; infinite altitude.
	if (!pointExists) {
		return -1;
	}
	// Get closest point.
	glm::vec3 closestPoint;
	float minDist = INT_MAX;
	for (int i = 0; i < intersectedNode.points.size(); i++) {
		glm::vec3 pt = oct->mesh.getVertex(intersectedNode.points[i]);
		float dist = bottomCenter.y - pt.y;
		if (dist < minDist) {
			minDist = dist;
			closestPoint = pt;
		}
	}
	return minDist;
}

void Player::drawAltitudeSensor() {
	if (!showAltitudeSensor) return;
	glm::vec3 bottomCenter = getBottomCenter();
	ofSetColor(ofColor::green);
	ofDrawLine(bottomCenter, bottomCenter + glm::vec3(0, -1000, 0));
	ofSetColor(ofColor::white);
}

bool Player::getBottomCollisionPoint(Octree* oct, glm::vec3& ptRtn) {
	vector<Box> boxes;
	oct->drawBox(bBoxWorldSpace);
	oct->intersect(bBoxWorldSpace, oct->root, boxes);
	glm::vec3 bottomCenter = getBottomCenter();
	glm::vec3 closestPoint;
	bool hasCollision = false;
	float minDist = INT_MAX;
	for (int i = 0; i < boxes.size(); i++) {
		float dist = bottomCenter.y - boxes[i].max().y();
		if (dist < minDist) {
			minDist = dist;
			// Get closest topmost vertex.
			closestPoint = glm::vec3(boxes[i].center().x(), boxes[i].max().y(), boxes[i].center().z());
			hasCollision = true;
		}
	}
	ptRtn = closestPoint;
	return hasCollision;
}

int Player::collide(Octree* oct, ParticleEmitter* deathEmitter) {
	// Get point of collision with terrain on model underside.
	glm::vec3 collisionPt;
	isCollided = getBottomCollisionPoint(oct, collisionPt);
	if (!isCollided) {
		return -1;
	}
	// Prevent model from going past terrain.
	float modelBottomOffset = getDimensions().y / 2;
	cout << "Terrain: " << collisionPt.y << " | Player Bottom: " << position.y << endl;
	position.y = collisionPt.y;

	// Reaction based on fall velocity.
	// Selfdestruct
	if (velocity.y < deathVelocity) {
		destroy(deathEmitter);
		return 2;
	}
	// Bounce
	else if (velocity.y < bounceVelocity) {
		glm::vec3 normal = getBounceNormal(collisionPt);
		glm::vec3 bounce = (restitution + 1.0) * ((glm::dot(-velocity, normal)) * normal);
		ImpulseForce* bounceForce = new ImpulseForce(glm::length(bounce), glm::normalize(bounce));
		addForce(bounceForce);
		velocity.y = 0;
		if (acceleration.y < 0) {
			acceleration.y = 0;
		}
		return 1;
	}
	// Stop
	else if (velocity.y < 0) {
		velocity.y = 0;
		if (acceleration.y < 0) {
			acceleration.y = 0;
		}
		return 0;
	}
	// Side-based collision or unexpected.
	return 3;
}

glm::vec3 Player::getBounceNormal(glm::vec3 collisionPt) {
	return glm::vec3(0, -velocity.y, 0);
}
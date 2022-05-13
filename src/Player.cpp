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

bool Player::inside(glm::vec3 p) {

}

void Player::integrate() {

}

void Player::addForce() {

}

void Player::updateForces() {

}

void Player::removeSideSlipping() {

}

void Player::destroy(ParticleEmitter deathEmitter) {

}

float Player::getNearestAltitude(Octree oct) {

}

glm::vec3 Player::getCollisionPoint(Octree oct) {

}

glm::vec3 Player::getBounceForce(glm::vec3 collisionPt) {

}
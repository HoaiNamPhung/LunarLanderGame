#pragma once
#include "TransformObject.h"

//  Base class for any object that needs a transform.
//
TransformObject::TransformObject() {
	position = ofVec3f(0, 0, 0);
	scale = ofVec3f(1, 1, 1);
	rotation = ofVec3f(0, 0, 0);
}

void TransformObject::setPosition(const glm::vec3& pos) {
	position = pos;
}

void TransformObject::setRotation(const glm::vec3& rot) {
	rotation = rot;
}

void TransformObject::setYRotation(const float& yRot) {
	rotation = glm::vec3(0, yRot, 0);
}

void TransformObject::setScale(const glm::vec3& sca) {
	scale = sca;
}

void TransformObject::setScale(const float& sca) {
	scale = glm::vec3(sca, sca, sca);
}

// Retrieves matrix generated from current shape's tranformations.
glm::mat4 TransformObject::getMatrix() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(position));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation).y, glm::vec3(0, 1, 0));	// Only rotates on y-axis; assumed that only Player will rotate.
	glm::mat4 sca = glm::scale(glm::mat4(1.0), glm::vec3(scale));
	glm::mat4 mat = trans * sca * rot;
	if (parent) mat = parent->getMatrix() * mat; // Recursive transformation heirarchy.
	return mat;
}

glm::mat4 TransformObject::getMatrixNoRotation() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(position));
	glm::mat4 sca = glm::scale(glm::mat4(1.0), glm::vec3(scale));
	glm::mat4 mat = trans * sca;
	if (parent) mat = parent->getMatrix() * mat; // Recursive transformation heirarchy.
	return mat;
}
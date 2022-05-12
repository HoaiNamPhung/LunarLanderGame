#pragma once
#include "TransformObject.h"

//  Base class for any object that needs a transform.
//
TransformObject::TransformObject() {
	position = ofVec3f(0, 0, 0);
	scale = ofVec3f(1, 1, 1);
	rotation = ofVec3f(0, 0, 0);
}

void TransformObject::setPosition(const ofVec3f & pos) {
	position = pos;
}

void TransformObject::setRotation(const ofVec3f& rot) {
	rotation = rot;
}

void TransformObject::setScale(const ofVec3f& sca) {
	scale = sca;
}


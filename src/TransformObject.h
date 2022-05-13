#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

//  Base class for any object that needs a transform.
//
class TransformObject {
public:
	TransformObject();
	TransformObject* parent = NULL;
	glm::vec3 position, rotation, scale;
	bool bSelected;
	void setPosition(const glm::vec3 &);
	void setRotation(const glm::vec3 &);
	void setYRotation(const float &);
	void setScale(const glm::vec3 &);
	glm::mat4 getMatrix();
};
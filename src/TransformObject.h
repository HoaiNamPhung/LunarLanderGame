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
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(0, 0, 0);
	bool bSelected = false;
	void setPosition(const glm::vec3 &);
	void setRotation(const glm::vec3 &);
	void setYRotation(const float &);
	void setScale(const glm::vec3 &);
	void setScale(const float &);
	glm::mat4 getMatrix();
};
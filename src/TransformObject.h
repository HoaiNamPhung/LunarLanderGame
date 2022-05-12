#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

//  Base class for any object that needs a transform.
//
class TransformObject {
public:
	TransformObject();
	ofVec3f position, rotation, scale;
	bool	bSelected;
	void setPosition(const ofVec3f &);
	void setRotation(const ofVec3f&);
	void setScale(const ofVec3f&);
};
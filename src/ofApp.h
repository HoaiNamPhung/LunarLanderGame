#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>
#include "Camera.h"
#include "Player.h"


class LandingAreas {
public:
	vector<Box> areas;
	vector<Vector3> points;
	float landDuration = 5;
	float currLandDuration = 0;
	float lastUpdateTime = 0;

	LandingAreas() {
		float height = 2.5;
		float width = 2.5;
		//first area
		Vector3 point1 = Vector3(-1, 0, -1);
		Vector3 min1 = Vector3(point1.x() - width / 2, point1.y() - height / 2, point1.z() - width / 2);
		Vector3 max1 = Vector3(point1.x() + width / 2, point1.y() + height / 2, point1.z() + width / 2);
		Box box1 = Box(min1, max1);
		areas.push_back(box1);
		//second area
		Vector3 point2 = Vector3(10, 0, 8);
		Vector3 min2 = Vector3(point2.x() - width / 2, point2.y() - height / 2, point2.z() - width / 2);
		Vector3 max2 = Vector3(point2.x() + width / 2, point2.y() + height / 2, point2.z() + width / 2);
		Box box2 = Box(min2, max2);
		areas.push_back(box2);
		//second area
		Vector3 point3 = Vector3(-11, 9, 5);
		Vector3 min3 = Vector3(point3.x() - width / 2, point3.y() - height / 2, point3.z() - width / 2);
		Vector3 max3 = Vector3(point3.x() + width / 2, point3.y() + height / 2, point3.z() + width / 2);
		Box box3 = Box(min3, max3);
		areas.push_back(box3);
	}	
	void draw() {
		for (int i = 0; i < areas.size(); i++) {
			ofNoFill();
			ofSetColor(ofColor::white);
			Octree::drawBox(areas[i]);
		}
	}

	bool updateCheckLand(Player* p) {
		Vector3 position = Vector3(p->position.x, p->position.y, p->position.z);
		for (int i = 0; i < areas.size(); i++) {
			if (areas[i].inside(position)) {
				if (p->velocity.x < 0.5 && p->velocity.x > -0.5 &&
					p->velocity.y < 0.5 && p->velocity.y > -0.5 &&
					p->velocity.z < 0.5 && p->velocity.z > -0.5) {
					currLandDuration += ofGetElapsedTimef() - lastUpdateTime;
					lastUpdateTime = ofGetElapsedTimef();
					return true;
				}
					
			}
		}
		currLandDuration = 0;
		return false;
	}


};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawShader(ofVbo* vbo, ParticleEmitter* emitter, ofShader* shader, ofTexture* particleTex, ofColor color);
		void loadVbo(ParticleEmitter* emitter, ofVbo* vbo);
		void reset();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);
		void randomizeSpawnPosition();

		//Cameras
		ofEasyCam* cam;
		ofCamera* fixed; 
		ofCamera* playerCam; 
		ofCamera* chooseCamera;


		//Lights
		ofLight keyLight, fillLight, rimLight, spotlight;

		/*ofxFloatSlider fillScale;
		ofxFloatSlider rimScale;
		ofxVec3Slider keyPos;
		ofxVec3Slider fillPos;
		ofxVec3Slider rimPos;
		int prevKeyXRot;
		int prevKeyYRot;
		ofxIntSlider keyXRot;
		ofxIntSlider keyYRot;
		int prevFillXRot;
		int prevFillYRot;
		ofxIntSlider fillXRot;
		ofxIntSlider fillYRot;
		int prevRimYRot;
		ofxIntSlider rimYRot;
		ofxVec3Slider keyAmbientColor;
		ofxVec3Slider fillAmbientColor;
		ofxVec3Slider rimAmbientColor;
		ofxVec3Slider keyDiffuseColor;
		ofxVec3Slider fillDiffuseColor;
		ofxVec3Slider rimDiffuseColor;
		ofxVec3Slider keySpecularColor;
		ofxVec3Slider fillSpecularColor;
		ofxVec3Slider rimSpecularColor;*/

		//background image
		ofImage background;

		ofxAssimpModelLoader moon;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;

		/* GUI */
		ofxPanel gui;
		// Octree
		ofxIntSlider numLevels;
		// Player
		Player* player;
		ParticleSystem* sys;
		ofxFloatSlider scale;
		ofxFloatSlider thrust;
		ofxFloatSlider thrustEmitterVelocityScale = 5;
		ofxFloatSlider thrustEmitterParticleRadius = 0.3;
		ofxFloatSlider torque;
		ofxFloatSlider restitution;
		ofxFloatSlider gravity;
		ofxIntSlider maxFuel;
		// Toggles
		ofxToggle headingVectorToggle;
		ofxToggle altitudeSensorToggle;
		ofxToggle boundingBoxToggle;
		ofxToggle requireFuelToggle;
		// Emitters
		ofxFloatSlider thrustRate;
		ofxIntSlider turbScale = 10;
		ofxFloatSlider deathParticleSize = 0.3f;
		ofxFloatSlider thrustParticleSize = 0.1f;
		TurbulenceForce* turbulenceForce;
		ImpulseRadialForce* radialForce;
		ParticleEmitter* thrustEmitter;
		ParticleEmitter* deathEmitter;
		// SFX
		ofSoundPlayer bgm;
		ofSoundPlayer thrustSfx;
		ofSoundPlayer rotateSfx;
		ofSoundPlayer deathSfx;
		ofSoundPlayer bounceSfx;
		ofSoundPlayer victorySfx;
		ofxFloatSlider bgmVolume = 0.5f;
		ofxFloatSlider sfxVolume = 0.5f;
		ofxFloatSlider masterVolume = 0.5f;
		// Shader
		ofTexture particleTex;
		ofTexture deathParticleTex;
		ofShader shader;
		ofVbo vboThrust, vboDeath;
		// Values
		bool gameWon = false;
		bool gameLost = false;
		float spawnHeight = 30;
		glm::vec3 spawnPos = glm::vec3(0, spawnHeight, 0);
		float altitude = 0;
		ofxVec3Slider positionSlider;
		ofxVec3Slider velocitySlider;

		// States
		Camera::mode cameraState = Camera::mode::FIXED;
		bool bAltKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		bool bLanderLoaded;
		bool bTerrainSelected;
		bool bPaused = true;
		// Mouse selection
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;
		bool isLanding;
		vector<Box> bboxList;


		//LandingAreas
		LandingAreas* landing;

		const float selectionRange = 4.0;
};

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>
#include "Camera.h"
#include "Player.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void loadVbo(ParticleEmitter* emitter, ofVbo vbo);
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
		void toggleAltitudeSensor();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		ofEasyCam cam;
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
		// Shader
		ofTexture particleTex;
		ofShader shader;
		ofVbo vboUpward, vboForward, vboDeath;

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

		vector<Box> bboxList;

		const float selectionRange = 4.0;
};

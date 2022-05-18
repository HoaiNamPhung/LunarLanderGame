
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	background.load("geo/background.jpg");
	// Preload shader textures
	ofDisableArbTex();
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}
	#ifdef TARGET_OPENGLES
		shader.load("shaders_gles/shader");
	#else
		shader.load("shaders/shader");
	#endif

	// Camera
	//easycam
	cam = new ofEasyCam();
	fixed = new ofCamera();
	playerCam = new ofCamera();
	cam->setDistance(10);
	cam->setNearClip(.1);
	cam->setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	cam->disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	



	// Lighting

	initLightingAndMaterials();

	// Load models
	moon.loadModel("geo/terrain/terrain.dae");
	moon.setScaleNormalization(false);

	// GUI Sliders
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(scale.setup("Player Scale", 1, 1, 10));
	gui.add(thrust.setup("Player Thrust", 5, 1, 50));
	gui.add(torque.setup("Player Torque", 50, 1, 300));
	gui.add(restitution.setup("Player Restitution", 1, 1, 10));
	gui.add(gravity.setup("Gravity", 2, 0, 10));
	gui.add(maxFuel.setup("Max Fuel", 1, 1, 10));
	gui.add(thrustRate.setup("Thrust Rate", 0.05, 0.01, 1));
	gui.add(turbScale.setup("Turbulence Scale", 10, 0, 100));
	gui.add(headingVectorToggle.setup("Show Heading Vector", false));
	gui.add(altitudeSensorToggle.setup("Show Altitude Sensor", false));
	gui.add(boundingBoxToggle.setup("Show Bounding Box", false));
	bHide = false;

	// Player
	player = new Player();
	player->position = glm::vec3(0, 0, 0);
	player->radius = 1.0;
	player->lifespan = 100000;
	player->isAlive = true;
	player->gravity = gravity;
	player->toggleGravity(true);
	// Player model
	player->model.loadModel("geo/submarine.dae");
	bLanderLoaded = true;
	player->model.setScaleNormalization(false);
	player->model.setPosition(0, 0, 0);
	bboxList.clear();
	for (int i = 0; i < player->model.getMeshCount(); i++) {
		bboxList.push_back(Octree::meshBounds(player->model.getMesh(i)));
	}


	//fixed cam
	fixed->setFov(60);
	fixed->setPosition(10, 40, 10);
	fixed->lookAt(player->position);


	//player cam
	playerCam->setFov(60);
	playerCam->setPosition(player->position.x - 10, player->position.y + 10, player->position.z);
	playerCam->lookAt(player->position);
	

	//set default cam
	chooseCamera = cam;

	// Set up bounding box.
	player->updateBoundingBox();

	/* Particle Emitters */
	turbulenceForce = new TurbulenceForce(glm::vec3((int)-turbScale, (int)-turbScale, (int)-turbScale), glm::vec3((int)turbScale, (int)turbScale, (int)turbScale));
	radialForce = new ImpulseRadialForce(2000);
	// Thrusters
	glm::vec3 thrusterPos = player->position;
	thrustEmitter = new ParticleEmitter();
	thrustEmitter->setEmitterType(DirectionalEmitter);
	thrustEmitter->setPosition(thrusterPos);
	thrustEmitter->setRate(thrustRate);
	thrustEmitter->setVelocity(-1 * glm::vec3(player->velocity.x, player->velocity.y, player->velocity.z));
	thrustEmitter->setLifespan(0.5);
	thrustEmitter->setParticleRadius(0.4);
	thrustEmitter->sys->addForce(turbulenceForce);
	// Death
	deathEmitter = new ParticleEmitter();
	deathEmitter->setOneShot(true);
	deathEmitter->setEmitterType(RadialEmitter);
	deathEmitter->setVelocity(ofVec3f(0, 0, 0));
	deathEmitter->setGroupSize(1000);
	deathEmitter->setRate(0);
	deathEmitter->setLifespan(2);
	deathEmitter->setParticleRadius(1);
	deathEmitter->sys->addForce(radialForce);
	deathEmitter->sys->addForce(turbulenceForce);

	// Create Octree for testing and time how long it took to build the tree.
	ofResetElapsedTimeCounter();
	octree.create(moon.getMesh(0), 20);
	cout << "Tree creation time: " << ofGetElapsedTimeMillis() << " ms" << endl;
	cout << "Number of Verts: " << moon.getMesh(0).getNumVertices() << endl;
	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));
}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	// Update slider values.
	player->setScale(scale);
	player->thrust = thrust;
	player->torque = torque;
	player->restitution = restitution;
	player->gravity = gravity;
	player->toggleGravity(true);
	player->showHeadingVector = headingVectorToggle;
	player->showAltitudeSensor = altitudeSensorToggle;
	thrustEmitter->setRate(thrustRate);

	// Update physics if game is running.
	if (!bPaused) {
		// Player states/movement.
		player->move();
		//camera tracking
		playerCam->setPosition(player->position.x - 10, player->position.y + 10, player->position.z);
		cout << "Rotation:" << player->rotation << endl;
		playerCam->rotateAround(player->rotation.y + 90, glm::vec3(0,1,0), player->position);
		playerCam->lookAt(player->position);
		fixed->lookAt(player->position);
		// Particle system movement.
		thrustEmitter->update();
		deathEmitter->update();
		// Adjust position of objects attached to player.
		thrustEmitter->setPosition(player->position);
		thrustEmitter->setVelocity(-1 * glm::vec3(player->velocity.x, (float) thrustEmitterVelocityScale, player->velocity.z));
	}
}
//--------------------------------------------------------------
void ofApp::draw() {
	
	//ofBackground(ofColor::black);
	loadVbo(deathEmitter, vboDeath);
	
	
	
	glDepthMask(GL_FALSE);
	ofSetColor(ofColor::white);
	background.draw(ofGetWindowRect());
	glDepthMask(GL_TRUE);

	chooseCamera->begin();
	
	ofPushMatrix();
	
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		moon.drawWireframe();
		if (bLanderLoaded) {
			player->model.drawWireframe();
			if (!bTerrainSelected) drawAxis(player->model.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		moon.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			player->model.drawFaces();
			if (!bTerrainSelected) drawAxis(player->model.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < player->model.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(player->model.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {
				// draw colliding boxes on lander bounding box.
				Box bounds = player->drawBoundingBox();
				ofSetColor(ofColor::lightCoral);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		moon.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - chooseCamera->getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}

	// Draw player attachments.
	if (player->isAlive && !bPaused) {
		// Draw particle emitters.
		thrustEmitter->draw();
		deathEmitter->draw();
		// Draw directional vectors.
		player->drawHeadingVector();
		player->drawAltitudeSensor();
		if (boundingBoxToggle) {
			player->drawBoundingBox();
		}
		// Shader based emitters.
		glDepthMask(GL_FALSE);
		ofSetColor(255, 100, 90);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofEnablePointSprites();
		shader.begin();
		chooseCamera->begin();
		particleTex.bind();
		vboDeath.draw(GL_POINTS, 0, (int)deathEmitter->sys->particles.size());
		particleTex.unbind();
		chooseCamera->end();
		shader.end();
		ofDisablePointSprites();
		ofDisableBlendMode();
		ofEnableAlphaBlending();
		glDepthMask(GL_TRUE);
	}
	
	ofPopMatrix();
	chooseCamera->end();
	

	// Draw HUD
	if (bPaused) {
		ofDrawBitmapStringHighlight("Paused.", ofGetWindowWidth() - 100, ofGetWindowHeight() - 25);
	}
	else {
		// Status: Bottom Right
		ofSetColor(ofColor::white);
		ofDrawBitmapStringHighlight("Running.", ofGetWindowWidth() - 100, ofGetWindowHeight() - 25);
		// Gauges: Top Right
		ofSetColor(ofColor::green);
		//float altitude = player->getNearestAltitude(octree);
		//ofDrawBitmapStringHighlight("ESTIMATED " + std::to_string(altitude) + "M", ofGetWindowWidth() - 100, 25);
		ofDrawBitmapStringHighlight(std::to_string(ofGetFrameRate()) + " FPS", ofGetWindowWidth() - 100, 50);
		// Controls: Bottom Left
		ofSetColor(ofColor::yellow);
		ofDrawBitmapStringHighlight("Thrust Upward: Space", 100, ofGetWindowHeight() - 125);
		ofDrawBitmapStringHighlight("Thrust Forward/Backward: W/S", 100, ofGetWindowHeight() - 100);
		ofDrawBitmapStringHighlight("Rotate Left/Right: A/D", 100, ofGetWindowHeight() - 75);
		ofDrawBitmapStringHighlight("Toggle Mouse Controls: C", 100, ofGetWindowHeight() - 50);
		ofDrawBitmapStringHighlight("Restart/Play: CTRL", 100, ofGetWindowHeight() - 25);
		// GUI reserved for Top Left
		ofSetColor(ofColor::white);
	}

	// Draw GUI
	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadVbo(ParticleEmitter* emitter, ofVbo vbo) {
	if (emitter->sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < emitter->sys->particles.size(); i++) {
		points.push_back(emitter->sys->particles[i].position);
		sizes.push_back(ofVec3f(emitter->radius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

// Resets the game state.
//
void ofApp::reset() {
	player->reset();
	player->position = glm::vec3(0, 0, 0);
	player->radius = 1.0;
	player->lifespan = 100000;
	player->isAlive = true;
	player->gravity = gravity;
	player->toggleGravity(true);
	player->model.setPosition(0, 0, 0);
	thrustEmitter->stop();
	ofResetElapsedTimeCounter();
}

// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case '1':
		chooseCamera = fixed;
		break;
	case '2':
		chooseCamera = playerCam;
		break;
	case '3':
		chooseCamera = cam;
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'V':
		break;
	case OF_KEY_F1:
		cameraState = Camera::mode::FIXED;
		break;
	case OF_KEY_F2:
		cameraState = Camera::mode::ONBOARD;
		break;
	case OF_KEY_F3:
		cameraState = Camera::mode::EASYCAM;
		break;
	case OF_KEY_F4:
		cameraState = Camera::mode::EASYCAM;
		cam->reset();
		break;
	case 'C':
	case 'c':
	case OF_KEY_F5:
		if (cam->getMouseInputEnabled()) cam->disableMouseInput();
		else cam->enableMouseInput();
		break;
	case OF_KEY_F6:
		setCameraTarget();
		break;
	case OF_KEY_F7:
		bDisplayOctree = !bDisplayOctree;
		break;
	case OF_KEY_F8:
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case OF_KEY_F9:
		toggleWireframeMode();
		break;
	case OF_KEY_F10:
		togglePointsDisplay();
		break;
	case OF_KEY_F11:
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case OF_KEY_F12:
		toggleAltitudeSensor();
		break;
	case OF_KEY_ALT:
		cam->enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bPaused = !bPaused;
		if (bPaused) {
			reset();
		}
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_LEFT:   // turn left
	case 'a':
	case 'A':
		player->rDir = ms::rotateDir::LEFT;
		break;
	case OF_KEY_RIGHT:  // turn right
	case 'd':
	case 'D':
		player->rDir = ms::rotateDir::RIGHT;
		break;
	case OF_KEY_UP:     // go forward
	case 'w':
	case 'W':
		if (!player->isAlive || bPaused) break;
		if (!player->isThrustingUpward) {
			thrustEmitter->sys->reset();
			thrustEmitter->start();
		}
		player->aDir = ms::accelDir::FORWARD;
		break;
	case OF_KEY_DOWN:   // go backward
	case 's':
	case 'S':
		if (!player->isThrustingUpward) {
			thrustEmitter->sys->reset();
			thrustEmitter->start();
		}
		player->aDir = ms::accelDir::BACKWARD;
		break;
	case ' ':
		if (!player->isAlive || bPaused) break;
		if (player->aDir == ms::accelDir::NONE) {
			thrustEmitter->sys->reset();
			thrustEmitter->start();
		}
		player->isThrustingUpward = true;
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_ALT:
		cam->disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_LEFT:   // turn left
	case 'a':
	case 'A':
		player->rDir = ms::rotateDir::NONE;
		break;
	case OF_KEY_RIGHT:  // turn right
	case 'd':
	case 'D':
		player->rDir = ms::rotateDir::NONE;
		break;
	case OF_KEY_UP:     // go forward
	case 'w':
	case 'W':
		player->aDir = ms::accelDir::NONE;
		if (!player->isThrustingUpward) {
			thrustEmitter->stop();
		}
		break;
	case OF_KEY_DOWN:   // go backward
	case 's':
	case 'S':
		player->aDir = ms::accelDir::NONE;
		if (!player->isThrustingUpward) {
			thrustEmitter->stop();
		}
		break;
	case ' ':
		player->isThrustingUpward = false;
		if (player->aDir == ms::accelDir::NONE) {
			thrustEmitter->stop();
		}
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam->getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = chooseCamera->getPosition();
		glm::vec3 mouseWorld = chooseCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = player->model.getSceneMin() + player->model.getPosition();
		ofVec3f max = player->model.getSceneMax() + player->model.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(player->model.getPosition(), chooseCamera->getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = chooseCamera->screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - chooseCamera->getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	// Time ray intersection runtime.
	ofResetElapsedTimeCounter();
	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	//cout << "Tree ray intersection time: " << ofGetElapsedTimeMillis() << " ms" << endl;

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
		cout << "Index: " << std::to_string(selectedNode.points[0]) << endl;

		// TODO: Explosion test, remove later.
		deathEmitter->setPosition(pointRet);
		deathEmitter->sys->reset();
		deathEmitter->start();
	}
	return pointSelected;
}

// TODO:
void ofApp::toggleAltitudeSensor() {
	player->getNearestAltitude(octree);
	return;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam->getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = player->model.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, chooseCamera->getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		player->model.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		Box bounds = player->bBox;
		colBoxList.clear();
		// Time box intersection runtime.
		ofResetElapsedTimeCounter();
		octree.intersect(bounds, octree.root, colBoxList);
		//cout << "Tree box intersection time: " << ofGetElapsedTimeMillis() << " ms" << endl;
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		}*/


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (bInDrag == true) {
		/*
		cout << "Boxes overlapped | ";
		for (Box box : colBoxList) {
			cout << box.center().x() << ", " << box.center().y() << ", " << box.center().z() << " | ";
		}
		cout << endl;
		*/
	}
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), chooseCamera->getZAxis(), point);
	if (player->model.loadModel(dragInfo.files[0])) {
		player->model.setScaleNormalization(false);
	//	player->model.setScale(.1, .1, .1);
	//	player->model.setPosition(point.x, point.y, point.z);
		player->model.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < player->model.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(player->model.getMesh(i)));
		}

		cout << "Mesh Count: " << player->model.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = chooseCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - chooseCamera->getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (player->model.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		player->model.setScaleNormalization(false);
		player->model.setPosition(0, 0, 0);
		cout << "number of meshes: " << player->model.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < player->model.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(player->model.getMesh(i)));
		}

		//		player->model.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = chooseCamera->getPosition();
		glm::vec3 camAxis = chooseCamera->getZAxis();
		glm::vec3 mouseWorld = chooseCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = player->model.getSceneMin();
			glm::vec3 max = player->model.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			player->model.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);
			player->updateBoundingBox();
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = chooseCamera->getPosition();
	glm::vec3 camAxis = chooseCamera->getZAxis();
	glm::vec3 mouseWorld = chooseCamera->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

#include "Particle.h"
#include "MoveState.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include "Force.h"
#include "ofxAssimpModelLoader.h"

class Player : public Particle {
public:
	Player() {}
	// Drawables
	ofxAssimpModelLoader model;
	// Values
	float fuel;
	float thrust;
	float torque;
	float restitution;
	// Physics Values
	vector<Force*> forces;
	float angularVelocity;
	float angularAcceleration;
	float angularForce;
	// States
	ms::accelDir aDir = ms::accelDir::NONE;
	ms::rotateDir rDir = ms::rotateDir::NONE;
	bool isThrustingUpward = false;
	bool isAlive = false;
	bool isLanded;
	bool isCollided;
	// Toggles
	bool showHeadingVector;

	// Gets the heading vector of the player, which rotates about player's y-axis.
	glm::vec3 heading(float len);
	// Draws a header vector pointing out from player direction.
	void drawHeadingVector();
	// Converts point to player object space and checks insideness.
	bool inside(glm::vec3 p);
	// Integrates forces on the player; updates linear and angular motion.
	void integrate();
	// Applies a force on the player.
	void addForce(Force* f);
	// Updates all existing forces on the player.
	void updateForces();
	// Removes residual horizontal side slipping from velocity of previous rotation.
	void removeSideSlipping();
	// Undraws the player and removes it from the scene. Emits a particle explosion to indicate destruction.
	void destroy(ParticleEmitter* deathEmitter);
	// Returns the distance between the lander and the closest terrain point directly below it.
	float getNearestAltitude(Octree oct);
	// Get the closest terrain point that collides with the lander.
	glm::vec3 getCollisionPoint(Octree oct);
	// Retrieves the bounce force of the player given a collided point.
	glm::vec3 getBounceForce(glm::vec3 collisionPt);



};
#include "Particle.h"
#include "MoveState.h"
#include "Octree.h"
#include "ParticleEmitter.h"
#include "Force.h"
#include "ofxAssimpModelLoader.h"

class Player : public Particle {
public:
	Player::Player() {
		addForce(gravityForce);
	}
	// Drawables
	ofxAssimpModelLoader model;
	Box bBox;
	Box bBoxWorldSpace;
	// Values
	int maxFuel = 0;
	float fuel = 0;
	float thrust = 0;
	float torque = 0;
	float restitution = 0;
	// Physics Values
	vector<Force*> forces;
	float angularVelocity = 0;
	float angularAcceleration = 0;
	float angularForce = 0;
	float gravity = 1;
	GForce* gravityForce = new GForce(gravity);
	float bounceVelocity = -1;
	float deathVelocity = -5;
	float lastUpdateTime = 0;
	// States
	ms::accelDir aDir = ms::accelDir::NONE;
	ms::rotateDir rDir = ms::rotateDir::NONE;
	bool hasGravity = false;
	bool isThrustingUpward = false;
	bool isAlive = false;
	bool isLanded = false;
	bool isCollided = false;
	// Toggles
	bool requireFuel = false;
	bool showHeadingVector = false;
	bool showAltitudeSensor = false;

	// Gets vector containing x, y, z widths of model.
	glm::vec3 getDimensions();
	// Gets the heading vector of the player, which rotates about player's y-axis.
	glm::vec3 heading(float len);
	// Draws a header vector pointing out from player direction.
	void drawHeadingVector();
	// Updates the bounding box position to physics position, then retrieves it.
	Box updateBoundingBox();
	// Draws and retrieves the bounding box.
	Box drawBoundingBox();
	// Gets center of model.
	glm::vec3 getCenter();
	// Gets center-bottom of model.
	glm::vec3 getBottomCenter();
	// Converts point to player object space and checks insideness.
	bool inside(glm::vec3 p);
	// Runs all per-frame physics calculations and moves the player model accordingly.
	void move(Octree* oct, ParticleEmitter* deathEmitter);
	// Integrates forces on the player; updates linear and angular motion.
	void integrate();
	// Applies a force on the player.
	void addForce(Force* f);
	// Updates all existing forces on the player.
	void updateForces();
	// Removes residual horizontal side slipping from velocity of previous rotation.
	void removeSideSlipping();
	// Removes residual rotation so that current rotation is maintained when there is no rotational input.
	void removeResidualRotation();
	// Undraws the player and removes it from the scene. Emits a particle explosion to indicate destruction.
	void destroy(ParticleEmitter* deathEmitter);
	// Returns the distance between the lander and the closest terrain point directly below it.
	float getNearestAltitude(Octree* oct);
	// Draws the altitude sensor ray on terrain.
	void drawAltitudeSensor();
	// Get the closest terrain point that collides with the bottom of the lander and places it in ptRtn. 
	// Returns true if collided point exists.
	bool getBottomCollisionPoint(Octree* oct, glm::vec3& ptRtn);
	// Retrieves the bounce normal of the player given a collided point.
	glm::vec3 getBounceNormal(glm::vec3 collisionPt);
	// Checks for bottom collision, then assures player either stops, bounces, or destructs based on that collision's velocity.
	// Returns: -1 = no collision; 0 = stop; 1 = bounce; 2 = selfdestruct; 3 = unexpected/side-based collision.
	int collide(Octree* oct, ParticleEmitter* deathEmitter);
	// Turns gravity on/off.
	void toggleGravity(bool gravityOn);
	// Resets player state to defaults, except parent TransformObject and model.
	void reset();



};
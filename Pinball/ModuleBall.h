#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
struct b2Vec2;
class ModuleBall : public Module
{
public:
	ModuleBall(Application* app, bool start_enabled = true);
	~ModuleBall();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	PhysBody* ball;
	bool debug;
	PhysBody* sensor;
	bool sensed;
	SDL_Texture* ballTexture;
	SDL_Texture* supraTex;
	uint collisionFx;
	uint bounceFx;

private:
	// Start functions
	bool LoadAssets();

	// Draw functions
	void DrawSupra();
	void DrawBalls();

	// Logic functions
	void CapBallVel();

	// Debug functions
	void DebugAndInput();
	void LogBall();

private:
	iPoint mouse;
	int ray_hit;
	fVector normal = { 0.0f, 0.0f };
	p2Point<int> ray;
	bool ray_on;

	void PreRayCast();
	void PostRayCast();

public:
	bool dead = false;
	bool lost = false;

	void RestartGameScore();
	uint lives = 3;

	bool isBounce = false;
	int bounceTimer = 0;
	void BounceLogic();
	void ResetBallState();

};
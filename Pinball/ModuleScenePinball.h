#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class b2RevoluteJoint;
class PhysBody;

class ModuleScenePinball : public Module
{
public:
	ModuleScenePinball(Application* app, bool start_enabled = true);
	~ModuleScenePinball();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
public:
	p2List<PhysBody*> bounces;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> chains;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* bounce;
	SDL_Texture* box;
	SDL_Texture* chain;
	SDL_Texture* infraTex;

	uint bonus_fx;
	


private:
	bool principalMap = false;
	bool specialLeftNet = false;
	bool specialRightKicker = true;  // True at start
	bool specialRightZigZag = false;
	bool specialCenterTwirl = false;
	bool specialLeftToRight = false;

private:
	bool LoadAssets();
	void LoadMap();
	void DrawBounces();
	void DrawChains();
	void DrawInfra();
	void DebugCreate();

private: // Raycast field
	iPoint mouse;
	int ray_hit;
	fVector normal = { 0.0f, 0.0f };
	p2Point<int> ray;
	bool ray_on;

	void PreRayCast();
	void PostRayCast();

public:
	b2RevoluteJoint* revJoint;
};

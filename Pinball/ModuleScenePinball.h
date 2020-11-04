#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"


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
	void BlitAll();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* backgroundTex;

	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;


private:
	bool principalMap = false;
	bool specialLeftNet = false;
	bool specialRightKicker = true;  // True at start
	bool specialRightZigZag = false;
	bool specialCenterTwirl = false;
	bool specialLeftToRight = false;
};

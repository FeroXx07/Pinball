#pragma once
#include "Module.h"
#include "p2List.h"
#include "Globals.h"
#include "p2List.h"

class PhysBody;
class b2PointerHouse;

class ModuleScenePinball : public Module
{
public:
	ModuleScenePinball(Application* app, bool start_enabled = true);
	~ModuleScenePinball();

	bool Start();
	update_status Update();
	bool CleanUp();
	void BlitAll();

public:
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* backgroundTex;

	p2List<b2PointerHouse*>circles;
	p2List<b2PointerHouse*>rects;
	p2List<b2PointerHouse*>ricks;
};

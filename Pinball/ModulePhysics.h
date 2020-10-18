#pragma once
#include "Module.h"
#include "Globals.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

//#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
//#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

class b2World;
class b2Body;

// TODO 6: Create a small class that keeps a pointer to the b2Body
// and has a method to request the position
// then write the implementation in the .cpp
// Then make your circle creation function to return a pointer to that class
class b2PointerHouse
{
public:
	b2PointerHouse();
	b2PointerHouse(b2Body* b);

	void GetPosition(int& x, int& y) const;
	float GetRotation()const;
	void GetRadius(int& r)const;

	b2Body* bodyPointer;
	float radius = 0; // or bodyPointer->GetFixtureList()->GetShape()->m_radius
	int h = 0, w = 0;
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	// TODO 4: Move body creation to 3 functions to create circles, rectangles and chains
	b2PointerHouse* CreatCircle(int x, int y, int radius_);
	b2PointerHouse* CreateRectangle(int x, int y, int w, int h);
	b2PointerHouse* CreateChain(int x, int y);
private:

	bool debug;
	b2World* world;
};
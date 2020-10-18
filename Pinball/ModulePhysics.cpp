#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

#include "Box2D/Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

b2PointerHouse::b2PointerHouse() :bodyPointer(NULL) {};
b2PointerHouse::b2PointerHouse(b2Body* b) :bodyPointer(b) {};

void b2PointerHouse::GetPosition(int& x, int& y)const
{
	x =	METERS_TO_PIXELS(bodyPointer->GetPosition().x);
	y = METERS_TO_PIXELS(bodyPointer->GetPosition().y);
}

float b2PointerHouse::GetRotation()const
{
	float a= RADTODEG * this->bodyPointer->GetAngle();
	return a;
}

void b2PointerHouse::GetRadius(int& r)const
{
	r = METERS_TO_PIXELS(bodyPointer->GetFixtureList()->GetShape()->m_radius);
}

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

	//// big static circle as "ground" in the middle of the screen
	//int x = SCREEN_WIDTH / 2;
	//int y = SCREEN_HEIGHT / 1.5f;
	//int diameter = SCREEN_WIDTH / 2;

	//b2BodyDef body;
	//body.type = b2_staticBody;
	//body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	//b2Body* b = world->CreateBody(&body);

	//b2CircleShape shape;
	//shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	//b2FixtureDef fixture;
	//fixture.shape = &shape;
	//b->CreateFixture(&fixture);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}


b2PointerHouse* ModulePhysics::CreatCircle(int x, int y, int radius_)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	float radius = PIXEL_TO_METERS(radius_);
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	
	b2CircleShape shape;
	shape.m_radius = radius;
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);
	b2PointerHouse* p = new b2PointerHouse;
	p->bodyPointer = b;
	p->radius = radius_;
	return p;
}

b2PointerHouse *ModulePhysics::CreateRectangle(int x, int y, int w, int h)
{
	// TODO 1: When pressing 2, create a box on the mouse position
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(w), PIXEL_TO_METERS(h)); // This would be (1m,0.5m)	
	b2FixtureDef fixture;
	fixture.shape = &shape;
	// TODO 2: To have the box behave normally, set fixture's density to 1.0f
	fixture.density = 1.0f;
	
	b->CreateFixture(&fixture);
	b2PointerHouse* p = new b2PointerHouse;
	p->bodyPointer = b;
	p->w = w;
	p->h = h;
	return p;
}


b2PointerHouse* ModulePhysics::CreateChain(int x, int y)
{
	// TODO 3: Create a chain shape using those vertices
	// remember to convert them from pixels to meters!
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	int points[68] = {};

	b2Vec2 vs[68];
	for (int i = 0; i < 68 / 2; ++i) // i = VERTICES / 2
	{
		vs[i].Set(PIXEL_TO_METERS(points[2 * i]), PIXEL_TO_METERS(points[2 * i + 1]));
	}

	b2ChainShape chain;
	chain.CreateLoop(vs, 68 / 2);

	b2FixtureDef fixture;
	fixture.shape = &chain;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	b2PointerHouse* p = new b2PointerHouse;
	p->bodyPointer = b;
	return p;
}
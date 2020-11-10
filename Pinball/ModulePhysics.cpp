#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	debug = true;
	mouseJoint = NULL;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

	world->SetContactListener(this);
	

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	// TODO 4: add a pointer to PhysBody as UserData to the body
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->width = pbody->height = radius;
	b->SetUserData(pbody);
	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	b->SetUserData(pbody);
	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->width = pbody->height = 0;
	b->SetUserData(pbody);
	return pbody;
}


update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	mousePosition = { 0,0 };

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

		if (InputMouseJoint(b))
			break;
	}

	UpdateMouseJoint();

	return UPDATE_CONTINUE;
}

int ModulePhysics::InputMouseJoint(b2Body* b)
{
	bool ret = false;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && (mouseJoint == NULL))
	{
		// test if the current body contains mouse position
		bool contain = false;
		PhysBody* body = (PhysBody*)b->GetUserData(); // Get PhysBody pointer
		b2Vec2 mousePos = { (float32)App->input->GetMouseX(),(float32)App->input->GetMouseY() };
		if (body != NULL)
			contain = body->Contains(mousePos.x, mousePos.y); // Check if point is in the body shape

		if (contain)
		{
			bodyClicked = body->body;
			mousePos.x = PIXEL_TO_METERS(mousePos.x);
			mousePos.y = PIXEL_TO_METERS(mousePos.y);

			mousePosition = mousePos;
			LOG("Body contains mouse!"); // Log it
			return ret = true;
		}
	}
	return ret;
}

void ModulePhysics::UpdateMouseJoint()
{
	if (bodyClicked != NULL && mouseJoint == NULL)
	{
		b2BodyDef bd;
		b2Body* ground = world->CreateBody(&bd);

		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = bodyClicked;
		def.target = mousePosition;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * bodyClicked->GetMass();
		mouseJoint = (b2MouseJoint*)world->CreateJoint(&def);
	}

	if (mouseJoint != NULL)
	{
		b2Vec2 mousePos = { PIXEL_TO_METERS((float32)App->input->GetMouseX()), PIXEL_TO_METERS((float32)App->input->GetMouseY()) }; // Mouse pos convert from pixels to meters
		b2Vec2 p1 = mouseJoint->GetAnchorA();
		b2Vec2 p2 = mouseJoint->GetAnchorB();

		//mouse_joint->GetAnchorB().Set(PIXEL_TO_METERS(mousePos.x), PIXEL_TO_METERS(mousePos.y)); // Set anchorB pos to mouse pos
		mouseJoint->SetTarget(mousePos);
		App->renderer->DrawLine(METERS_TO_PIXELS(p1.x), METERS_TO_PIXELS(p1.y), METERS_TO_PIXELS(p2.x), METERS_TO_PIXELS(p2.y), 255, 0, 0); // Convert the P2 from meters to pixel
		b2Vec2 line = p2 - p1;

		LOG("P1 is %d %d", METERS_TO_PIXELS(p1.x), METERS_TO_PIXELS(p1.y));
		LOG("P2 is %d %d", METERS_TO_PIXELS(p2.x), METERS_TO_PIXELS(p2.y));
		LOG("Lenght is %d", METERS_TO_PIXELS(line.Length()));
		LOG("mousePos is %d %d", METERS_TO_PIXELS(mousePos.x), METERS_TO_PIXELS(mousePos.y));
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouseJoint != NULL)
	{
		world->DestroyJoint(mouseJoint);
		if (mouseJoint != NULL)
			mouseJoint = NULL;
		bodyClicked = NULL;
	}
}

bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	bool ret = false;
	// TODO 1: Write the code to return true in case the point
	// is inside ANY of the shapes contained by this body
	b2Shape* shape = body->GetFixtureList()->GetShape();

	if (shape != NULL)
	{
		b2Vec2 point = { 0,0 };
		point.x = PIXEL_TO_METERS(x);
		point.y = PIXEL_TO_METERS(y);

		ret = shape->TestPoint(body->GetTransform(), point); 
	}

	return ret;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const // Used to avoid tunelling effects
{
	// TODO 2: Write code to test a ray cast between both points provided. If not hit return -1
	// if hit, fill normal_x and normal_y and return the distance between x1,y1 and it's colliding point
	int ret = -1;

	// Take as one unit the lenght of point 1 and point 2 and it (fraction) is that many times the distance between point 1 
	// and it's colliding point.
	// Point 1 and 2 define the direction of the ray (vector)
	b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		b2RayCastInput input;
		input.p1 = { PIXEL_TO_METERS((float)x1),PIXEL_TO_METERS((float)y1) };
		input.p2 = { PIXEL_TO_METERS((float)x2),PIXEL_TO_METERS((float)y2) };
		input.maxFraction = 1.0f;
		int32 childIndex = 0;

		b2RayCastOutput output;
		bool hit = fixture->RayCast(&output, input, childIndex);
		if (hit)
		{
			normal_x = output.normal.x;
			normal_y = output.normal.y;
			float fx = x2 - x1;
			float fy = y2 - y1;
			float distP1P2 = sqrtf((normal_x * normal_x) + (normal_y * normal_y)); // Vector lenght

			// MAXFRACTION (how far do you want the ray to extend to)
			// max_lenght_raycast = max_fraction * vector_lenght(p2,p1);
			float total_dist = output.fraction * distP1P2;

			return total_dist;
		}
		fixture = fixture->GetNext();
	}
	return ret;
}

// TODO 3
void ModulePhysics::BeginContact(b2Contact* contact)
{
	LOG("Collision!");
	PhysBody * physBodyA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody * physBodyB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	// TODO 7: Call the listeners that are not NULL

	if (physBodyA && physBodyA->listener != NULL)
		physBodyA->listener->OnCollision(physBodyA, physBodyB);

	if (physBodyB && physBodyB->listener != NULL)
		physBodyB->listener->OnCollision(physBodyB, physBodyA);
}



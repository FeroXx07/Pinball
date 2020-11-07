#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScenePinball.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "Audio.h"
#include "ModulePhysics.h"


b2BodyType;

ModuleScenePinball::ModuleScenePinball(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	paddlesTex = chain = NULL;
	ray_on = false;
	sensed = false;
}

ModuleScenePinball::~ModuleScenePinball()
{}

void ModuleScenePinball::LoadMap()
{
	// Create the three boing balls
	bounces.add(App->physics->CreateCircle(182, 140, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)this;

	bounces.add(App->physics->CreateCircle(254, 144, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)this;
}

bool ModuleScenePinball::LoadAssets()
{
	bool ret = true;

	paddlesTex = App->textures->Load("pinball/wheel.png");
	chain = App->textures->Load("pinball/rick_head.png");
	infraTex = App->textures->Load("pinball/InfraPinball.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	return ret;
}


void ModuleScenePinball::LoadLeftPaddle(int x, int y)
{
	// -------------- Left Paddle Creation --------------------------------
	PhysBody* pBoxLeft = App->physics->CreateRectangle(x, y, 60, 20, b2BodyType::b2_dynamicBody);
	PhysBody* pCircleLeft = App->physics->CreateCircle(x, y, 10, b2BodyType::b2_staticBody);

	b2RevoluteJointDef revDefLeft;
	//pBox->body->SetGravityScale(0.0f);
	revDefLeft.bodyA = pBoxLeft->body;
	revDefLeft.bodyB = pCircleLeft->body;
	revDefLeft.collideConnected = false;

	revDefLeft.localAnchorA.Set(-0.5f, 0);
	revDefLeft.localAnchorB.Set(0, 0);

	revDefLeft.enableLimit = true;
	revDefLeft.lowerAngle = -45 * DEGTORAD;
	revDefLeft.upperAngle = 45 * DEGTORAD;

	revDefLeft.enableMotor = false;
	revDefLeft.maxMotorTorque = 5;
	paddleSpeed = 270 * DEGTORAD;
	revDefLeft.motorSpeed = paddleSpeed;

	leftPaddle = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revDefLeft);
	totalPaddles.add(leftPaddle);
}
void ModuleScenePinball::LoadRightPaddle(int x, int y)
{
	// -------------- Right Paddle Creation --------------------------------
	PhysBody* pBoxRight = App->physics->CreateRectangle(x, y, 60, 20, b2BodyType::b2_dynamicBody);
	PhysBody* pCircleRight = App->physics->CreateCircle(x, y, 10, b2BodyType::b2_staticBody);

	b2RevoluteJointDef revDefRight;
	//pBox->body->SetGravityScale(0.0f);
	revDefRight.bodyA = pBoxRight->body;
	revDefRight.bodyB = pCircleRight->body;
	revDefRight.collideConnected = false;

	revDefRight.localAnchorA.Set(0.5f, 0);
	revDefRight.localAnchorB.Set(0, 0);

	revDefRight.enableLimit = true;
	revDefRight.lowerAngle = -45 * DEGTORAD;
	revDefRight.upperAngle = 45 * DEGTORAD;

	revDefRight.enableMotor = false;
	revDefRight.maxMotorTorque = 5;
	paddleSpeed = 270 * DEGTORAD;
	revDefRight.motorSpeed = paddleSpeed;

	rightPaddle = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revDefRight);
	totalPaddles.add(rightPaddle);
}

bool ModuleScenePinball::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	LoadAssets();

	principalMap = true;
	specialLeftNet = false;
	specialRightKicker = false;  // True at start
	specialRightZigZag = false;
	specialCenterTwirl = false;
	specialLeftToRight = false;

	LoadMap();
	
	// Creates the both paddles
	LoadLeftPaddle(95, 586);
	LoadRightPaddle(217, 586);
	return ret;
}


bool ModuleScenePinball::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(infraTex);

	bounces.~p2List();
	chains.~p2List();

	return true;
}

// Update: draw background
update_status ModuleScenePinball::Update()
{
	DebugCreate();

	PaddleInput();

	PaddleLogic();
	
	// AL booleans false -- > Activate the array/list of chains of all map
	
	// If one boolean is true -- > Deactivate whole 
		// Activate bodies to the corresponding boolean
	
	PreRayCast();

	DrawInfra();
	DrawBounces();
	DrawChains();
	DrawPaddlles();
	PostRayCast();

	return UPDATE_CONTINUE;
}

void ModuleScenePinball::PaddleInput()
{
	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		rightPaddle->GetBodyA()->ApplyTorque(paddleSpeed * 30, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		leftPaddle->GetBodyA()->ApplyTorque(-paddleSpeed * 30, true);
	}
}

void ModuleScenePinball::PaddleLogic()
{
	// Checks and iterates all limits of the paddles
	p2List_item<b2RevoluteJoint*>*list;
	list = totalPaddles.getFirst();
	while (list != NULL)
	{
		float32 currentAngle = list->data->GetJointAngle();
		float32 lowerAngleLimit = list->data->GetLowerLimit();
		float32 upperAngleLimit = list->data->GetUpperLimit();

		bool isLowerAngleLimit = list->data->GetJointAngle() <= list->data->GetLowerLimit();
		bool isUpperAngleLimit = list->data->GetJointAngle() >= list->data->GetUpperLimit();

		if (isLowerAngleLimit)
		{
			list->data->SetMotorSpeed(paddleSpeed);
		}
		if (isUpperAngleLimit)
		{
			list->data->SetMotorSpeed(-paddleSpeed);
		}

		list = list->next;
	}
}

void ModuleScenePinball::DrawPaddlles()
{
	/*PhysBody* toDraw = leftPaddle;
	if ( != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(chain, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}*/


}

void ModuleScenePinball::PreRayCast()
{
	// Prepare for raycast 
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	ray_hit = ray.DistanceTo(mouse);
	normal = { 0.0f, 0.0f };
}

void ModuleScenePinball::PostRayCast()
{
	// ray 
	if (ray_on == true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}
}

void ModuleScenePinball::DebugCreate()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		chains.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}
}

void ModuleScenePinball::DrawInfra()
{
	App->renderer->Blit(infraTex, 0, 0);
}

void ModuleScenePinball::DrawChains()
{
	p2List_item<PhysBody*>* c = chains.getFirst();
	c = chains.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(chain, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}
}

void ModuleScenePinball::DrawBounces()
{
	p2List_item<PhysBody*>* c = bounces.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if (ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}
}

void ModuleScenePinball::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	// bodyA is internal of the Scene & bodyB is the ball always

}

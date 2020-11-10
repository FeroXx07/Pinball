#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScenePinball.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "Audio.h"
#include "ModulePhysics.h"

#define COUNTDOWNBONUS 300

b2BodyType;

ModuleScenePinball::ModuleScenePinball(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	leftPaddleTex = chain = NULL;
	ray_on = false;
	sensed = false;
	//kickerJoint = NULL;
	
}

ModuleScenePinball::~ModuleScenePinball()
{}

Sensors::Sensors(){}

Sensors* ModuleScenePinball::CreateSensor(PhysBody* b, PhysBody* c, iPoint p, Module* l)
{
	Sensors* sensor = new Sensors();

	sensor->sensorBody = b;
	sensor->chainBody = c;
	sensor->pos = p;
	b->body->GetFixtureList()->SetSensor(true);
	sensor->listener = l;
	sensor->isActive = false;
	b->listener = l;

	return sensor;
}

void ModuleScenePinball::LoadMap()
{
	// Create the three boing balls
	bounces.add(App->physics->CreateCircle(182, 140, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)this;

	bounces.add(App->physics->CreateCircle(254, 144, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)this;

	// Upper small rects
	App->physics->CreateRectangle(187, 94, 3, 18);
	App->physics->CreateRectangle(209, 94, 3, 18);
	App->physics->CreateRectangle(233, 94, 3, 18);
	App->physics->CreateRectangle(255, 94, 3, 18);

	// The base walls, and side Plates
	App->physics->CreateChain(0, 0, wallsPoints, 312, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, rightPlate, 30, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, leftPlate, 36, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, rightRamp, 12, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, leftRamp, 12, b2BodyType::b2_staticBody);

	firstRecChain = App->physics->CreateChain(0, 0, firstRec, 10, b2BodyType::b2_staticBody);
	secondRecChain = App->physics->CreateChain(0, 0, secondRec, 10, b2BodyType::b2_staticBody);
	thirdRecChain = App->physics->CreateChain(0, 0, thirdRec, 8, b2BodyType::b2_staticBody);

}

bool ModuleScenePinball::LoadAssets()
{
	bool ret = true;

	leftPaddleTex = App->textures->Load("pinball/LeftStick.png");
	rightPaddleTex = App->textures->Load("pinball/RightStick.png");
	chain = App->textures->Load("pinball/rick_head.png");
	infraTex = App->textures->Load("pinball/InfraPinball.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//PhysBody* pBox2 = App->physics->CreateRectangle(200, 350, 20, 10, b2BodyType::b2_staticBody);

	bonusLettersTex.add(App->textures->Load("pinball/P.png"));
	bonusLettersTex.add(App->textures->Load("pinball/O.png"));
	bonusLettersTex.add(App->textures->Load("pinball/W.png"));
	bonusLettersTex.add(App->textures->Load("pinball/E.png"));
	bonusLettersTex.add(App->textures->Load("pinball/R.png"));
	bonusLettersTex.add(App->textures->Load("pinball/D.png"));
	bonusLettersTex.add(App->textures->Load("pinball/U.png"));
	bonusLettersTex.add(App->textures->Load("pinball/N.png"));
	bonusLettersTex.add(App->textures->Load("pinball/K.png"));

	return ret;
}

void ModuleScenePinball::LoadSensors()
{
	bonusSensors.add(App->physics->CreateRectangle(21, 367, 5, 5, b2BodyType::b2_staticBody)); // Letter P
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(30, 348, 5, 5, b2BodyType::b2_staticBody)); // Letter O
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(38, 322, 5, 5, b2BodyType::b2_staticBody)); // Letter W
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(84, 284, 5, 5, b2BodyType::b2_staticBody)); // Letter E
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(139, 277, 5, 5, b2BodyType::b2_staticBody)); // Letter R
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(161, 284, 5, 5, b2BodyType::b2_staticBody)); // Letter D
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(232, 283, 5, 5, b2BodyType::b2_staticBody)); // Letter U
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(287, 358, 5, 5, b2BodyType::b2_staticBody)); // Letter N
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;

	bonusSensors.add(App->physics->CreateRectangle(295, 382, 5, 5, b2BodyType::b2_staticBody)); // Letter K
	bonusSensors.getLast()->data->body->GetFixtureList()->SetSensor(true);
	bonusSensors.getLast()->data->listener = this;
	
	//Sensors* a = &(Sensors(App->physics->CreateRectangle(322, 233, 5, 5, b2BodyType::b2_staticBody), firstRecChain, iPoint(322, 233), (Module*)this));
	//mapSensors.add(a);
	//mapSensors.add(&(Sensors(App->physics->CreateRectangle(324, 172, 5, 5, b2BodyType::b2_staticBody),secondRecChain, iPoint(324, 172), (Module*)this)));
	Sensors* a = CreateSensor(App->physics->CreateRectangle(322, 233, 5, 5, b2BodyType::b2_staticBody), firstRecChain, iPoint(322, 233), (Module*)this);
	mapSensors.add(a);
	Sensors* b = CreateSensor(App->physics->CreateRectangle(324, 172, 5, 5, b2BodyType::b2_staticBody), secondRecChain, iPoint(324, 172), (Module*)this);
	mapSensors.add(b);
}


void ModuleScenePinball::LoadLeftPaddle(int x, int y)
{
	// -------------- Left Paddle Creation --------------------------------
	PhysBody* pBoxLeft = App->physics->CreateRectangle(x, y, 56, 10, b2BodyType::b2_dynamicBody);
	PhysBody* pCircleLeft = App->physics->CreateCircle(x, y, 10, b2BodyType::b2_staticBody);

	b2RevoluteJointDef revDefLeft;
	//pBox->body->SetGravityScale(0.0f);
	revDefLeft.bodyA = pBoxLeft->body;
	revDefLeft.bodyB = pCircleLeft->body;
	revDefLeft.collideConnected = false;

	revDefLeft.localAnchorA.Set(-0.5f, 0);
	revDefLeft.localAnchorB.Set(0, 0);

	revDefLeft.enableLimit = true;
	/*revDefLeft.lowerAngle = -45 * DEGTORAD;
	revDefLeft.upperAngle = 45 * DEGTORAD;*/
	revDefLeft.lowerAngle = -25 * DEGTORAD;
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
	PhysBody* pBoxRight = App->physics->CreateRectangle(x, y, 56, 10, b2BodyType::b2_dynamicBody);
	PhysBody* pCircleRight = App->physics->CreateCircle(x, y, 10, b2BodyType::b2_staticBody);

	b2RevoluteJointDef revDefRight;
	//pBoxRight->body->SetGravityScale(0.0f);
	revDefRight.bodyA = pBoxRight->body;
	revDefRight.bodyB = pCircleRight->body;
	revDefRight.collideConnected = false;

	revDefRight.localAnchorA.Set(0.5f, 0);
	revDefRight.localAnchorB.Set(0, 0);

	revDefRight.enableLimit = true;
	revDefRight.lowerAngle =  -50 * DEGTORAD;
	revDefRight.upperAngle = 25 * DEGTORAD;

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
	LoadSensors();

	SetAllBonusToFalse();
	StartBonusPointsPos();

	// Creates the both paddles
	LoadLeftPaddle(95, 586);
	LoadRightPaddle(217, 586);
	return ret;
}


bool ModuleScenePinball::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(infraTex);
	App->textures->Unload(leftPaddleTex);
	App->textures->Unload(rightPaddleTex);

	p2List_item<SDL_Texture*>* texs;
	texs = bonusLettersTex.getFirst();
	for (int i = 0; i < bonusLettersTex.count(); ++i)
	{
		if (texs == NULL)
			break;
		App->textures->Unload(texs->data);
		if (i == bonusLettersTex.count() - 1)
			break;

		texs = texs->next;
	}
	
	bounces.~p2List();
	bonusLettersTex.~p2List();
	bonusSensors.~p2List();
	return true;
}

// Update: draw background
update_status ModuleScenePinball::Update()
{
	DebugCreate();

	PaddleInput();
	
	BonusLettersLogic();
	// AL booleans false -- > Activate the array/list of chains of all map
	// Map Sensor check
	ChangeMap();
	// If one boolean is true -- > Deactivate whole 
		// Activate bodies to the corresponding boolean
	
	PreRayCast();

	DrawInfra();
	DrawBounces();
	DrawPaddlles();
	DrawBonusLetters();

	PostRayCast();

	return UPDATE_CONTINUE;
}

void ModuleScenePinball::ChangeMap()
{
	p2List_item< Sensors*>* sensorsList;
	sensorsList = mapSensors.getFirst();
	for (int i = 0; i < mapSensors.count(); ++i)
	{
		if (sensorsList->data->isActive == true)
		{
			sensorsList->data->chainBody->body->SetActive(false);
			LOG("SENSOR %d is TRUE", i);
		}

		if (i == mapSensors.count() - 1)
			break;

		sensorsList = sensorsList->next;
	}
}

void ModuleScenePinball::PaddleInput()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		rightPaddle->GetBodyA()->ApplyTorque(paddleSpeed * 30, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		leftPaddle->GetBodyA()->ApplyTorque(-paddleSpeed * 30, true);
	}
}


void ModuleScenePinball::DrawPaddlles()
{
	PhysBody* toDraw = (PhysBody*)leftPaddle->GetBodyA()->GetUserData();
	if ( toDraw != NULL)
	{
		int x, y;
		toDraw->GetPosition(x, y);
		App->renderer->Blit(leftPaddleTex, x, y-20, NULL, 1.0f, toDraw->GetRotation()-35.0f);
	}

	toDraw = NULL;

	toDraw = (PhysBody*)rightPaddle->GetBodyA()->GetUserData();
	if (toDraw != NULL)
	{
		int x, y;
		toDraw->GetPosition(x, y);
		App->renderer->Blit(rightPaddleTex, x-5, y - 20, NULL, 1.0f, toDraw->GetRotation() + 35.0f);
	}

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
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50, b2_dynamicBody));
	}
}

void ModuleScenePinball::DrawInfra()
{
	App->renderer->Blit(infraTex, 0, 0);
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

void ModuleScenePinball::DrawBonusLetters()
{
	p2List_item<SDL_Texture*>* texs;
	texs = bonusLettersTex.getFirst();
	for (int i = 0; i < bonusLettersTex.count(); ++i)
	{
		if (bonusLetters[i] == true && texs != NULL)
		{
			App->renderer->Blit(texs->data, bonusLettersPos[i].x, bonusLettersPos[i].y);
		}

		texs = texs->next;
	}
}


void ModuleScenePinball::SetAllBonusToFalse()
{
	for (int i = 0; i < 9; ++i)
	{
		bonusLetterTimer[i] = 0;
		bonusLetters[i] = false;
	}
}

void ModuleScenePinball::StartBonusPointsPos()
{
	bonusLettersPos[0] = { 20,362 };
	bonusLettersPos[1] = { 29,345 };
	bonusLettersPos[2] = { 38,329 };
	bonusLettersPos[3] = { 76,280 };
	bonusLettersPos[4] = { 121,277 };
	bonusLettersPos[5] = { 142,286 };
	bonusLettersPos[6] = { 213,283 };
	bonusLettersPos[7] = { 254,351 };
	bonusLettersPos[8] = { 260,376 };

}

void ModuleScenePinball::BonusLettersLogic()
{
	for (int i = 0; i < bonusLettersTex.count(); ++i)
	{
		if (bonusLetters[i] == true)
		{
			++bonusLetterTimer[i];
			LOG("SENSOR %d is TRUE", i);

			if (bonusLetterTimer[i] >= COUNTDOWNBONUS) // Cooldown of the bonus sensors
			{
				bonusLetterTimer[i] = 0;
				bonusLetters[i] = false;
			}

		}

		if (bonusLetterTimer[i] == 1) // Sum bonus score
		{

		}
	}
}


void ModuleScenePinball::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	// bodyA is internal of the Scene & bodyB is the ball always
	if (bodyA->body->GetFixtureList()->IsSensor())
	{
		// Bonus Sensor check
		p2List_item< PhysBody*>* bonusList;
		bonusList = bonusSensors.getFirst();
		for (int i = 0; i < bonusSensors.count(); ++i)
		{
			if (bodyA == bonusList->data)
			{
				bonusLetters[i] = true;
				LOG("SENSOR %d is TRUE", i);
			}

			if (i == bonusSensors.count() - 1)
				break;

			bonusList = bonusList->next;
		}

		// Map Sensor check
		p2List_item< Sensors*>* sensorsList = NULL;
		sensorsList = mapSensors.getFirst();
		for (int i = 0; i < mapSensors.count(); ++i)
		{
			if (bodyA == sensorsList->data->sensorBody)
			{
				sensorsList->data->isActive = true;
				LOG("SENSOR %d is TRUE", i);
			}

			if (i == mapSensors.count() - 1)
				break;

			sensorsList = sensorsList->next;
		}
		
	}
}

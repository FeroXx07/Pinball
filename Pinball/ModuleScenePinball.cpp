#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScenePinball.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "Audio.h"
#include "ModulePhysics.h"
#include "ModuleHud.h"
#include "ModuleBall.h"

#define COUNTDOWNBONUS 300

b2BodyType;

ModuleScenePinball::ModuleScenePinball(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	leftPaddleTex = rightPaddleTex = NULL;
	ray_on = false;
	sensed = false;
	debug = false;
	gameStart = true;
	gameOver = false;
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
	bounces.getLast()->data->listener = (Module*)App->ball;
	bounces.getLast()->data->body->GetFixtureList()->SetRestitution(0.25f); // Chain Bounciness ++
	reboundableBody.add(bounces.getLast()->data); // Add this PhysBody to a special list in order to do rebound when collision

	bounces.add(App->physics->CreateCircle(254, 144, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)App->ball;
	bounces.getLast()->data->body->GetFixtureList()->SetRestitution(0.25f); // Chain Bounciness ++
	reboundableBody.add(bounces.getLast()->data);// Add this PhysBody to a special list in order to do rebound when collision

	bounces.add(App->physics->CreateCircle(210, 188, 23, b2BodyType::b2_staticBody));
	bounces.getLast()->data->listener = (Module*)App->ball;
	bounces.getLast()->data->body->GetFixtureList()->SetRestitution(0.0f); // Chain Bounciness ++
	reboundableBody.add(bounces.getLast()->data); // Add this PhysBody to a special list in order to do rebound when collision

	// Upper small rects
	redRects.add(App->physics->CreateRectangle(187, 94, 3, 18));
	redRects.getLast()->data->listener = (Module*)this;
	redRects.add(App->physics->CreateRectangle(209, 94, 3, 18));
	redRects.getLast()->data->listener = (Module*)this;
	redRects.add(App->physics->CreateRectangle(233, 94, 3, 18));
	redRects.getLast()->data->listener = (Module*)this;
	redRects.add(App->physics->CreateRectangle(255, 94, 3, 18));
	redRects.getLast()->data->listener = (Module*)this;

	// The base walls, and side Plates
	App->physics->CreateChain(0, 0, wallsPoints, 160, b2BodyType::b2_staticBody);

	reboundableBody.add(App->physics->CreateChain(0, 0, leftPlate, 36, b2BodyType::b2_staticBody));		 // Add this PhysBody to a special list in order to do rebound when collision
	reboundableBody.getLast()->data->listener = (Module*)App->ball;
	//reboundableBody.getLast()->data->body->GetFixtureList()->SetRestitution(1.5f); // Chain Bounciness ++

	reboundableBody.add(App->physics->CreateChain(0, 0, rightPlate, 30, b2BodyType::b2_staticBody));	 // Add this PhysBody to a special list in order to do rebound when collision
	reboundableBody.getLast()->data->listener= (Module*)App->ball;
	//reboundableBody.getLast()->data->body->GetFixtureList()->SetRestitution(1.5f); // Chain Bounciness ++


	App->physics->CreateChain(0, 0, rightRamp, 12, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, leftRamp, 12, b2BodyType::b2_staticBody);

	App->physics->CreateChain(0, 0, blackRamp, 54, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, leftBigPlate, 92, b2BodyType::b2_staticBody);
	App->physics->CreateChain(0, 0, rightBigPlate, 56, b2BodyType::b2_staticBody);

	exitKickerRect = App->physics->CreateChain(0+50, 0+50, exitRect, 10, b2BodyType::b2_staticBody);
	exitKickerRect->body->SetActive(false);

}

bool ModuleScenePinball::LoadAssets()
{
	bool ret = true;

	leftPaddleTex = App->textures->Load("pinball/sprites/LeftStick.png");
	rightPaddleTex = App->textures->Load("pinball/sprites/RightStick.png");
	infraTex = App->textures->Load("pinball/sprites/InfraPinball.png");
	initialTex = App->textures->Load("pinball/sprites/InitialScreen.png");
	gameOverTex = App->textures->Load("pinball/sprites/GameOverScreen.png");

	bonusFx = App->audio->LoadFx("pinball/audio/bonusFx.wav");
	paddleFx = App->audio->LoadFx("pinball/audio/paddleFx.wav");

	bonusLettersTex.add(App->textures->Load("pinball/sprites/P.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/O.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/W.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/E.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/R.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/D.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/U.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/N.png"));
	bonusLettersTex.add(App->textures->Load("pinball/sprites/K.png"));

	App->audio->PlayMusic("pinball/audio/bgMusic.ogg");
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
	
	// Exit sensor
	Sensors* a = CreateSensor(App->physics->CreateRectangle(309, 102, 5, 5, b2BodyType::b2_staticBody), exitKickerRect, iPoint(309, 102), (Module*)this);
	mapSensors.add(a);
	// Death sensor
	Sensors* b = CreateSensor(App->physics->CreateRectangle(155, 641, 120, 40, b2BodyType::b2_staticBody), NULL, iPoint(155, 641), (Module*)App->ball);
	mapSensors.add(b);
}


void ModuleScenePinball::LoadLeftPaddle(int x, int y)
{
	// -------------- Left Paddle Creation --------------------------------
	PhysBody* pBoxLeft = App->physics->CreateRectangle(x, y, 56, 10, b2BodyType::b2_dynamicBody);
	PhysBody* pCircleLeft = App->physics->CreateCircle(x, y, 10, b2BodyType::b2_staticBody);
	pCircleLeft->body->GetFixtureList()->SetSensor(true);

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
	pCircleRight->body->GetFixtureList()->SetSensor(true);

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
	App->audio->UnloadFx(bonusFx);
	App->audio->UnloadFx(paddleFx);

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
	if (gameStart == false && gameOver == false)
	{
		Debug();

		PaddleInput();

		BonusLettersLogic();

		SensorLogic();

		PreRayCast();

		DrawInfra();
		DrawBounces();
		DrawPaddlles();
		DrawBonusLetters();

		PostRayCast();
	}
	else if (gameStart == true)
	{
		// Draw Image
		App->renderer->Blit(initialTex, 0, 0);
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			// If press start then startGame = false;
			gameStart = false;
		}
		
	}
	else if (gameOver == true)
	{
		// Draw Image
		App->renderer->Blit(gameOverTex, 0, 0);
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			// If press start then startGame = false;
			App->ball->lives = 3;
			gameOver = false;
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleScenePinball::SensorLogic()
{
	p2List_item< Sensors*>* sensorsList;

	// Exit kicker Sensor check
	sensorsList = mapSensors.getFirst();
	if (sensorsList->data->isActive == true)
	{
		if (sensorsList->data->chainBody->body->IsActive() == false) // If sensor true and exit false, then put exit chain into correct pos (0,0)
		{
			b2Vec2 newPos = {0.0f,0.0f };
			sensorsList->data->chainBody->body->SetTransform(newPos, sensorsList->data->chainBody->GetRotation());
			sensorsList->data->chainBody->body->SetActive(true);
		}
	}

}

void ModuleScenePinball::PaddleInput()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		rightPaddle->GetBodyA()->ApplyTorque(paddleSpeed * 30, true);
		App->audio->PlayFx(paddleFx);
	}
	/*if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		rightPaddle->GetBodyA()->SetGravityScale(0.0f);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		rightPaddle->GetBodyA()->SetGravityScale(1.0f);
	}*/

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		leftPaddle->GetBodyA()->ApplyTorque(-paddleSpeed * 30, true);
		App->audio->PlayFx(paddleFx);
	}
	/*if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		leftPaddle->GetBodyA()->SetGravityScale(0.0f);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		leftPaddle->GetBodyA()->SetGravityScale(1.0f);
	}*/
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
	if (ray_on == true && debug ==true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}
}

void ModuleScenePinball::Debug()
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

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug = !debug;
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
	int count = 0;
	for (int i = 0; i < bonusLettersTex.count(); ++i)
	{
		if (bonusLetters[i] == true)
		{
			++bonusLetterTimer[i];
			++count;
			LOG("SENSOR %d is TRUE", i);
		}

		if (bonusLetterTimer[i] == 1) // Sum bonus score
		{
			App->audio->PlayFx(bonusFx);
			App->hud->score += 40;
		}
	}

	if (count == 9) // If all letters true add on live
	{
		App->audio->PlayFx(bonusFx);
		SetAllBonusToFalse();
		App->ball->lives += 1;
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
	else
	{
		App->hud->score += 20;
	}
}

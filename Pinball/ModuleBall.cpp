#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleBall.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "Audio.h"
#include "ModulePhysics.h"
#include "ModuleScenePinball.h"
#include "ModuleHud.h"

#define MAXBALLVEL 8.0f

ModuleBall::ModuleBall(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ballTexture = NULL;
	ray_on = false;
	sensed = false;
	debug = true;
	
}

ModuleBall::~ModuleBall()
{}


bool ModuleBall::Start()
{
	LOG("Loading Intro assets");
	bool ret = LoadAssets();
	lives = 3;

	b2Vec2 startPos = { 319.0f,585.0f };
	if (ball == NULL)
	{
		ball = App->physics->CreateCircle(startPos.x, startPos.y, 8);
		ball->listener = (Module*)this;
	}
	lost = false;
	return ret;
}

bool ModuleBall::LoadAssets()
{
	bool ret = true;
	ballTexture = App->textures->Load("pinball/wheel1.png");
	supraTex = App->textures->Load("pinball/SupraPinball.png");
	bonusFx = App->audio->LoadFx("pinball/bonus.wav");
	return ret;
}


bool ModuleBall::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(ballTexture);
	App->textures->Unload(supraTex);
	App->audio->UnloadFx(bonusFx);

	return true;
}

update_status ModuleBall::Update()
{
	if (lives > 0)
	{
		// <<<< INPUT >>>>
		DebugCreate();

		// <<<< LOGIC? >>>>
		ResetBallState();
		CapBallVel();
		BounceLogic();
		LogBall();

		
	}
	else
	{
		lost = true;
		
	}
	RestartGame();

	// <<<< DRAW >>>>
	PreRayCast();

	DrawBalls();
	DrawSupra();

	PostRayCast();
	return UPDATE_CONTINUE;
}
void ModuleBall::RestartGame()
{
	// CHANGE SCORE,PREVIOUS SCORE, HIGHSCORE


	// SHOW GAME OVER 
	LOG("LOSE :(");

	// PRESS SOMETHING TO RESTART ---> lives = 3;
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		lives = 3;
	}

	lost = false;
}


void ModuleBall::CapBallVel()
{
	if (ball != NULL && App->scene_pinball->exitKickerRect->body->IsActive()==true)
	{
		b2Vec2 currentVel = ball->body->GetLinearVelocity();
		if (currentVel.y > MAXBALLVEL)
		{
			b2Vec2 newVel = { currentVel.x,MAXBALLVEL };
			ball->body->SetLinearVelocity(newVel);
		}
		/*else if (currentVel.y < -MAXBALLVEL)
		{
			b2Vec2 newVel = { currentVel.x,-MAXBALLVEL };
			ball->body->SetLinearVelocity(newVel);
		}*/
	}
}

void ModuleBall::DebugCreate()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		if (ball == NULL)
		{
			ball = App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8);
			ball->listener = (Module*)this;
		}
		else
		{
			b2Vec2 mousePos = { PIXEL_TO_METERS(App->input->GetMouseX()),  PIXEL_TO_METERS(App->input->GetMouseY()) };
			b2Vec2 newVel = { 0.0f,2.0f };
			ball->body->SetTransform(mousePos, ball->GetRotation());
			ball->body->SetLinearVelocity(newVel);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug = !debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		dead = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) // Provisional kicker
	{
		b2Vec2 startPos = { 300.0f,580.0f };

		int x = 0, y = 0;
		ball->GetPosition(x, y);
		if (x >= startPos.x && y >= startPos.y)
		{
			if (ball != NULL)
			{
				b2Vec2 force = { 0.0f,-75.0f };
				ball->body->ApplyForceToCenter(force, true);
			}
		}
		
	}
}

void ModuleBall::DrawBalls()
{
	p2List_item<PhysBody*>* c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		/*if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))*/
		if (ballTexture !=NULL)App->renderer->Blit(ballTexture, x, y, NULL, 1.0f, c->data->GetRotation());
		if (ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		if (debug)
		{
			b2Vec2 vel = c->data->body->GetLinearVelocity();
			vel.Normalize();
			iPoint pos = { x,y };
			App->renderer->DrawLine(pos.x + 8, pos.y + 8, pos.x + vel.x * 25, pos.y + vel.y * 25, 250, 0, 0);
		}
		c = c->next;
	}
}

void ModuleBall::DrawSupra()
{
	App->renderer->Blit(supraTex, 0, 0);
}

void ModuleBall::PreRayCast()
{
	// Prepare for raycast 
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	ray_hit = ray.DistanceTo(mouse);
	normal = { 0.0f, 0.0f };
}

void ModuleBall::PostRayCast()
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

void ModuleBall::ResetBallState()
{
	if (dead)
	{
		lives = lives - 1;
		dead = false;
		b2Vec2 outSide = { 50.0f, 50.0f };
		p2List_item<Sensors*>* t;
		t = App->scene_pinball->mapSensors.getFirst();

		// If dead, set sensor false, and exit chain in a outside screen position
		t->data->chainBody->body->SetActive(false); 
		t->data->isActive = false;
		t->data->chainBody->body->SetTransform(outSide, App->scene_pinball->mapSensors.getFirst()->data->chainBody->GetRotation());

		// Reset the ball to the kicker 
		b2Vec2 startPos = { PIXEL_TO_METERS(319.0f),PIXEL_TO_METERS(585.0f) };
		if (ball != NULL)
			ball->body->SetTransform(startPos, ball->GetRotation());
	}

}

void ModuleBall::LogBall()
{
	if (debug && ball != NULL)
	{
		int x = 0, y = 0;
		b2Vec2 vel = ball->body->GetLinearVelocity();
		ball->GetPosition(x, y);
		
		LOG("Ball pos is %d %d", x, y);
		LOG("Ball vel is %f %f", vel.x, vel.y);
	}
}

void ModuleBall::BounceLogic()
{
	LOG("BOING!!!!");
	if (isBounce)
	{
		if (bounceTimer == 1)
		{
			//// Not doing what i Intend :(
			//b2Vec2 vel = ball->body->GetLinearVelocity();
			//vel.Normalize();
			//vel = { -vel.x,-vel.y/2 };
			//b2Vec2 newImpulse = vel;
			////ball->body->SetLinearVelocity(newImpulse);
			//ball->body->ApplyLinearImpulse(newImpulse, ball->body->GetLocalCenter(), true);
			////ball->body->ApplyForce(newImpulse, ball->body->GetLocalCenter(), true);
			//CapBallVel();

			// Sum points
			App->hud->score += 300;
			isBounce = false;
			LogBall();
		}

		if (bounceTimer >= 60) // Cooldown of the bonus sensors
		{
			bounceTimer = 0;
			isBounce = false;
		}

		bounceTimer++;
	}
}

void ModuleBall::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	//App->audio->PlayFx(bonusFx);
	if (bodyA->body->GetFixtureList()->IsSensor())
	{
		LOG("DEAD!!!!!!!!!");
		dead = true;
	}

	if (bodyA == ball)
	{
		p2List_item<PhysBody*>* boingList;
		boingList = App->scene_pinball->reboundableBody.getFirst();

		for (int i = 0; i < App->scene_pinball->reboundableBody.count(); ++i)
		{
			if (bodyB == boingList->data)
			{
				//LOG("BOING!!!!");
				//// Not doing what i Intend :(
				//b2Vec2 vel = ball->body->GetLinearVelocity();
				//vel.Normalize();
				//vel = { -vel.x,-vel.y/2 };
				//b2Vec2 newImpulse = vel;
				////ball->body->SetLinearVelocity(newImpulse);
				//ball->body->ApplyLinearImpulse(newImpulse, ball->body->GetLocalCenter(), true);
				////ball->body->ApplyForce(newImpulse, ball->body->GetLocalCenter(), true);
				//CapBallVel();
				isBounce = true;
				LogBall();
			}

			boingList = boingList->next;
		}

	}
}
// TODO 8: Now just define collision callback for the circle and play bonus_fx audio

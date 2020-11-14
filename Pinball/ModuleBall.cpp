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
	debug = false;
	
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
		ball = App->physics->CreateCircle(startPos.x, startPos.y, 7);//8
		ball->body->SetBullet(true);
		ball->listener = (Module*)this;
	}
	lost = false;
	return ret;
}

bool ModuleBall::LoadAssets()
{
	bool ret = true;
	ballTexture = App->textures->Load("pinball/sprites/ball.png");
	supraTex = App->textures->Load("pinball/sprites/SupraPinball.png");
	collisionFx = App->audio->LoadFx("pinball/audio/collideFx.wav");
	bounceFx = App->audio->LoadFx("pinball/audio/bounceFx.wav");
	return ret;
}


bool ModuleBall::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(ballTexture);
	App->textures->Unload(supraTex);
	App->audio->UnloadFx(collisionFx);
	App->audio->UnloadFx(bounceFx);

	return true;
}

update_status ModuleBall::Update()
{
	if (lives > 0)
	{
		if (App->scene_pinball->gameStart == false && App->scene_pinball->gameOver == false)
		{
			// <<<< INPUT >>>>
			DebugAndInput();

			// <<<< LOGIC? >>>>
			ResetBallState();
			CapBallVel();
			BounceLogic();
			LogBall();

			// <<<< DRAW >>>>
			//PreRayCast();
			DrawBalls();
			DrawSupra();
		}

		//PostRayCast();
	}
	else
	{
		lost = true;
		App->scene_pinball->gameOver = true;
	}

	RestartGameScore();

	
	return UPDATE_CONTINUE;
}
void ModuleBall::RestartGameScore()
{
	if (lost)
	{
		// CHANGE SCORE,PREVIOUS SCORE, HIGHSCORE
		if (App->hud->score != 0)
		{
			App->hud->previousScore = App->hud->score;
		}
		App->hud->score = 0;
		if (App->hud->highScore == 0)
		{
			App->hud->highScore = App->hud->previousScore;
		}
		if (App->hud->previousScore > App->hud->highScore)
		{
			App->hud->highScore = App->hud->previousScore;
		}
		// SHOW GAME OVER 
		LOG("LOSE :(");

		lost = false;
	}
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

void ModuleBall::DebugAndInput()
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

	if (ball != NULL)
	{
		int x, y;
		ball->GetPosition(x, y);
		/*if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))*/
		if (ballTexture !=NULL)App->renderer->Blit(ballTexture, x, y, NULL, 1.0f, ball->GetRotation());
		if (ray_on)
		{
			int hit = ball->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		if (debug)
		{
			b2Vec2 vel = ball->body->GetLinearVelocity();
			vel.Normalize();
			iPoint pos = { x,y };
			App->renderer->DrawLine(pos.x + 8, pos.y + 8, pos.x + vel.x * 25, pos.y + vel.y * 25, 250, 0, 0);
		}
		
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
			// Sum points
			App->hud->score += 30;
			isBounce = false;
			//LogBall();
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
	App->audio->PlayFx(collisionFx);

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

		PhysBody* temp = boingList->data;
		// Right bounce
		App->scene_pinball->reboundableBody.at(3, boingList->data);
		if (bodyB == boingList->data)
		{
			b2Vec2 force = { 35.0f,-25.0f };
			ball->body->ApplyForceToCenter(force, true);
		}
		App->scene_pinball->reboundableBody.at(4, boingList->data);
		if (bodyB == boingList->data)
		{
			b2Vec2 force = { -25.0f,-25.0f };
			ball->body->ApplyForceToCenter(force, true);
		}
	
		p2List_item<PhysBody*>* allBounces;
		allBounces = App->scene_pinball->reboundableBody.getFirst();
		for (int i = 0; i < App->scene_pinball->reboundableBody.count(); ++i)
		{
			if (bodyB == allBounces->data)
			{
				App->audio->PlayFx(bounceFx);
			}
			allBounces = allBounces->next;
		}

		//CapBallVel();

		isBounce = true;
		LogBall();
	
		
	}
}




#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleBall.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"



ModuleBall::ModuleBall(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ballTexture = NULL;
	ray_on = false;
	sensed = false;
}

ModuleBall::~ModuleBall()
{}


bool ModuleBall::Start()
{
	LOG("Loading Intro assets");
	bool ret = LoadAssets();
	return ret;
}

bool ModuleBall::LoadAssets()
{
	bool ret = true;
	ballTexture = App->textures->Load("pinball/wheel.png");
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
	// <<<< INPUT >>>>
	DebugCreate();


	// <<<< LOGIC? >>>>



	// <<<< DRAW >>>>
	PreRayCast();

	DrawBalls();
	DrawSupra();

	PostRayCast();

	return UPDATE_CONTINUE;
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
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8));
		circles.getLast()->data->listener = (Module*)this;
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
		//App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		if (ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
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

void ModuleBall::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonusFx);
	
}
// TODO 8: Now just define collision callback for the circle and play bonus_fx audio
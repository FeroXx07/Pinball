#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScenePinball.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"




ModuleScenePinball::ModuleScenePinball(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = NULL;
}

ModuleScenePinball::~ModuleScenePinball()
{}

// Load assets
bool ModuleScenePinball::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	backgroundTex = App->textures->Load("pinball/BG.png");

	return ret;
}

// Load assets
bool ModuleScenePinball::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(backgroundTex);
	return true;
}

// Update: draw background
update_status ModuleScenePinball::Update()
{
	// TODO 5: Move all creation of bodies on 1,2,3 key press here in the scene
	// On space bar press, create a circle on mouse position
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
 		circles.add(App->physics->CreatCircle((App->input->GetMouseX()), (App->input->GetMouseY()), 25));
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		rects.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(),50,25));
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY()));
	}

	BlitAll();

	return UPDATE_CONTINUE;
}

void ModuleScenePinball::BlitAll()
{
	// TODO 7: Draw all the circles using "circle" texture
	App->renderer->Blit(backgroundTex, 0, 0);

	p2List_item<b2PointerHouse*>* c = circles.getFirst();
	while (c != NULL)
	{
		int x = 0, y = 0;
		int r = 0;
		c->data->GetPosition(x, y);
		//r = c->data->radius;
		c->data->GetRadius(r);
		App->renderer->Blit(circle, x - r, y - r, NULL, 1.0f, c->data->GetRotation()); // The data is already converted from meters to pixels
		c = c->next;
	}

	p2List_item<b2PointerHouse*>* b = rects.getFirst();
	while (b != NULL)
	{
		int x = 0, y = 0;
		b->data->GetPosition(x, y);
		App->renderer->Blit(box, x - b->data->w, y - b->data->h, NULL, 1.0f, b->data->GetRotation()); // The data is already converted from meters to pixels
		b = b->next;
	}

	//p2List_item<b2PointerHouse*>* r = ricks.getFirst();
	//while (r != NULL)
	//{
	//	int x = 0, y = 0;
	//	r->data->GetPosition(x, y);
	//	App->renderer->Blit(rick, x, y, NULL, 1.0f, r->data->GetRotation()); // The data is already converted from meters to pixels
	//	r = r->next;
	//}
}
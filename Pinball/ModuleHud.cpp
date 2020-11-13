#include "ModuleHud.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleFonts.h"
#include "ModulePlayer.h"
#include "ModuleScenePinball.h"
#include "ModuleBall.h"

#include <stdio.h>
ModuleHud::ModuleHud(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}


// Destructor
ModuleHud::~ModuleHud()
{
	this->CleanUp();
}


bool ModuleHud::Start()
{
	LOG("Loading hud");

	bool ret = true;

	char lookupTableNumbers[] = { "0123456789" };
	//char lookupTableTextAndLives[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ.-," }; // "," is LIVES SPRITE!!!!!!!!!
	whiteFont = App->fonts->Load("pinball/fonts/score.png", lookupTableNumbers, 1);
	redFont = App->fonts->Load("pinball/fonts/Lives_Font.png", lookupTableNumbers, 1);
	greenFont = App->fonts->Load("pinball/fonts/previousScore.png", lookupTableNumbers, 1);
	//lightBlueFont = App->fonts->Load("Assets/Fonts/Fonts_LIGHTBLUE.png", lookupTableTextAndLives, 1);
	//darkBlueFont = App->fonts->Load("Assets/Fonts/Fonts_DARKBLUE.png", lookupTableNumbers, 1);
	score = 0;
	previousScore = 0;
	highScore = 0;
	return ret;
}

update_status ModuleHud::Update()
{
	// Draw UI (score) --------------------------------------
	sprintf_s(scoreText, 10, "%4d", score);
	sprintf_s(highScoreText, 10, "%4d", highScore);
	sprintf_s(previousScoreText, 10, "%4d", previousScore);

	sprintf_s(livesText, 10, "%1d", App->ball->lives);	

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleHud::PostUpdate()
{
	// TODO 3: Blit the text of the score in at the bottom of the screen
	if (drawScore)
	{

		App->fonts->BlitText(6, 5, whiteFont, scoreText);
		App->fonts->BlitText(305, 5, redFont, livesText);
		App->fonts->BlitText(106, 5, greenFont, previousScoreText);
		App->fonts->BlitText(206, 5, redFont, highScoreText);


		/*if (App->player->destroyed && App->hud->lives == 0 && App->intro->IsEnabled() == false)
		{
			App->fonts->BlitText(78, 185, lightBlueFont, "GAME");
			App->fonts->BlitText(120, 185, lightBlueFont, "OVER");
		}*/
	}


	return update_status::UPDATE_CONTINUE;
}


bool ModuleHud::CleanUp()
{
	return true;
}

#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"
#include "ModulePhysics.h"

#include<string.h>

ModuleFonts::ModuleFonts(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFonts::~ModuleFonts()
{

}

// Load new texture from file path
int ModuleFonts::Load(const char* texture_path, const char* characters, uint rows)
{
	int id = -1;

	if(texture_path == nullptr || characters == nullptr || rows == 0)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = App->textures->Load(texture_path);

	if(tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}


	/*for (; id < MAX_FONTS; ++id)
	{*/
	
	//}
		

	if(id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}

	//Font& font = fonts[id];
	Font* font = new Font();
	fonts.add(font);
	font->texture = tex;
	font->rows = rows;

	// TODO 1: Finish storing font data

	// totalLength ---	length of the lookup table
	// table ---------  All characters displayed in the same order as the texture
	// columns -------  Amount of chars per row of the texture
	// char_w --------	Width of each character
	// char_h --------	Height of each character

	/*trcpy_s(list->data->table, MAX_FONT_CHARS, characters);
	list->data->totalLength = strlen(characters);
	list->data->columns = list->data->totalLength / rows;*/

	strcpy_s(font->table, MAX_FONT_CHARS, characters);
	font->totalLength = strlen(characters);
	font->columns = font->totalLength / rows;


	uint tex_w, tex_h;
	App->textures->GetTextureSize(tex, tex_w, tex_h);
	font->char_w = tex_w / font->columns;
	font->char_h = tex_h / font->rows;

	++fontsCount;

	LOG("Successfully loaded BMP font from %s", texture_path);

	p2List_item<Font*>* list;
	id = 0;
	list = fonts.getLast();
	id = fonts.find(list->data);
	
	return id;
}

void ModuleFonts::UnLoad(int font_id)
{
	p2List_item<Font*>* list;
	list = fonts.getFirst();
	fonts.at(font_id, list->data);
	if(font_id >= 0 && font_id < fonts.count() && list->data->texture != nullptr)
	{
		App->textures->Unload(list->data->texture);
		list->data->texture = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void ModuleFonts::BlitText(int x, int y, int font_id, const char* text) const
{
	Font* data;
	fonts.at(font_id, data);
	if(text == nullptr || font_id < 0 || data->texture == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = data;
	SDL_Rect spriteRect;
	uint len = strlen(text);

	spriteRect.w = font->char_w;
	spriteRect.h = font->char_h;

	for(uint i = 0; i < len; ++i)
	{
		// TODO 2: Find the character in the table and its position in the texture, then Blit
		uint charIndex = 0;

		// Find the location of the current character in the lookup table
		for (uint j = 0; j < font->totalLength; ++j)
		{
			if (font->table[j] == text[i])
			{
				charIndex = j;
				break;
			}
		}

		// Retrieve the position of the current character in the sprite
		spriteRect.x = spriteRect.w * (charIndex % font->columns);
		spriteRect.y = spriteRect.h * (charIndex / font->columns);

		App->renderer->Blit(font->texture, x, y, &spriteRect, 0.0f, false);

		// Advance the position where we blit the next character
		x += spriteRect.w;
	}
}

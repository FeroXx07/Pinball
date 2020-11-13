#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class b2RevoluteJoint;
class PhysBody;

class Sensors
{
public:
	Sensors();
	~Sensors(){}

public:
	PhysBody* sensorBody = NULL;
	PhysBody* chainBody = NULL;
	iPoint pos;
	SDL_Texture* texture = NULL;
	bool isActive = false;
	Module* listener;
	
};

class ModuleScenePinball : public Module
{
public:
	ModuleScenePinball(Application* app, bool start_enabled = true);
	~ModuleScenePinball();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
public:
	p2List<PhysBody*> bounces;
	p2List<PhysBody*> boxes;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* leftPaddleTex;
	SDL_Texture* rightPaddleTex;
	SDL_Texture* chain;
	SDL_Texture* infraTex;

	uint collisionFx;
private:


private:
	// Start field
	bool LoadAssets();
	void LoadMap();
	void LoadLeftPaddle(int x, int y);
	void LoadRightPaddle(int x, int y);
	void LoadSensors();

	// Draw field
	void DrawBounces();

	void DrawInfra();
	void DrawPaddlles();
	void DrawBonusLetters();

	void DebugCreate();

private: // Raycast field
	iPoint mouse;
	int ray_hit;
	fVector normal = { 0.0f, 0.0f };
	p2Point<int> ray;
	bool ray_on;

	void PreRayCast();
	void PostRayCast();

public: // Paddle field
	b2RevoluteJoint* rightPaddle;
	b2RevoluteJoint* leftPaddle;
	p2List<b2RevoluteJoint*> totalPaddles;
	float paddleSpeed;
	void PaddleInput();

public: // Collider points
	//int wallsPoints[312] = {
	//223, 621,
	//310, 562,
	//309, 443,
	//303, 436,
	//294, 426,
	//290, 422,
	//287, 412,
	//288, 404,
	//294, 397,
	//297, 392,
	//298, 385,
	//296, 376,
	//294, 368,
	//291, 356,
	//288, 347,
	//287, 339,
	//287, 333,
	//290, 328,
	//294, 322,
	//294, 317,
	//286, 308,
	//281, 301,
	//280, 293,
	//281, 285,
	//284, 273,
	//287, 258,
	//292, 239,
	//298, 208,
	//307, 170,
	//312, 148,
	//317, 127,
	//321, 104,
	//322, 86,
	//316, 70,
	//308, 59,
	//299, 49,
	//280, 40,
	//261, 39,
	//237, 38,
	//211, 38,
	//200, 38,
	//183, 68,
	//184, 116,
	//183, 125,
	//174, 158,
	//168, 170,
	//168, 256,
	//171, 265,
	//175, 272,
	//179, 278,
	//179, 282,
	//170, 280,
	//153, 274,
	//134, 267,
	//125, 264,
	//120, 259,
	//118, 250,
	//116, 238,
	//114, 225,
	//112, 216,
	//110, 203,
	//106, 192,
	//103, 176,
	//100, 161,
	//97, 144,
	//95, 134,
	//93, 129,
	//90, 128,
	//89, 103,
	//89, 85,
	//95, 75,
	//102, 66,
	//93, 46,
	//85, 52,
	//77, 58,
	//72, 66,
	//68, 74,
	//65, 86,
	//67, 104,
	//68, 121,
	//70, 136,
	//74, 165,
	//75, 189,
	//77, 199,
	//79, 211,
	//79, 222,
	//82, 240,
	//83, 256,
	//84, 266,
	//84, 275,
	//79, 279,
	//73, 276,
	//62, 240,
	//58, 227,
	//54, 218,
	//49, 203,
	//43, 184,
	//40, 173,
	//36, 160,
	//33, 147,
	//31, 132,
	//31, 115,
	//35, 100,
	//42, 89,
	//52, 80,
	//62, 72,
	//47, 49,
	//40, 54,
	//32, 61,
	//24, 69,
	//15, 82,
	//9, 97,
	//5, 112,
	//4, 126,
	//4, 143,
	//5, 166,
	//7, 182,
	//12, 206,
	//17, 229,
	//21, 246,
	//25, 261,
	//28, 274,
	//31, 287,
	//35, 298,
	//36, 305,
	//35, 316,
	//31, 327,
	//25, 337,
	//20, 348,
	//15, 358,
	//10, 368,
	//8, 373,
	//8, 380,
	//13, 385,
	//18, 391,
	//24, 398,
	//26, 403,
	//25, 408,
	//22, 413,
	//18, 419,
	//13, 425,
	//6, 437,
	//4, 477,
	//5, 510,
	//4, 550,
	//5, 568,
	//24, 579,
	//50, 594,
	//65, 605,
	//88, 622,
	//110, 621,
	//133, 622,
	//157, 622,
	//181, 622,
	//202, 622,
	//217, 622
	//};
	int wallsPoints[160] = {
310, 154,
310, 604,
330, 604,
330, 155,
324, 100,
323, 88,
316, 68,
299, 49,
276, 39,
254, 38,
216, 38,
193, 38,
183, 70,
183, 119,
176, 157,
168, 172,
167, 256,
170, 266,
175, 273,
182, 281,
172, 282,
160, 276,
136, 268,
125, 266,
120, 256,
115, 228,
112, 214,
107, 193,
96, 142,
94, 132,
89, 126,
90, 103,
112, 92,
84-4, 53-14,
41, 54-6,
22, 71,
10, 91,
4, 112,
3, 128,
3, 160,
8, 190,
16, 228,
25, 260,
34, 296,
36, 314,
27, 333,
15, 357,
8, 371,
8, 381,
15, 388,
22, 394,
28, 405,
20, 416,
10, 432,
6, 439,
4, 569,
24, 580,
87, 621,
87, 673,
224, 673,
224, 621,
310, 564,
309, 444,
301, 433,
292, 424,
287, 414,
290, 403,
297, 395,
300, 384,
296, 369,
291, 348,
288, 337,
293, 328,
296, 319,
287, 309,
281, 299,
282, 278,
287, 260,
297, 215,
308, 170
};
	int rightPlate[30] = {
	269, 489,
	269, 474,
	269, 460,
	264, 453,
	256, 456,
	251, 472,
	242, 500,
	235, 522,
	232, 530,
	235, 537,
	242, 537,
	251, 529,
	265, 521,
	269, 513,
	269, 497
	};
	int leftPlate[36] = {
	63, 472,
	57, 460,
	52, 452,
	47, 454,
	45, 460,
	44, 474,
	44, 500,
	44, 512,
	45, 518,
	52, 523,
	64, 531,
	72, 538,
	80, 538,
	84, 532,
	82, 525,
	77, 510,
	71, 492,
	65, 478
	};
	int rightRamp[12] = {
	289, 464,
	289, 528,
	220, 578,
	224, 583,
	292, 536,
	292, 464
	};
	int leftRamp[12] = {
	23, 464,
	26, 464,
	27, 529,
	97, 578,
	90, 583,
	23, 536
	};
	int exitRect[10] = {
	310, 157,
	317, 128,
	324, 97,
	329, 132,
	329, 157
	};
	int blackRamp[54] = {
	241, 275,
	251, 247,
	256, 235,
	256, 226,
	252, 218,
	245, 212,
	238, 207,
	205, 188,
	195, 188,
	190, 191,
	188, 196,
	188, 216,
	187, 243,
	187, 248,
	187, 254,
	191, 258,
	194, 248,
	197, 232,
	200, 219,
	202, 202,
	203, 194,
	237, 214,
	235, 235,
	234, 249,
	233, 260,
	233, 271,
	236, 277
	};
	int leftBigPlate[92] = {
	69, 69,
	68, 77,
	65, 83,
	65, 96,
	67, 107,
	68, 114,
	69, 125,
	70, 137,
	71, 146,
	73, 165,
	74, 180,
	74, 193,
	76, 204,
	77, 213,
	79, 229,
	80, 240,
	81, 252,
	82, 264,
	83, 270,
	84, 276,
	80, 277,
	76, 278,
	73, 270,
	70, 262,
	66, 252,
	64, 245,
	61, 237,
	58, 227,
	54, 216,
	52, 209,
	46, 191,
	43, 184,
	40, 174,
	37, 165,
	34, 159,
	32, 149,
	31, 140,
	30, 128,
	30, 120,
	32, 109,
	35, 102,
	40, 94,
	46, 87,
	53, 80,
	59, 76,
	65, 71
	};
	int rightBigPlate[56] = {
	260, 79,
	261, 85,
	264, 90,
	268, 93,
	263, 161,
	262, 172,
	262, 178,
	263, 185,
	265, 190,
	267, 197,
	275, 181,
	282, 164,
	287, 150,
	290, 141,
	293, 134,
	296, 124,
	297, 115,
	299, 104,
	299, 95,
	298, 87,
	294, 78,
	288, 72,
	282, 67,
	273, 63,
	266, 62,
	261, 63,
	260, 67,
	260, 73
	};

public: // Bonus letters field
	bool bonusLetters[9];
	iPoint bonusLettersPos[9];
	int bonusLetterTimer[9];
	p2List<SDL_Texture*> bonusLettersTex;

	void SetAllBonusToFalse(); // Sets all bonus bools to false
	void StartBonusPointsPos(); // Sets positions to the textures to be drawn when true
	void BonusLettersLogic(); // Countdown to disable the bools once true
	p2List< PhysBody*>bonusSensors;

public: // Map sensors field
	p2List< Sensors*>mapSensors;
	void SensorLogic();
	PhysBody* exitKickerRect;
	Sensors* CreateSensor(PhysBody* b, PhysBody* c, iPoint p, Module* l);

public: // Rebound (rebote) field
	p2List< PhysBody*>reboundableBody;

public: // RedRects 
	p2List< PhysBody*>redRects;

};

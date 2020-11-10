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

	uint bonus_fx;
private:
	bool principalMap = false;
	bool specialLeftNet = false;
	bool specialRightKicker = true;  // True at start
	bool specialRightZigZag = false;
	bool specialCenterTwirl = false;
	bool specialLeftToRight = false;

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
	int wallsPoints[312] = {
	223, 621,
	310, 562,
	309, 443,
	303, 436,
	294, 426,
	290, 422,
	287, 412,
	288, 404,
	294, 397,
	297, 392,
	298, 385,
	296, 376,
	294, 368,
	291, 356,
	288, 347,
	287, 339,
	287, 333,
	290, 328,
	294, 322,
	294, 317,
	286, 308,
	281, 301,
	280, 293,
	281, 285,
	284, 273,
	287, 258,
	292, 239,
	298, 208,
	307, 170,
	312, 148,
	317, 127,
	321, 104,
	322, 86,
	316, 70,
	308, 59,
	299, 49,
	280, 40,
	261, 39,
	237, 38,
	211, 38,
	200, 38,
	183, 68,
	184, 116,
	183, 125,
	174, 158,
	168, 170,
	168, 256,
	171, 265,
	175, 272,
	179, 278,
	179, 282,
	170, 280,
	153, 274,
	134, 267,
	125, 264,
	120, 259,
	118, 250,
	116, 238,
	114, 225,
	112, 216,
	110, 203,
	106, 192,
	103, 176,
	100, 161,
	97, 144,
	95, 134,
	93, 129,
	90, 128,
	89, 103,
	89, 85,
	95, 75,
	102, 66,
	93, 46,
	85, 52,
	77, 58,
	72, 66,
	68, 74,
	65, 86,
	67, 104,
	68, 121,
	70, 136,
	74, 165,
	75, 189,
	77, 199,
	79, 211,
	79, 222,
	82, 240,
	83, 256,
	84, 266,
	84, 275,
	79, 279,
	73, 276,
	62, 240,
	58, 227,
	54, 218,
	49, 203,
	43, 184,
	40, 173,
	36, 160,
	33, 147,
	31, 132,
	31, 115,
	35, 100,
	42, 89,
	52, 80,
	62, 72,
	47, 49,
	40, 54,
	32, 61,
	24, 69,
	15, 82,
	9, 97,
	5, 112,
	4, 126,
	4, 143,
	5, 166,
	7, 182,
	12, 206,
	17, 229,
	21, 246,
	25, 261,
	28, 274,
	31, 287,
	35, 298,
	36, 305,
	35, 316,
	31, 327,
	25, 337,
	20, 348,
	15, 358,
	10, 368,
	8, 373,
	8, 380,
	13, 385,
	18, 391,
	24, 398,
	26, 403,
	25, 408,
	22, 413,
	18, 419,
	13, 425,
	6, 437,
	4, 477,
	5, 510,
	4, 550,
	5, 568,
	24, 579,
	50, 594,
	65, 605,
	88, 622,
	110, 621,
	133, 622,
	157, 622,
	181, 622,
	202, 622,
	217, 622
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
	int firstRec[10] = {
	329, 603,
	329, 212,
	312, 212,
	312, 605,
	329, 605
	};
	int secondRec[10] = {
	312, 210,
	312, 152,
	330, 152,
	330, 211,
	315, 211
	};
	int thirdRec[8] = {
	311, 152,
	330, 150,
	323, 105,
	313, 144
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
	void ChangeMap();
	PhysBody* firstRecChain;
	PhysBody* secondRecChain;
	PhysBody* thirdRecChain;
	Sensors* CreateSensor(PhysBody* b, PhysBody* c, iPoint p, Module* l);
};

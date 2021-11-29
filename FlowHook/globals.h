#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include "Driver.h"
#include "offsets.h"
#include "imgui/imgui.h"
#include "defs.h"

struct globalVars
{
	HWND hWnd;
	uintptr_t pID;
	uintptr_t moduleBase;
	uintptr_t localPlayer;
	std::vector<uintptr_t> entityList;
	uintptr_t nameList;
	uintptr_t viewRenderer;
	uintptr_t currentAimTarget;
	viewMatrix_t viewMatrix;

	POINT windowWH = { 0, 0 };

	ImVec4 fovCircleColor = { 0.f, 0.f, 0.f, 1.f };;
	ImVec4 espColorVisible = { 1.f, 1.f, 1.f, 1.f };
	ImVec4 espColorInvisible = { 1.f, 1.f, 1.f, 1.f };
	ImVec4 espFillColorVisible = { 0.f, 1.f, 0.f, .039f };
	ImVec4 espFillColorInvisible = { 1.f, 0.f, 0.f, .039f };
	ImVec4 textColorVisible = { 1.f, 1.f, 1.f, 1.f };
	ImVec4 textColorInvisible = { 1.f, 1.f, 1.f, 1.f };
	ImVec4 headCircleColorVisible = { 0.f, 1.f, .1f, 1.f };
	ImVec4 headCircleColorInvisible = { 1.f, .1f, 0.f, 1.f };
	ImVec4 headLineColor = { 1.f, 0.f, 0.f, 1.f };
	ImVec4 CrosshairColor = { 1.f, 1.f, 1.f, 1.f };

	ImVec4 healthcol = { 0.f, 1.f, 0.f, 1.f };
	ImVec4 shieldcol = { 0.f, 0.f, 1.f, 1.f };

	ImVec4 SnapColor = { 1.f, 0.f, 0.f, 1.f };
	ImVec4 SnapColorv = { 0.f, 1.f, 0.f, 1.f };

	bool rcs = true;								// Recoil Control System Toggle
	bool esp = true;								// Player Box ESP Toggle
	bool itemESP = true;							// Item ESP Toggle
	bool menuActive = false;						// imgui Toggle
	bool fillBox = false;

	bool Boxesp = true;
	bool Circleesp = false;
	bool Crosshair = true;
	bool Snapeline = true;

	float rcsIntensity = 100.f;
	float maxBoxDistance = 500.f;
	float SnaplineDist = 200.f;
	float maxTextDistance = 300.f;
	float maxHealthDistance = 150.f;
	float maxHeadCircleDistance = 150.f;
	float maxHeadLineDistance = 150.f;
	int espThickness = 2;
	int headLineThickness = 2;

	/* Aimbot */
	bool aimbot = true;
	bool aimbotting = false;
	bool Prediction = false;
	bool aimbotVisibleCheck = true;
	bool fov = false;
	float aimbotFOV = 300.f;
	float aimbotSmoothness = 10.f;

	int aimbotKeyID = 0;
	const char * aimbotKeys[3] = { "Right Mouse", "Left Alt", "Left Mouse"}; // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

	int aimbotOffset = 0;
	const char* Bonez[5] = { "Head", "Neck", "Chest", "Stomach", "Random" };

	int SnapSelect = 0;
	const char* SnapSel[3] = { "Top", "Crosshair", "Bottom" };

	int Boxnum = 1;
	const char* Boxtype[2] = { "Normal", "Cornered"};

	int Healthnum = 1;
	const char* Healthtype[2] = { "Bar", "Number" };

	/* Chams */
	const char* playerGlowStyles[24]{	"scan(animated, wall)",
										"hologram(animated, wall)",
										"hologram2(animated, wall)",
										"scan chams(animated, wall)",
										"Flashing chams(animated, wall)",
										"flashing chams 2(animeted, walls)",
										"highlight outline(animated, wall)",
										"flashing chams(animated)",
										"cham(wall)",
										"bloodhound scan(wall)",
										"bloodhound scan2(wall)",
										"screen effect(wall)",
										"nigger(wall)",
										"normal(wall)",
										"flat chams(wall)",
										"flat chams2 (wall)",
										"flat chams3 (wall)",
										"highlight outline(wall)",
										"buzzy chams(wall)",
										"chams glass",
										"buzzy chams",
										"chams",
										"chams 2",
										"highlight"
	};								
	int playerGlowStyle = 21;
	float PlayerTransperancy = 100.f;

	int CustomPlayerGlowtypes;

	bool playerGlow = true;
	bool playerOutline = false;
	//r, g, b, a
	Color playerGlowColor = { 1.f, 0.f, 0.f };
	Color playerGlowColorv = { 0.f, 1.f, 0.f };
	float PlayerGlowBrightness = 3.f;

	bool localplayerGlow = true;
	bool localplayerOutline = false;
	Color localplayerGlowColorv = { 0.f, 1.f, 0.f };
	float localPlayerGlowBrightness = 2.f;

	const char* localplayerGlowStyles[24]{ "scan(animated, wall)",
										"hologram(animated, wall)",
										"hologram2(animated, wall)",
										"scan chams(animated, wall)",
										"Flashing chams(animated, wall)",
										"flashing chams 2(animeted, walls)",
										"highlight outline(animated, wall)",
										"flashing chams(animated)",
										"cham(wall)",
										"bloodhound scan(wall)",
										"bloodhound scan2(wall)",
										"screen effect(wall)",
										"nigger(wall)",
										"normal(wall)",
										"flat chams(wall)",
										"flat chams2 (wall)",
										"flat chams3 (wall)",
										"highlight outline(wall)",
										"buzzy chams(wall)",
										"chams glass",
										"buzzy chams",
										"chams",
										"chams 2",
										"highlight"
	};
	int localplayerGlowStyle = 20;
	int CustomLocalGlowtypes;
	float LocalTransperancy = 100.f;

	//viewmodel
	bool viewmodelGlow = true;
	bool viewmodelOutline = false;
	int viewmodelGlowType;
	float viewmodelBrightness = 3.f;
	float ViewModelTransperancy = 100.f;
	Color viewmodelColor = { 0.f, 229.f, 255.f };

	int Skin;

	/* Item Glow */
	bool itemGlow = true;
	bool deathBoxESP = true;
	/*
	*  Technically correct but they look bad
	Color heirloomColor = { 1.0f, 0.306f, 0.114f };
	Color legendaryColor = { 0.98f, 0.78f, 0.f };
	Color epicColor = { 0.34f, 0.f, 0.7f };
	Color rareColor = { 0.11f, 0.56f, 1.f };
	Color commonColor = { 0.55f, 0.55f, 0.5f };
	*/
	Color heirloomColor = { 0.5f, 0.f, 0.f };
	Color legendaryColor = { 1.f, 1.f, 0.f };
	Color epicColor = { 1.f, 0.f, 1.f };
	Color rareColor = { 0.f, 1.f, 1.f };
	Color commonColor = { 1.f, 1.f, 1.f };

	/* Third Person */
	bool thirdPerson = true;
	bool Minimap = false;

	/* Speed Hack */
	bool rapidFire = true;
	float rapidFireSpeed = 2.f;

	uintptr_t testPtr = 0x0;
};

extern KDriver Driver;
extern globalVars globals;
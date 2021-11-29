#include "Player.h"
#include "Util.h"

struct visibleTime
{
	float lastTime[100]; // last visible time in float
	bool lastState[100]; // last visibility state
	uintptr_t lastCheck[100]; // last visibility check time
} lastVis;

std::vector<uintptr_t> Player::GetPlayers()
{
	while (true)
	{
		std::vector<uintptr_t> vec;
		uintptr_t EntityList = globals.moduleBase + OFFSET_ENTITYLIST;
		uintptr_t baseent = Driver.rpm<uintptr_t>(EntityList);

		if (baseent == 0)
			continue;

		for (uintptr_t i = 0; i < 100; i++)
		{
			uintptr_t localent = Driver.rpm<uintptr_t>(globals.moduleBase + OFFSET_LOCAL_ENT);
			if (!localent) continue;

			uintptr_t centity = Driver.rpm<uintptr_t>(EntityList + ((uintptr_t)i << 5));
			if (!Player::IsPlayer(centity)) continue;
			int health = Driver.rpm<int>(centity + OFFSET_HEALTH);
			if (health < 1 || Player::GetTeam(centity) == Player::GetTeam(localent)) continue;

			uintptr_t EntityCache{ };
			EntityCache = centity;
			vec.push_back(EntityCache);
		}
		return vec;
	}	
}

int Player::GetTeam(uintptr_t ent)
{
	return Driver.rpm<int>(ent + OFFSET_TEAM);
}

bool Player::IsPlayer(uintptr_t player)
{
	//std::string name = Driver.ReadString(player + OFFSET_NAME);
	//printf("%s", name);
	return (Driver.rpm<uintptr_t>(player + OFFSET_NAME) == 125780153691248);
}

vec3 Player::GetEntityBonePosition(uintptr_t ent, uint32_t BoneId, vec3 BasePosition)
{
	unsigned long long pBoneArray = Player::GetEntityBoneArray(ent);

    vec3 EntityHead = vec3();

	EntityHead.x = Driver.rpm<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
	EntityHead.y = Driver.rpm<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
	EntityHead.z = Driver.rpm<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

	return EntityHead;
}

bool Player::IsDummy(uintptr_t ent)
{
	return Player::GetSignifier(ent) == "npc_dummie";
}

std::string Player::GetSignifier(uintptr_t ent)
{
	uintptr_t sigAddr = Driver.rpm<uintptr_t>(ent + OFFSET_SIGNIFIER);
	return Driver.ReadString(sigAddr, 0xF);
}

bool Player::IsAlive(uintptr_t player)
{
	return (Driver.rpm<int>(player + OFFSET_BLEED_OUT_STATE) == 0 && Driver.rpm<int>(player + OFFSET_LIFE_STATE) == 0);
}

bool Player::IsDead(uintptr_t player)
{
	return Driver.rpm<int>(player + OFFSET_LIFE_STATE) < 0;
}

bool Player::IsValidPlayer(uintptr_t player)
{
	int health = Driver.rpm<int>(player + OFFSET_HEALTH);
	int teamID = Driver.rpm<int>(player + OFFSET_TEAM);

	if (!Player::IsAlive(player)) return false;
	if (health < 0 || health > 100 || teamID < 0 || teamID > 32) return false;

	return true;
}

bool Player::IsValidEnemy(uintptr_t player)
{
	int health = Driver.rpm<int>(player + OFFSET_HEALTH);
	int teamID = Driver.rpm<int>(player + OFFSET_TEAM);

	//if (health < 0 || health > 100 || teamID < 0 || teamID > 32) return false;
	if (teamID == Driver.rpm<int>(globals.localPlayer + OFFSET_TEAM)) return false;
	if (!Player::IsAlive(player)) return false;

	return true;
}

bool Player::IsVisible(uintptr_t ent, int index)
{
	if (Util::GetMs() >= (lastVis.lastCheck[index] + 10)) // only check every 10ms else its gonna flicker
	{
		float visTime = Driver.rpm<float>(ent + OFFSET_VISIBLE_TIME);
		lastVis.lastState[index] = visTime > lastVis.lastTime[index] || visTime < 0.f && lastVis.lastTime[index] > 0.f;
		lastVis.lastTime[index] = visTime;
		lastVis.lastCheck[index] = Util::GetMs();
	}
	return lastVis.lastState[index];
}

vec3 Player::GetViewAngles(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_VIEWANGLES);
}

vec3 Player::GetSwayAngles(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_BREATH_ANGLES);
}

vec3 Player::GetViewAnglesA(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_VIEWANGLES);
}

vec3 Player::GetEntityBasePosition(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_ORIGIN);
}

uintptr_t Player::GetEntityBoneArray(uintptr_t ent)
{
	return Driver.rpm<uintptr_t>(ent + OFFSET_BONES);
}

void Player::SetViewAngles(uintptr_t ent, vec3 angles)
{
	Driver.wpm<vec3>(ent + OFFSET_VIEWANGLES, angles);
}

float Player::LastVisTime(uintptr_t ent)
{
	return Driver.rpm<float>(ent + OFFSET_VISIBLE_TIME);
}

vec3 Player::GetCamPos(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_CAMERAPOS);
}

vec3 Player::GetSwayAnglesA(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_BREATH_ANGLES);
}

vec3 Player::GetRecoil(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_AIMPUNCH);
}

vec3 Player::GetRecoilA(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_AIMPUNCH);
}

vec3 Player::GetAbsVelocity(uintptr_t ent)
{
	return Driver.rpm<vec3>(ent + OFFSET_ABS_VELOCITY);
}

vec3 Player::GetBonePos(uintptr_t ent, int id)
{
	vec3 pos = Driver.rpm<vec3>(ent + OFFSET_ORIGIN);
	uintptr_t bones = Driver.rpm<uintptr_t>(ent + OFFSET_BONES);
	vec3 bone = {};
	UINT32 boneloc = (id * 0x30);
	bone_t boneOffset = {};
	boneOffset = Driver.rpm<bone_t>(bones + boneloc);

	bone.x = boneOffset.x + pos.x;
	bone.y = boneOffset.y + pos.y;
	bone.z = boneOffset.z + pos.z;
	return bone;
}

std::string Player::GetName(uintptr_t player)
{
	uintptr_t nameIndex = Driver.rpm<uintptr_t>(player + OFFSET_NAME_INDEX);
	uintptr_t nameOffset = Driver.rpm<uintptr_t>(globals.nameList + ((nameIndex - 1) << 4));
	std::string name = Driver.ReadString(nameOffset);
	return name;
}

uintptr_t Player::GetBestTarget()
{
	if (Player::IsPlayer(globals.localPlayer))
	{
		float oldDist = FLT_MAX;
		float newDist = 0;
		uintptr_t aimTarget = NULL;

		int index = 0;
		for (uintptr_t& player : Player::GetPlayers())
		{
			bool visible = Player::IsVisible(player, index);
			index++;

			if (!Player::IsValidEnemy(player)) continue;

			vec3 localHead = Driver.rpm<vec3>(globals.localPlayer + OFFSET_CAMERAPOS);
			vec3 targetHead = Player::GetBonePos(player, 8);
			//Player::PredictPos(aimTarget, &targetHead);
			vec2 screenMiddle = { globals.windowWH.x / 2.f, globals.windowWH.y / 2.f };
			vec2 targetHeadScreen;
			if (!Util::WorldToScreen(targetHead, targetHeadScreen)) continue;
			newDist = abs(screenMiddle.Dist2D(targetHeadScreen));

			if (newDist < oldDist && newDist <= globals.aimbotFOV && Util::ToMeters(localHead.DistTo(targetHead)) <= 1000)
			{
				if (globals.aimbotVisibleCheck && !visible) continue;

				oldDist = newDist;
				aimTarget = player;
			}
		}
		return aimTarget;
	}
	else
		return NULL;
}

struct GlowMode {
	uint8_t GeneralGlowMode, // 0x0
		BorderGlowMode, // 0x8
		BorderSize, // 0x10
		TransparentLevel; // 0x18
};

void Player::EnableGlowPlayer(uintptr_t ent)
{
	Color glowcol;
	uint8_t outlinez;

	int x = globals.CustomPlayerGlowtypes;
	uint8_t type = (uint8_t)x;

	int y = globals.PlayerTransperancy;
	uint8_t Localtransperancy = (uint8_t)y;

	int index = 0;
	for (uintptr_t& player : Player::GetPlayers())
	{
		bool visible = Player::IsVisible(player, index);
		index++;

		if (visible) {
			Color glowcol = globals.playerGlowColorv;
		}
		else {
			Color glowcol = globals.playerGlowColor;
		}
	}

	//https://casualhacks.net/apexstuff/highlight-functions.html
	if (!globals.CustomPlayerGlowtypes > 0)
	{
		switch (globals.localplayerGlowStyle) {
		case 0: //scan(animated, wall)
			type = 2;
			break;
		case 1: //hologram(animated, wall)
			type = 103;
			break;
		case 2: //hologram2(animated, wall)
			type = 114;
			break;
		case 3: //scan chams(animated, wall)
			type = 3;
			break;
		case 4: //Flashing chams(animated, wall)
			type = 117;
			break;
		case 5: //flashing chams 2(animeted, walls)
			type = 126;
			break;
		case 6: //highlight outline(animated, wall)
			type = 171;
			break;
		case 7: //flashing chams(animated)
			type = 124;
			break;
		case 8: //cham(wall)
			type = 2;
			break;
		case 9: //bloodhound scan(wall)
			type = 12;
			break;
		case 10: //bloodhound scan2(wall)
			type = 13;
			break;
		case 11: //screen effect(wall)
			type = 115;
			break;
		case 12: //nigger(wall)
			type = 75;
			break;
		case 13: //normal(wall)
			type = 101;
			break;
		case 14: //flat chams(wall)
			type = 137;
			break;
		case 15: //flat chams2 (wall)
			type = 139;
			break;
		case 16: //flat chams3 (wall)
			type = 118;
			break;
		case 17: //highlight outline(wall)
			type = 128;
			break;
		case 18: //buzzy chams 2(wall)
			type = 134;
			break;
		case 19: //chams glass
			type = 136;
			break;
		case 20: //buzzy chams
			type = 133;
			break;
		case 21: //chams
			type = 112;
			break;
		case 22: //chams2
			type = 124;
			break;
		case 23: //highlight
			type = 135;
			break;
		}
	}

	if (globals.playerOutline) {
		outlinez = 40;
	}
	else {
		outlinez = 0;
	}

	if (Player::IsPlayer(ent) || (true && Player::IsDummy(ent))) {
		Driver.wpm<int>(ent + OFFSET_GLOW_ENABLE, globals.playerGlow); // Enable/Disable
		Driver.wpm<int>(ent + OFFSET_GLOW_THROUGH_WALLS, 2); // Through Walls
		Driver.wpm<GlowMode>(ent + 0x2C4, { type, type, outlinez, Localtransperancy });

		Driver.wpm<Color>(ent + OFFSET_GLOW_COLOR, glowcol * globals.PlayerGlowBrightness);
	}
}

void Player::EnableGlowLocalPlayer(uintptr_t ent)
{
	Color glowcol;
	uint8_t outlinez;

	//https://casualhacks.net/apexstuff/highlight-functions.html
	int x = globals.CustomLocalGlowtypes;
	uint8_t type = (uint8_t)x;

	int y = globals.LocalTransperancy;
	uint8_t Localtransperancy = (uint8_t)y;

	if (!globals.CustomLocalGlowtypes > 0)
	{
		switch (globals.localplayerGlowStyle) {
		case 0: //scan(animated, wall)
			type = 2;
			break;
		case 1: //hologram(animated, wall)
			type = 103;
			break;
		case 2: //hologram2(animated, wall)
			type = 114;
			break;
		case 3: //scan chams(animated, wall)
			type = 3;
			break;
		case 4: //Flashing chams(animated, wall)
			type = 117;
			break;
		case 5: //flashing chams 2(animeted, walls)
			type = 126;
			break;
		case 6: //highlight outline(animated, wall)
			type = 171;
			break;
		case 7: //flashing chams(animated)
			type = 124;
			break;
		case 8: //cham(wall)
			type = 2;
			break;
		case 9: //bloodhound scan(wall)
			type = 12;
			break;
		case 10: //bloodhound scan2(wall)
			type = 13;
			break;
		case 11: //screen effect(wall)
			type = 115;
			break;
		case 12: //nigger(wall)
			type = 75;
			break;
		case 13: //normal(wall)
			type = 101;
			break;
		case 14: //flat chams(wall)
			type = 137;
			break;
		case 15: //flat chams2 (wall)
			type = 139;
			break;
		case 16: //flat chams3 (wall)
			type = 118;
			break;
		case 17: //highlight outline(wall)
			type = 128;
			break;
		case 18: //buzzy chams 2(wall)
			type = 134;
			break;
		case 19: //chams glass
			type = 136;
			break;
		case 20: //buzzy chams
			type = 133;
			break;
		case 21: //chams
			type = 112;
			break;
		case 22: //chams2
			type = 124;
			break;
		case 23: //highlight
			type = 135;
			break;
		}
	}
	if (globals.localplayerOutline) {
		outlinez = 40;
	}
	else {
		outlinez = 0;
	}

	if (Player::IsPlayer(ent) || (true && Player::IsDummy(ent))) {
		Driver.wpm<int>(ent + OFFSET_GLOW_ENABLE, globals.localplayerGlow); // Enable/Disable
		Driver.wpm<int>(ent + OFFSET_GLOW_THROUGH_WALLS, 2); // Through Walls
		Driver.wpm<GlowMode>(ent + 0x2C4, { type, type, outlinez, Localtransperancy });

		Driver.wpm<Color>(ent + OFFSET_GLOW_COLOR, globals.localplayerGlowColorv * globals.localPlayerGlowBrightness);
	}
}

void Player::ViewModelGlow(uintptr_t ent)
{
	for (int i = 0; i < globals.entityList.size(); i++) {
			uintptr_t ent = globals.entityList[i];
			if (ent == NULL) continue;

			int b = globals.viewmodelGlowType;
			uint8_t type = (uint8_t)b;
			int y = globals.ViewModelTransperancy;
			uint8_t trasnperancy = (uint8_t)y;
			uint8_t outlinez;

			if (globals.viewmodelOutline) { outlinez = 40; }
			else { outlinez = 0; }

			std::string signifer = Player::GetSignifier(ent);
			if (signifer != xorstr_("viewmodel"))
				continue;

			if (signifer == xorstr_("viewmodel")) {
				Driver.wpm<int>(ent + OFFSET_GLOW_ENABLE, globals.viewmodelGlow); // Glow Enable
				Driver.wpm<int>(ent + OFFSET_GLOW_THROUGH_WALLS, 2); // Through Walls
				Driver.wpm<GlowMode>(ent + OFFSET_GLOW_MODE, { type,type,outlinez,trasnperancy });
				Driver.wpm<Color>(ent + OFFSET_GLOW_COLOR, globals.viewmodelColor * globals.viewmodelBrightness);
			}
		}
}
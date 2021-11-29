#pragma once
#include "globals.h"
#include "defs.h"
#include <string>
#include <vector>

namespace Player
{
	std::vector<uintptr_t> GetPlayers();
	bool IsPlayer(uintptr_t player);
	bool IsDummy(uintptr_t ent);
	std::string GetSignifier(uintptr_t ent);
	bool IsAlive(uintptr_t player);
	bool IsDead(uintptr_t player);
	bool IsValidPlayer(uintptr_t player);
	bool IsValidEnemy(uintptr_t player);
	bool IsVisible(uintptr_t ent, int index);
	std::string GetName(uintptr_t player);
	uintptr_t GetBestTarget();
	vec3 GetViewAngles(uintptr_t ent);
	vec3 GetSwayAngles(uintptr_t ent);
	vec3 GetViewAnglesA(uintptr_t ent);
	vec3 GetEntityBasePosition(uintptr_t ent);
	uintptr_t GetEntityBoneArray(uintptr_t ent);
	void SetViewAngles(uintptr_t ent, vec3 angles);
	float LastVisTime(uintptr_t ent);
	vec3 GetCamPos(uintptr_t ent);
	vec3 GetSwayAnglesA(uintptr_t ent);
	vec3 GetRecoil(uintptr_t ent);
	vec3 GetRecoilA(uintptr_t ent);
	vec3 GetAbsVelocity(uintptr_t ent);
	vec3 GetBonePos(uintptr_t ent, int id);
	int GetTeam(uintptr_t ent);
	vec3 GetEntityBonePosition(uintptr_t ent, uint32_t BoneId, vec3 BasePosition);
	void EnableGlowPlayer(uintptr_t ent);
	void EnableGlowLocalPlayer(uintptr_t ent);
	void ViewModelGlow(uintptr_t ent);
}
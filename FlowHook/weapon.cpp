#include "weapon.h"
#include "globals.h"

void WeaponXEntity::update(uintptr_t LocalPlayer)
{
	uintptr_t entitylist = globals.moduleBase + OFFSET_ENTITYLIST;
	uintptr_t wephandle = Driver.rpm<uintptr_t>(LocalPlayer + OFFSET_CURRENT_WEAPON /*active weapon*/);

	wephandle &= 0xffff;

	uintptr_t wep_entity = Driver.rpm<uintptr_t>(entitylist + (wephandle << 5));


	projectile_speed = Driver.rpm<float>(wep_entity + OFFSET_BULLET_SPEED);
	projectile_scale = Driver.rpm<float>(wep_entity + OFFSET_BULLET_GRAVITY);
	zoom_fov = Driver.rpm<float>(wep_entity + OFFSET_ZOOM_FOV);
}

float WeaponXEntity::get_projectile_speed()
{
	return projectile_speed;
}

float WeaponXEntity::get_projectile_gravity()
{
	return 750.0f * projectile_scale;
}

float WeaponXEntity::get_zoom_fov()
{
	return zoom_fov;
}
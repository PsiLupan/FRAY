#include "hitbox.h"

//80008428
void Hitbox_Deactivate(Hitbox* hitbox){
	hitbox->x4_active = false;
}

//80008434
void Hitbox_Activate(Hitbox* hitbox){
	hitbox->x4_active = true;
}

//8007AFC8
void Hitbox_DeactivateByIndex(HSD_GObj* gobj, u32 index){
	Player* player = GOBJ_PLAYER(gobj);
	Hitbox_Deactivate(&player->x914_hitbox[index]);
}

//8007AFF8
void Hitbox_DeactivateAll(HSD_GObj* gobj){
	Player* player = GOBJ_PLAYER(gobj);
	for(u32 i = 0; i < 4; i++){
		Hitbox* h = &player->x914_hitbox[i];
		Hitbox_Deactivate(h);
	}
	player->x2219_flags &= 0xEFu;
}

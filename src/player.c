#include "player.h"

//800872A4
u32 Player_GetInternalID(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->internal_id;
}

//80087460
u32 Player_GetSpawnCount(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->spawn_ctr;
}

//80086BE0
u8 Player_GetPort(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->slot;
}

//80086C0C
u32 Player_GetActState(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->action_state;
}

//800865C0
f32 Player_GetFacing(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->facedir;
}

//800865CC
bool Player_IsInAir(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->in_air;
}

//80087300
u32 Player_GetLastAtkerIdx(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return ply->last_atkeridx;
}

//800872B0
u32* Player_Get20A4(HSD_GObj* gobj){
	Player* ply = (Player*)gobj->data;
	return &ply->unk20A4;
}

//8022697C
u8 Player_GetCollisionBubbleFlags(HSD_GObj* gobj){
	if(GObj_IsPlayer(gobj)){
		Player* ply = (Player*)gobj->data;
		return ply->collbubble_flags;
	}
	return 0;
}
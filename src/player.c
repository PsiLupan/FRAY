#include "player.h"

//800872A4
u32 Player_GetInternalID(struct Player *ply){
	return ply->internal_id;
}

//80087460
u32 Player_GetSpawnCount(struct Player *ply){
	return ply->spawn_ctr;
}

//80086BE0
u8 Player_GetPort(struct Player *ply){
	return ply->slot;
}

//80086C0C
u32 Player_GetActState(struct Player *ply){
	return ply->action_state;
}

//800865C0
f32 Player_GetFacing(struct Player *ply){
	return ply->facedir;
}

//800865CC
bool Player_IsInAir(struct Player *ply){
	return ply->in_air;
}

//80087300
u32 Player_GetLastAtkerIdx(struct Player *ply){
	return ply->last_atkeridx;
}

//800872B0
u32* Player_Get20A4(struct Player *ply){
	return &ply->unk20A4;
}

//8022697C
u8 Player_GetCollisionBubbleFlags(struct Entity *ent){
	if(Entity_IsPlayer(ent)){
		struct Player* ply = (struct Player*)ent->entity_data;
		return ply->collbubble_flags;
	}
	return 0;
}
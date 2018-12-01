#include "player.h"

//8007D174
void sub_8007D174(Player* player, f32 vel, f32 mobility_1, f32 mobility_2, f32 friction){

}

//8007D264
void Player_UpdateHorzVelocity(Player* player){
	Player_CalculateHorzMobility(player, player->vel_self.x);
}

//8007D28C
void Player_CalculateHorzMobility(Player* player, f32 hVel){
	f32 mobility_1 = (player->joystick_x * player->attribs.airMobilityA) + player->attribs.airMobilityB;
	f32 mobility_2 = (player->joystick_x * player->attribs.airMaxHorzSpeed);
	sub_8007D174(player, hVel, mobility_1, mobility_2, player->attribs.airFriction);
}

//8007D440
void Player_ClampHorzVelocity(Player* player, f32 vel){
	f32 hVel = player->vel_self.x;
	if(hVel < -vel){
		player->vel_self.x = -vel;
	}else{
		if(hVel > vel)
			player->vel_self.x = vel;
	}
}

//8007D468
void Player_ClampMaxHorzVelocity(Player* player){
	Player_ClampHorzVelocity(player, player->attribs.airMaxHorzSpeed);
}

//8007D4E4
void Player_UpdateVelocityFromFastFall(Player* player){
	player->vel_self.y = -player->attribs.fallTermVel;
}

//8007D4F4
void Player_ClampVertVelocity(Player* player, f32 vel){
	if(player->vel_self.y > vel){
		player->vel_self.y = vel;
	}
}

//8007D5BC
void Player_ECBBottom_EnableUpdate(Player* player){
	player->ecb_inactive_frames = 0;
	player->unk820 &= 0xFFFFFFEF;
}

//8007D5D4
void Player_LoseGroundJump_ECBDisable(Player* player){
	player->in_air = TRUE;
	player->x_vel_ground_self = 0;
	player->unkA0 = 0;
	player->pos.z = 0;
	player->jumps_used = 1;
	player->ecb_inactive_frames = 10;
	player->unk820 |= 0x10u;
}

//8007D60C
void Player_LoseAllJumps_ECBDisable(Player* player){
	//TODO
}

//8007D698
void Player_UseAllJumps(Player* player){
	player->jumps_used = player->attribs.maxJumps;
}

//8007D964
f64 Player_GetJoystickAngle_AbsX(Player* player){
	return atan2(player->joystick_y, fabs(player->joystick_x));
}

//8007D99C
f64 Player_GetCStickAngle_AbsX(Player* player){
	return atan2(player->cstick_y, fabs(player->cstick_x));
}

//8007D9D4
f64 Player_GetJoystickAngle(Player* player){
	return atan2(player->joystick_y, player->joystick_x);
}

//8007D9FC
void Player_UpdateFacingFromStick(Player* player){
	if(player->joystick_x < 0){
		player->facedir = -1.0f;
	}else{
		player->facedir = 1.0f;
	}
}

//8007E2F4
void Player_SetGrabbableFlags(Player* player, u16 flags){
	player->grabbable_flags = flags;
}

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
#include "player.h"

#include "actionstate.h"

static f32* r13_514C;

//8004CBE8
void Player_MoveGroundTypeOffset(Player* player)
{
    player->x6F0_physics.x3C_floor_id_actionstate = player->x6F0_physics.x14C_floor_id;
}

//8004CBF4
void Player_SetInvalidGround(Player* player)
{
    player->x6F0_physics.x3C_floor_id_actionstate = -1;
}

//800693AC
void Player_ChangeActionState(HSD_GObj* gobj, u32 state, u32 flags, HSD_GObj* gobj_2, f32 start_frame, f32 frame_rate, f32 unk3)
{
}

//80074A4C
void Player_SetCharacterFlags(HSD_GObj* gobj, u32 offset, u32 flags)
{
    Player* player = GOBJ_PLAYER(gobj);
    *(u32*)(player + offset * 2 + 0x5F4) = flags; //TODO: Not rely on set offsets if possible
    player->x221D_flags = (player->x221D_flags & 0xDF) | 0x20;
}

//8007500C
u32 Player_BoneID2Index(Player* player, u32 bone_id)
{
    /*u32* r3 = -0x515C(r13)*/

    /*Points to an array pointers that go to the below struct
	static bone_id* bone_ids[ACTIVE_OBJECTS];

	Which points to struct:
	struct {
		u8* unk_ids;
		u8* bone_ids;
		u32 num_bones;
	}
	*/

    /*u32* result_1 = (u32*)(*((u32*)(r3 + (player->x4_internal_id * 4))));
	u8* bones = (result_1 + 0x4);
	return bone[bone_id];
	*/
    return 0;
}

//8007D174
void Player_CalculateAirMobility(Player* player, f32 vel, f32 mobility_1, f32 mobility_2, f32 friction)
{
    if (mobility_2 != 0.f) {
        if ((vel * mobility_1) > 0.f) {
            if (mobility_1 <= 0.f) {
                if ((vel + mobility_1) < mobility_2) {
                    mobility_1 = friction;
                    if (mobility_2 < (vel + friction)) {
                        mobility_1 = mobility_2 - vel;
                    }
                    if ((vel + mobility_1) < -player->attribs.horizontalAirMobility) {
                        mobility_1 = -player->attribs.horizontalAirMobility - vel;
                    }
                }
            } else if (mobility_2 < (vel + mobility_1)) {
                mobility_1 = -friction;
                if ((vel + mobility_1) < mobility_2) {
                    mobility_1 = mobility_2 - vel;
                }
                if (player->attribs.horizontalAirMobility < (vel + mobility_1)) {
                    mobility_1 = player->attribs.horizontalAirMobility - vel;
                }
            }
        }
        player->x74_mobility = mobility_1;
    } else {
        f32 x_vel = player->x80_x88_self_vel.x;
        if (x_vel < 0) {
            x_vel = -x_vel;
        }

        f32 frict = friction;
        if (frict < 0) {
            frict = -frict;
        }

        if (frict < x_vel) {
            if (player->x80_x88_self_vel.x > 0) {
                friction = -friction;
            }
        } else {
            friction = -player->x80_x88_self_vel.x;
        }
        player->x74_mobility = friction;
    }
}

//8007D268
void Player_UpdateHorzVelocity(Player* player)
{
    Player_CalculateHorzMobility(player, player->x80_x88_self_vel.x);
}

//8007D28C
void Player_CalculateHorzMobility(Player* player, f32 hVel)
{
    f32 mobilityB = 0.f;
    if(0.f <= player->x620_joystick_x){
        mobilityB = -player->attribs.airMobilityB;
    }else{
        mobilityB = player->attribs.airMobilityB;
    }
    f32 mobility_1 = (player->x620_joystick_x * player->attribs.airMobilityA) + mobilityB;
    f32 mobility_2 = (player->x620_joystick_x * player->attribs.airMaxHorzSpeed);
    Player_CalculateAirMobility(player, hVel, mobility_1, mobility_2, player->attribs.airFriction);
}

//8007D440
void Player_ClampHorzVelocity(Player* player, f32 vel)
{
    f32 hVel = player->x80_x88_self_vel.x;
    if (hVel < -vel) {
        player->x80_x88_self_vel.x = -vel;
    } else {
        if (hVel > vel) {
            player->x80_x88_self_vel.x = vel;
        }
    }
}

//8007D468
void Player_ClampMaxHorzVelocity(Player* player)
{
    Player_ClampHorzVelocity(player, player->attribs.airMaxHorzSpeed);
}

//8007D494
void Player_UpdateFallingVelocity(Player* player, f32 gravity, f32 term_vel)
{
    f32 vVel = player->x80_x88_self_vel.y;
    term_vel = -term_vel;
    vVel = vVel - gravity;
    player->x80_x88_self_vel.y = vVel;
    if (player->x80_x88_self_vel.y < term_vel) {
        player->x80_x88_self_vel.y = term_vel;
    }
}

//8007D4E4
void Player_UpdateVelocityFromFastFall(Player* player)
{
    player->x80_x88_self_vel.y = -player->attribs.fallTermVel;
}

//8007D4F4
void Player_ClampVertVelocity(Player* player, f32 vel)
{
    if (player->x80_x88_self_vel.y > vel) {
        player->x80_x88_self_vel.y = vel;
    }
}

//8007D508
void Player_UpdateAscendingVelocity(Player* player, f32 vel, f32 term_vel)
{
    f32 vVel = player->x80_x88_self_vel.y;
    vVel = vVel + vel;
    player->x80_x88_self_vel.y = vVel;
    if (player->x80_x88_self_vel.y > term_vel) {
        player->x80_x88_self_vel.y = term_vel;
    }
}

//8007D528
BOOL Player_Interrupt_Fastfall(Player* player)
{
    if ((player->x221A_flags >> 3) & 1) {
        return FALSE;
    }
    if (player->x80_x88_self_vel.y >= 0.0f) {
        return FALSE;
    }
    //f32* unk = r13_514C + 0x88;
    if (player->x624_joystick_y > -0.6625f || player->x671_frames_tilt_y >= 4) { //Values here are hardcoded - Need to account for individual character data
        return FALSE;
    }
    player->x221A_flags = (player->x221A_flags & 0xF7) | 8;
    player->x671_frames_tilt_y = -2;
    Player_PlaySFX(player, 0x96, 0x7F, 0x40);
    return TRUE;
}

//8007D5BC
void Player_ECBBottom_EnableUpdate(Player* player)
{
    player->x88C_ecb_inactive_frames = 0;
    player->x6F0_physics.x130_unk &= 0xFFFFFFEF;
}

//8007D5D4
void Player_LoseGroundJump_ECBDisable(Player* player)
{
    player->xE0_in_air = TRUE;
    player->xEC_vel_ground_self_x = 0;
    player->xA0_unk = 0;
    player->xB0_xB8_pos.z = 0;
    player->x1968_jumps_used = 1;
    player->x88C_ecb_inactive_frames = 10;
    player->x6F0_physics.x130_unk |= 0x10;
}

//8007D60C
void Player_LoseAllJumps_ECBDisable(Player* player)
{
    if ((player->x2227_flags >> 7) != 0 && player->x1968_jumps_used < 2) {
        //sub_8003FC44(player->slot, (player->x221F_flags >> 3) & 1);
    }
    player->xE0_in_air = TRUE;
    player->xEC_vel_ground_self_x = 0.f;
    player->x78_unk = 0.f;
    player->x1968_jumps_used = player->attribs.maxJumps;
    player->x88C_ecb_inactive_frames = 5;
    player->x6F0_physics.x130_unk |= 0x10;
}

//8007D698
void Player_UseAllJumps(Player* player)
{
    player->x1968_jumps_used = player->attribs.maxJumps;
}

//8007D7FC
void Player_SetGroundedState(Player* player)
{
}

//8007D964
f64 Player_GetJoystickAngle_AbsX(Player* player)
{
    return atan2(player->x624_joystick_y, fabs(player->x620_joystick_x));
}

//8007D99C
f64 Player_GetCStickAngle_AbsX(Player* player)
{
    return atan2(player->x63C_cstick_y, fabs(player->x638_cstick_x));
}

//8007D9D4
f64 Player_GetJoystickAngle(Player* player)
{
    return atan2(player->x624_joystick_y, player->x620_joystick_x);
}

//8007D9FC
void Player_UpdateFacingFromStick(Player* player)
{
    if (player->x620_joystick_x < 0) {
        player->x2C_facedir = -1.0f;
    } else {
        player->x2C_facedir = 1.0f;
    }
}

//8007E2F4
void Player_SetGrabbableFlags(Player* player, u16 flags)
{
    player->x1A64_grabbable_flags = flags;
}

//8007EFC0
void Player_SetIndicatorDisplay(Player* player, u16 flags)
{
    player->x209A_flags = flags;
}

//80081D0C
BOOL Player_CollisonCheck_Ground(HSD_GObj* gobj)
{
    BOOL collided = FALSE;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    collided = Physics_CollisionCheck_Ground(&ply->x6F0_physics);

    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (sub_80081A00(gobj) == TRUE) {
        return collided;
    }
    return FALSE;
}

//80083F88
void Player_Collision_KneeBend(HSD_GObj* gobj)
{
    u32 res = 0;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    res = Physics_FallOffLedge(&ply->x6F0_physics);
    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (res == 0) {
        ActionState_Fall(gobj);
    }
}

//80084DB0
void Player_CheckFastFallAndUpdate(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    Player_Interrupt_Fastfall(ply);
    if ((ply->x221A_flags >> 3) & 1) {
        Player_UpdateVelocityFromFastFall(ply);
    } else {
        Player_UpdateFallingVelocity(ply, ply->attribs.gravity, ply->attribs.termVel);
    }
    Player_UpdateHorzVelocity(ply);
}

//80085FD4
u32* Player_FetchAnimHeader(Player* player, u32 anim_id)
{
    return NULL;
    /*if(player->x4_internal_id == INTERNAL_NANA){
		u32 type = StaticPlayer_GetSlotType(player->xC_slot);
		if(type != 2){
			if(player->x24_state_ptr[anim_id]->x14_unk == 0){
				return *((u32*)((*(u32**)(0x804598B8 + 0x2C)) + 0xC)[anim_id]);
			}
		}
	}
	return *(u32*)(player->x24_state_ptr[anim_id]);*/
}

//800865C0
f32 Player_GetFacing(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x2C_facedir;
}

//800865CC
BOOL Player_IsInAir(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->xE0_in_air;
}

//80086BE0
u8 Player_GetPort(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->xC_slot;
}

//80086C0C
u32 Player_GetActionState(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x10_action_state;
}

//800872A4
u32 Player_GetInternalID(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x4_internal_id;
}

//800872B0
u32* Player_Get20A4(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return &ply->x20A4_unk;
}

//80087300
u32 Player_GetLastAtkerIdx(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x18C4_last_atkeridx;
}

//80087460
u32 Player_GetSpawnCount(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    return ply->x8_spawn_ctr;
}

//80088148
void Player_PlaySFX(Player* player, u32 pitch_id, u32 unk1, u32 unk2)
{
}

//8008A4D4
void Player_Interrupt_Wait()
{
}

//80094818
void Player_80094818(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    if (ply->x1978_unk == 0) {
        if (ply->x1974_held_item != NULL) {
            /*if(OnItemPickup_Func[ply->x4_internal_id] != NULL){
                (*OnItemPickup_Func[ply->x4_internal_id])();
            }
            sub_8003E17C(player->slot, (player->field_0x221f >> 3) & 1, player->x1974_held_item);
            */
        } else {
            //sub_8003E17C(player->slot, (player->field_0x221f >> 3) & 1, NULL);
        }
    }
}

//800A2040
BOOL Player_IsCPU(Player* player)
{
    if (sub_8003248C(player->xC_slot, (player->x221F_flags >> 3) & 1) == TRUE) {
        return player->x1A94_cpu_flags != 5;
    }
    return FALSE;
}

//800D65B8
void Player_Collision_SquatWait(HSD_GObj* gobj)
{
    u32 res = 0;

    Player* ply = GOBJ_PLAYER(gobj);
    ply->x6F0_physics.x1C_x24_TopN_prev = ply->x6F0_physics.x4_xC_TopN;
    ply->x6F0_physics.x4_xC_TopN = ply->xB0_xB8_pos;
    res = Physics_FallOffLedge(&ply->x6F0_physics);
    ply->xB0_xB8_pos = ply->x6F0_physics.x4_xC_TopN;
    if (res == 0) {
        ActionState_Fall(gobj);
    }
}

//800DE9B8
BOOL Player_IsDPadUpInstantPressed(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    return (player->x668_instant_buttons & 8) != 0;
}

//8011D518
BOOL Player_SwapItem(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    player->x2238_flags = 0;
    player->x21E8_func = NULL;
    player->x21DC_Player_OnHit = NULL;
    HSD_GObj* item = player->x223C_pend_item;
    if (item != NULL) {
        player->x1974_held_item = item;
        player->x223C_pend_item = NULL;
        sub_8026BB20(player->x1974_held_item);
        sub_8026B73C(player->x1974_held_item);
        sub_80094818(gobj, 1);
    }
    return item != NULL;
}

//8022697C
u8 Player_GetCollisionBubbleFlags(HSD_GObj* gobj)
{
    if (GObj_IsPlayer(gobj)) {
        Player* ply = GOBJ_PLAYER(gobj);
        return ply->x21FC_collbubble_flags;
    }
    return 0;
}

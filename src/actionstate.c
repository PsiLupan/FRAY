#include "actionstate.h"

//8007D92C
void ActionState_FallOrWait(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	if(ply->xE0_in_air == TRUE){
		ActionState_Fall(gobj);
		return;
	}
	ActionState_Wait_CheckPlayer(gobj);
}

//8008732C
BOOL ActionState_IsDead(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= 0 && state <= 10){
		return TRUE;
	}
	return FALSE;
}

//80087354
BOOL ActionState_IsDeadUp(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= 3 && state <= 10){
		return TRUE;
	}
	return FALSE;
}

//8008737C
BOOL ActionState_IsDeadStarUp(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= 4 && state <= 5){
		return TRUE;
	}
	return FALSE;
}

//800873A4
BOOL ActionState_IsDeadUpFall(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= 6 && state <= 10){
		return TRUE;
	}
	return FALSE;
}

//800873CC
BOOL ActionState_IsRebirth(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= 12 && state <= 13){
		return TRUE;
	}
	return FALSE;
}

//800873F4
BOOL ActionState_IsEntry(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 state = ply->x10_action_state;
	if(state >= ACTIONSTATE_ENTRY && state <= ACTIONSTATE_ENTRYEND){
		return TRUE;
	}
	return FALSE;
}

//8008A2BC
void ActionState_Wait_CheckPlayer(HSD_GObj* gobj){
	if(Player_GetInternalID(gobj) == INTERNAL_MASTERHAND){
		sub_80151018(gobj);
		return;
	}
	if(Player_GetInternalID(gobj) == INTERNAL_CRAZYHAND){
		sub_8015BC88(gobj);
		return;
	}
	ActionState_Wait(gobj, 0.0f);
}

//8008A324
void ActionState_Wait_NoCheck(HSD_GObj* gobj){
	ActionState_Wait(gobj, -1.0f);
}

//8008A348
void ActionState_Wait(HSD_GObj* gobj, const f32 unk){
	Player* ply = GOBJ_PLAYER(gobj);

	if(((ply->x2224_flags >> 5) & 1) != 0){
		ActionState_800C8B74(gobj);
		return;
	}
	if(Item_PlayerHasHammer(gobj) == TRUE){
		ActionState_HammerWait(gobj);
		return;
	}
	if(ply->xE0_in_air == TRUE){
		sub_8007D7FC(ply);
	}
	if(ply->x4_internal_id == INTERNAL_PEACH){
		HSD_GObj* held = ply->x1974_held_item;
		if(held != NULL){
			if(Item_GetItemType(held) == ITEM_PEACH_PARASOL){
				Peach_RemoveParasol(held);
			}
		}
	}

	Player_ChangeActionState(gobj, ACTIONSTATE_WAIT, 0, NULL, 0.0f, 1.0f, unk);

	if(sub_8008A6989(ply) != 0){
		u32* unk_struct = NULL; //Player_80085FD4(ply, 6);
		if(unk_struct[2] != 0){
			sub_8008A6D8(gobj, 6);
		}
	}

	Player_SetIndicatorDisplay(ply, 32); //(r13 - 0x514C)->x5F0, but we'll hardcode the value till we understand it better
	u32 char_id = ply->x4_internal_id;
	if(char_id == INTERNAL_YLINK){
		sub_8014919C(gobj);
		return;
	}else if(char_id == INTERNAL_LINK){
		sub_800EB3BC(gobj);
		return;
	}
}

//800C4ED8
void ActionState_HammerWait(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	u32 hammer_state = (u32)ActionState_IsHammer(ply);
	f32 hammer_frames = ActionState_HammerFrameCount(ply);
	Player_ChangeActionState(gobj, ACTIONSTATE_HAMMERWAIT, hammer_state, NULL, hammer_frames, 1.0f, 0.0f);
	sub_800C5408(ply); //Changes Player Overlay Color
	ActionState_Hammer_ActivateHitbox(ply); //Relates to setting hitbox values for Hammer
	sub_8007EBAC(ply, 1, 0); //Makes the player's controller rumble
}

//800C548C
f32 ActionState_HammerFrameCount(Player* ply){
	u32 state = ply->x10_action_state;
	if(state >= ACTIONSTATE_HAMMERWAIT && state <= ACTIONSTATE_HAMMERLANDING){
		return ply->x894_action_state_frames;
	}
	return 0.0f;
}

//800C53E4
//800C54C4
BOOL ActionState_IsHammer(Player* ply){
	u32 state = ply->x10_action_state;
	if(state >= ACTIONSTATE_HAMMERWAIT && state <= ACTIONSTATE_HAMMERLANDING){
		return TRUE;
	}
	return FALSE;
}

//800C5588
void ActionState_Hammer_ActivateHitbox(Player* ply){
	sub_8026C16C(ply->x1974_held_item, 1);
	ply->x2219_flags = (ply->x2219_flags & 0xF7) | 8;
}

//800C5D34
void ActionState_HammerFall(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	Player_ChangeActionState(gobj, ACTIONSTATE_HAMMERFALL, ActionState_IsHammer(ply), NULL, ActionState_HammerFrameCount(ply), 1.0f, 0.0f);
	if(ply->xE0_in_air == FALSE)
		Player_LoseGroundJump_ECBDisable(ply);
	sub_800C4E94(ply);
}

//800C8B74
void ActionState_800C8B74(HSD_GObj* gobj){
	Player* player = GOBJ_PLAYER(gobj);
	u32* ustruct = NULL; //Player_80085FD4(player, 0xBE);

	u32 state;
	if((u8*)ustruct[0x14] == 0){
		state = 198;
	}else{
		state = 190;
	}
	Player_ChangeActionState(gobj, state, 0, NULL, 0.0f, 1.0f, 0.0f);
	sub_8006EBA4(gobj);
	Player_SetGrabbableFlags(player, 0x1FF);
	player->x2344_flags = 0;
}

//800CC730
void ActionState_Fall(HSD_GObj* gobj){
	if(Player_GetInternalID(gobj) == INTERNAL_MASTERHAND){
		sub_80151018(gobj);
		return;
	}
	if(Player_GetInternalID(gobj) == INTERNAL_CRAZYHAND){
		sub_8015BC88(gobj);
		return;
	}
	
	Player* ply = GOBJ_PLAYER(gobj);
	BOOL nofall = FALSE;
	if((ply->x2224_flags >> 5) & 1){
		ActionState_DamageFall(gobj);
		nofall = TRUE;
	}else{
		if(Item_PlayerHasHammer(gobj) == TRUE){
			ActionState_HammerFall(gobj);
			nofall = TRUE;
		}else{
			nofall = FALSE;
		}
	}
	if(nofall == FALSE){
		Player_ChangeActionState(gobj, ACTIONSTATE_FALL, 1, NULL, 0.0f, 1.0f, 0.0f);
		Player_ClampMaxHorzVelocity(ply);
		ply->x2340_flags = 20;
		ply->x2344_flags = 0.0f;
		if(ply->xE0_in_air == FALSE)
			Player_LoseGroundJump_ECBDisable(ply);
	}
}

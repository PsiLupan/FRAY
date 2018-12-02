#include "actionstate.h"

//8007D92C
void ActionState_FallOrWait(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	if(ply->xE0_in_air == TRUE){
		return ActionState_Fall(gobj);
	}
	return /*ActionState_Wait(gobj)*/;
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

//800C548C
f32 ActionState_HammerFrameCount(Player* ply){
	u32 state = ply->x10_action_state;
	if(state >= ACTIONSTATE_HAMMERWAIT && state <= ACTIONSTATE_HAMMERLANDING){
		return ply->x894_action_state_frames;
	}
	return 0.0f;
}

//800C54C4
BOOL ActionState_IsHammer(Player* ply){
	u32 state = ply->x10_action_state;
	if(state >= ACTIONSTATE_HAMMERWAIT && state <= ACTIONSTATE_HAMMERLANDING){
		return TRUE;
	}
	return FALSE;
}

//800C5D34
void ActionState_HammerFall(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	Player_ChangeActionState(gobj, ACTIONSTATE_HAMMERFALL, ActionState_IsHammer(ply), NULL, ActionState_HammerFrameCount(ply), 1.0f, 0.0f);
	if(ply->xE0_in_air == FALSE)
		Player_LoseGroundJump_ECBDisable(ply);
	sub_800C4E94(ply);
}

//800CC730
void ActionState_Fall(HSD_GObj* gobj){
	if(Player_GetInternalID(gobj) == INTERNAL_MASTERHAND)
		return sub_80151018(gobj);
	if(Player_GetInternalID(gobj) == INTERNAL_CRAZYHAND)
		return sub_8015BC88(gobj);
	
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
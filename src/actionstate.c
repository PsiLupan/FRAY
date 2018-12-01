#include "actionstate.h"

//8007D92C
void ActionState_FallOrWait(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	if(ply->in_air == TRUE){
		return /*ActionState_Fall(gobj)*/;
	}
	return /*ActionState_Wait(gobj)*/;
}

//8008732C
bool ActionState_IsDead(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 0 && state <= 10){
		return true;
	}
	return false;
}

//80087354
bool ActionState_IsDeadUp(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 3 && state <= 10){
		return true;
	}
	return false;
}

//8008737C
bool ActionState_IsDeadStarUp(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 4 && state <= 5){
		return true;
	}
	return false;
}

//800873A4
bool ActionState_IsDeadUpFall(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 6 && state <= 10){
		return true;
	}
	return false;
}

//800873CC
bool ActionState_IsRebirth(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 12 && state <= 13){
		return true;
	}
	return false;
}

//800873F4
bool ActionState_IsEntry(HSD_GObj *gobj){
	Player* ply = (Player*)gobj->data;
	u32 state = ply->action_state;
	if(state >= 322 && state <= 324){
		return true;
	}
	return false;
}
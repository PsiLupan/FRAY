#include "actionstate.h"

//8008732C
bool ActionState_IsDead(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 0 && state <= 10){
		return true;
	}
	return false;
}

//80087354
bool ActionState_IsDeadUp(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 3 && state <= 10){
		return true;
	}
	return false;
}

//8008737C
bool ActionState_IsDeadStarUp(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 4 && state <= 5){
		return true;
	}
	return false;
}

//800873A4
bool ActionState_IsDeadUpFall(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 6 && state <= 10){
		return true;
	}
	return false;
}

//800873CC
bool ActionState_IsRebirth(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 12 && state <= 13){
		return true;
	}
	return false;
}

//800873F4
bool ActionState_IsEntry(struct Player *ply){
	u32 state = ply->action_state;
	if(state >= 322 && state <= 324){
		return true;
	}
	return false;
}
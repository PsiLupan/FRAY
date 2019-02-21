#ifndef _staticplayer_h_
#define _staticplayer_h_

#include <gctypes.h>

#include "gobj.h"
#include "player.h"
#include "match.h"

#define MAX_PLAYERS 6

typedef struct _StaticPlayer
{
	s32 state;
	u32 character_id;
	u32 slot_type;
	u16 transformed;
	u16 unknown0E;
	f32 nametag_pos[3];
	f32 transformed_pos[3];
	f32 spawnplat_final_pos[3];
	f32 unk_pos[3];
	f32 facedir;
	u8 costume_id;
	u8 unk45;
	u8 controller_idx;
	u8 team_id;
	u8 player_id;
	u8 cpu_level;
	u8 cpu_type;
	u8 handicap;
	f32 unk4C;
	f32 unk50;
	f32 attack_ratio;
	f32 dmgdef_ratio;
	f32 model_scale;
	u16 curr_lost_percent;
	u16 start_alt_percent;
	u16 stamina_hp;
	u32 falls;
	u32 nana_deaths;
	u32 ko_counter[MAX_PLAYERS];
	s32 match_frames;
	u16 suicides;
	u8 stocks;
	u8 unk8F;
	u32 curr_coins;
	u32 total_coins;
	u32 unk98;
	u32 unk9C;
	u32 joymov_count;
	u32 unkA4;
	u32 nametag_slotid;
	u8 multi_spawn;
	u8 unkAD;
	u8 unkAE;
	u8 unkAF;
	HSD_GObj* player;
	HSD_GObj* subchar;
	u32 stale_writeidx;
	u16 stale_moves[20]; //idx 0 move ID, idx 1 # of AS changes in match
	u32 stats[48];
	u32 unkEC;
	
	u32 unk6A8;
} StaticPlayer;

extern StaticPlayer players[MAX_PLAYERS];

u32 StaticPlayer_GetState(u32);
u32 StaticPlayer_GetExternalID(u32);
void StaticPlayer_SetExternalID(u32, u32);
u32 StaticPlayer_GetSlotType(u32);
void StaticPlayer_SetSlotType(u32, u32);
f32* StaticPlayer_GetNametagCoords(u32, f32*);
f32 StaticPlayer_GetFacing(u32);
void StaticPlayer_SetFacing(u32, f32);

u8 StaticPlayer_GetCPULevel(u32);

void StaticPlayer_SetHandicap(u32, u8);

f32 StaticPlayer_Get50(u32);
f32 StaticPlayer_GetAttackRatio(u32);
void StaticPlayer_SetAttackRatio(u32, f32);
f32 StaticPlayer_GetDefenseRatio(u32);
void StaticPlayer_SetDefenseRatio(u32, f32);
f32 StaticPlayer_GetModelScale(u32);
void StaticPlayer_SetModelScale(u32, f32);

u16 StaticPlayer_GetStaminaHP(u32);
void StaticPlayer_SetStaminaHP(u32, u16);

u32 StaticPlayer_GetFalls(u32);
void StaticPlayer_SetFalls(u32, u32);

u32 StaticPlayer_GetKillCount(u32, u32);
s32 StaticPlayer_GetMatchFrames(u32);
void StaticPlayer_SetMatchFrames(u32, bool);
u16 StaticPlayer_GetSuicides(u32);

u8 StaticPlayer_GetStocks(u32);
u8 StaticPlayer_GetPlayer1Stocks();
void StaticPlayer_SetStocks(u32, u32);
u32 StaticPlayer_DecrementStocks(u32);
u32 StaticPlayer_GetCurrentCoins(u32);
void StaticPlayer_SetCurrentCoins(u32, u32);
u32 StaticPlayer_GetTotalCoins(u32);
void StaticPlayer_SetTotalCoins(u32, u32);
u32 StaticPlayer_Get98(u32);
void StaticPlayer_Set98(u32, u32);
u32 StaticPlayer_Get9C(u32);
void StaticPlayer_Set9C(u32, u32);

HSD_GObj* StaticPlayer_GetCharacterGObj(u32, bool);
Player* StaticPlayer_GetPlayerStruct(u32, bool);
u32* StaticPlayer_GetStaleMoveTable(u32);
u32* StaticPlayer_GetTotalAttacks(u32);

u32* StaticPlayer_Get6A8(u32);

#endif 
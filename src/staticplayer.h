#ifndef _player_h_
#define _player_h_

#include <gctypes.h>

#include "entity.h"

#define MAX_PLAYERS 6

struct StaticPlayer
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
	u32 p1ko_counter;
	u32 p2ko_counter;
	u32 p3ko_counter;
	u32 p4ko_counter;
	u32 p5ko_counter;
	u32 p6ko_counter;
	u32 unk88;
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
	struct Entity* player_ent;
	struct Entity* subchar_ent;
	u32 stale_writeidx;
	u16 stale_moves[20]; //idx 0 move ID, idx 1 # of AS changes in match
	u32 stats[48];
	u32 unkEC;
	
	u32 unk6A8;
};

extern struct StaticPlayer players[MAX_PLAYERS];

extern u32 StaticPlayer_GetState(u32);
extern u32 StaticPlayer_GetExternalID(u32);
extern struct StaticPlayer* StaticPlayer_SetExternalID(u32, u32);
extern u32 StaticPlayer_GetSlotType(u32);
extern struct StaticPlayer* StaticPlayer_SetSlotType(u32, u32);
extern f32* StaticPlayer_GetNametagCoords(u32, f32*);
extern f32 StaticPlayer_GetFacing(u32);
extern struct StaticPlayer* StaticPlayer_SetFacing(u32, f32);

extern u8 StaticPlayer_GetCPULevel(u32);

extern struct StaticPlayer* StaticPlayer_SetHandicap(u32, u8);

extern f32 StaticPlayer_Get50(u32);
extern f32 StaticPlayer_GetAttackRatio(u32);
extern struct StaticPlayer* StaticPlayer_SetAttackRatio(u32, f32);
extern f32 StaticPlayer_GetDefenseRatio(u32);
extern struct StaticPlayer* StaticPlayer_SetDefenseRatio(u32, f32);
extern f32 StaticPlayer_GetModelScale(u32);
extern struct StaticPlayer* StaticPlayer_SetModelScale(u32, f32);

extern u16 StaticPlayer_GetStaminaHP(u32);
extern struct StaticPlayer* StaticPlayer_SetStaminaHP(u32, u16);
extern u32 StaticPlayer_GetFalls(u32);
extern struct StaticPlayer* StaticPlayer_SetFalls(u32, u32);

extern u8 StaticPlayer_GetStocks(u32);
extern u8 StaticPlayer_GetPlayer1Stocks();
extern struct StaticPlayer* StaticPlayer_SetStocks(u32, u32);
extern u32 StaticPlayer_DecrementStocks(u32);
extern u32 StaticPlayer_GetCurrentCoins(u32);
extern struct StaticPlayer* StaticPlayer_SetCurrentCoins(u32, u32);
extern u32 StaticPlayer_GetTotalCoins(u32);
extern struct StaticPlayer* StaticPlayer_SetTotalCoins(u32, u32);
extern u32 StaticPlayer_Get98(u32);
extern struct StaticPlayer* StaticPlayer_Set98(u32, u32);
extern u32 StaticPlayer_Get9C(u32);
extern struct StaticPlayer* StaticPlayer_Set9C(u32, u32);

extern struct Entity* StaticPlayer_GetCharacterEntity(u32, bool);
extern struct Player* StaticPlayer_GetPlayerStruct(u32, bool);
extern u32* StaticPlayer_GetStaleMoveTable(u32);
extern u32* StaticPlayer_GetTotalAttacks(u32);

extern u32* StaticPlayer_Get6A8(u32);

#endif 
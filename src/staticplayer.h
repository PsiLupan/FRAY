#ifndef _player_h_
#define _player_h_

#include <gctypes.h>

#include "entity.h"

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
	u8 unk4A;
	u8 handicap;
	f32 unk4C;
	f32 attack_ratio;
	f32 unk54;
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

u32 StaticPlayer_GetState(u32);
u32 StaticPlayer_GetExternalID(u32);
struct StaticPlayer* StaticPlayer_SetExternalID(u32, u32);
u32 StaticPlayer_GetSlotType(u32);
struct StaticPlayer* StaticPlayer_SetSlotType(u32, u32);
f32* StaticPlayer_GetNametagCoords(u32, f32*);
f32 StaticPlayer_GetFacing(u32);
struct StaticPlayer* StaticPlayer_SetFacing(u32, f32);

f32 StaticPlayer_GetDefenseRatio(u32);
struct StaticPlayer* StaticPlayer_SetDefenseRatio(u32, f32);
f32 StaticPlayer_GetModelScale(u32);
struct StaticPlayer* StaticPlayer_SetModelScale(u32, f32);

u8 StaticPlayer_GetStocks(u32);
u8 StaticPlayer_GetPlayer1Stocks();
u32 StaticPlayer_GetCurrentCoins(u32);
struct StaticPlayer* StaticPlayer_SetCurrentCoins(u32, u32);
u32 StaticPlayer_GetTotalCoins(u32);
struct StaticPlayer* StaticPlayer_SetTotalCoins(u32, u32);

u32* StaticPlayer_GetStaleMoveTable(u32);
u32* StaticPlayer_GetTotalAttacks(u32);

u32* StaticPlayer_Get6A8(u32);

#endif 
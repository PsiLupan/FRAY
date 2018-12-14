#ifndef _player_h_
#define _player_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include <math.h>

#include "gobj.h"
#include "hitbox.h"
#include "subaction.h"

#define GOBJ_PLAYER(s) ((Player*)s->data)

#define BONE_LOOKUP_STRUCT_SIZE 0x10

#define INTERNAL_MARIO 0x00
#define INTERNAL_FOX 0x01
#define INTERNAL_CFALCON 0x02
#define INTERNAL_DKONG 0x03
#define INTERNAL_KIRBY 0x04
#define INTERNAL_MASTERHAND 0x1B
#define INTERNAL_CRAZYHAND 0x1C

typedef struct _Attributes {
	f32 walkInitVel; //0x00
	f32 walkAccel; //0x04
	f32 walkMaxVel; //0x08
	f32 walkAnimScale; //0x0C
	f32 midWalkSpeed; //0x10
	f32 fastWalkSpeed; //0x14
	f32 grFriction; //0x18
	f32 dashInitVel; //0x1C
	f32 turnInitVel; //0x20 - Called Accel A in doc
	f32 turnPeakVel; //0x24 - Called Accel B in doc.
	f32 runInitVel; //0x28
	f32 runAnimScale; //0x2C
	u32 runAccel; //0x30
	f32 grMaxHorzVel; //0x34
	u32 jumpStartFrames; //0x38
	f32 jumpInitHorzVel; //0x3C
	f32 jumpInitVertVel; //0x40
	f32 jumpMomentumMulti; //0x44
	f32 jumpMaxHorzVel; //0x48
	f32 hopInitVertVel; //0x4C
	f32 dJumpVertMulti; //0x50
	f32 dJumpHorzMulti; //0x54
	f32 maxJumps; //0x58
	f32 gravity; //0x5C
	f32 termVel; //0x60
	f32 airMobilityA; //0x64
	f32 airMobilityB; //0x68
	f32 airMaxHorzSpeed; //0x6C
	f32 airFriction; //0x70
	f32 fallTermVel; //0x74
	u32 tiltTurnDuration; //0x78
	f32 jab2InputWindow; //0x7C
	f32 jab3InputWindow; //0x80
	u32 turnFrames;
	f32 weight; //0x88
	f32 modelScale; //0x8C
	f32 shieldSize; //0x90
	f32 shieldBreakInitVel; //0x94
	u32 rapidJabWindow; //0x98
	u32 unkConst0x9C; //0x9C
	u32 unkConst0xA0; //0xA0
	u32 unkBoolean0xA4; //0xA4
	f32 ledgeJumpHorzVel; //0xA8
	f32 ledgeJumpVertVel; //0xAC
	f32 itemThrowVelMulti; //0xB0
	u32 unk0xB4;
	u32 unk0xB8;
	u32 unk0xBC;
	u32 unk0xC0;
	u32 unk0xC4;
	u32 unk0xC8;
	u32 unk0xCC;
	u32 unk0xD0;
	f32 unk0xD4; //0xD8
	u32 unk0xD8; //0xD8
	u32 kirbyStarDmg; //0xE0
	u32 normLandFrames; //0xE4
	u32 nAirLandFrames; //0xE8
	u32 fAirLandFrames; //0xEC
	u32 bAirLandFrames; //0xF0
	u32 dAirLandFrames; //0xF4
	u32 uAirLandFrames; //0xF8
	f32 victoryModelScale; //0xFC
	f32 unk0x100;
	f32 wallJumpHorzVel; //0x104
	f32 wallJumpVertVel; //0x108
	f32 unk0x10C;
	f32 unk0x110;
	f32 unk0x114;
	f32 unk0x118;
	f32 unk0x11C;
	f32 unk0x120;
	f32 unk0x124;
	f32 unk0x128;
	f32 unk0x12C;
	u32 unkConst0x130;
	f32 unk0x134;
	f32 unk0x138;
	f32 unk0x13C;
	f32 unk0x140;
	f32 unk0x144;
	f32 unk0x148;
	f32 bubbleRatio; //0x14C
	f32 unk0x150;
	f32 unk0x154;
	f32 unkConst0x158;
	u32 unkConst0x15C;
	f32 iceTraction; //0x160
	u32 unkConst0x164;
	u32 unkConst0x168;
	u32 cameraTargetBone;
	u32 unkConst0x170;
	u32 unk0x174;
	u32 unkConst0x178;
	u32 specialJumpAction; //0x17C - Pretty much an unknown, need to research what uses this
	u32 throwSpeedFlags; //0x180		
} Attributes;

typedef struct _Player {
	struct _HSD_GObj* parent;
	u32 x4_internal_id;
	u32 x8_spawn_ctr;
	u8 xC_slot;
	u8 flag1;
	u8 flag2;
	u8 flag3;
	u32 x10_action_state;
	u32 anim_state;
	u32 tblconstant;
	void* shrdaction_tbl;
	void* move_tbl;
	void* state_ptr;
	void* hitboxdata_endptr;
	f32 facedir;
	f32 facedir_model;
	f32 scale;
	f32 scale_dup;
	f32 depth_scale;
	void* unkptr40;
	void* unkptr44;
	u32 linklist_len;
	guVector vel_self;
	guVector vel_attk;
	f32 xA0_unk;
	guVector xB0_pos;
	guVector xBC_pos_prevframe;
	guVector xC8_pos_delta;
	BOOL xE0_in_air;
	f32 xE4_vel_queue_x;
	f32 xE8_unk;
	f32 xEC_vel_ground_self_x;
	f32 xF0_unk;
	f32 xF4_unk;
	f32 xF8_unk;
	f32 xFC_unk;
	f32 x100_unk;
	void* x104_unk;
	void* x108_unk;
	void* x10C_unk;
	Attributes attribs;

	SubactionInfo x3E4_subaction_info;

	u32* x5E8_bone_lookup_table;

	f32 x620_joystick_x;
	f32 x624_joystick_y;
	f32 x628_prev_joystick_x;
	f32 x62C_prev_joystick_y;

	f32 x638_cstick_x;
	f32 x63C_cstick_y;

	u32 x668_instant_buttons;

	s32 x72C_floor_id_actionstate;

	u32 x820_unk;
	s32 x83C_floor_id;
	u32 x840_ground_type;

	u32 x88C_ecb_inactive_frames;

	f32 x894_action_state_frames;

	struct _Hitbox x914_hitbox[4];
	
	u32 x18C4_last_atkeridx;

	u8 x1968_jumps_used;

	struct _Item* x1974_held_item;

	u16 x1A64_grabbable_flags;

	u32 x1A94_cpu_flags;
	u32 x1A98_cpu_level;
	
	u32 x20A4_unk;

	void (*x21BC_Projectile_Spawn)(HSD_GObj* gobj);

	u8 x21FC_collbubble_flags;
	BOOL x2200_iasa_avail;

	u8 x2210_flags;
	u8 x2218_flags;
	u8 x2219_flags;
	u8 x221F_flags;

	u8 x2224_flags;
	u32 x222C_flags;
	u32 x2230_flags;

	u32 x2340_flags;
	f32 x2344_flags;
} Player;

#endif 
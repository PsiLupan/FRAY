#include "ftMario.h"

//800E0E54
static void Mario_Special_Neutral_IASA(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    if(player->x2200_iasa_avail == TRUE){
        Interrupt_Wait(gobj);
    }
}

//800E0E84
static void Mario_Special_Neutral_Physics(HSD_GObj* gobj){
    sub_80084F3C(gobj);
}

//800E0EA4
static void Mario_Special_Neutral_CollInterrupt(HSD_GObj* gobj){
    if(Player_CopyCurrFramePhysicsToPrev(gobj) == TRUE){
        Mario_Special_Neutral_SetActionState(gobj);
    }
}

//800E0EE0
static void Mario_SpawnProjectile(HSD_GObj* gobj, guVector pos){
    Player* player = GOBJ_PLAYER(gobj);
    BOOL new_fireball = FALSE;
    if((player->x2210_flags >> 7) & 1){
        player->x2210_flags &= 0xFFFFFF7F;
        new_fireball = TRUE;
    }
    if(new_fireball){
        u32 index = Player_BoneID2Index(player, 0x17);
        HSD_JObj* spawn_bone = (HSD_JObj*)(*(player->x5E8_bone_lookup_table) + (index * BONE_LOOKUP_STRUCT_SIZE));
        HSD_JObj* jobj = JObj_Spawn(spawn_bone, NULL, pos);
        if(player->x4_internal_id == INTERNAL_MARIO){
            sub_8039B6F8(gobj, pos, ITEM_MARIO_FIREBALL, player->facedir);
            sub_8005FDDC(0x47A, gobj, spawn_bone, &player->facedir);
        }else{ //IF DR. MARIO
            u32 values[9];            
            u32 iterations = 9;
            u32 color_idx = 0;
            u32 num_values = 0;

            do {
                if(iterations != player->x222C_flags && iterations != player->x2230_flags){
                    values[color_idx] = color_idx;
                    num_values += 1;
                }
                color_idx += 1;
                iterations -= 1;
            } while (iterations != 0);
            player->x2230_flags = player->x222C_flags;
            u32 pill_color = values[HSD_Randi(num_values)];
            player->x222C_flags = pill_color;
            sub_802C0510(gobj, pos, pill_color, ITEM_DRMARIO_PILL, player->facedir);
        }
    }
}

//800E1040
static void Mario_Special_Neutral_Air_SetActionState(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2200_iasa_avail = FALSE;
    player->x2210_flags = 0;
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL, 0, NULL, 0.0f, 1.0f, 0.0f);
    sub_8006EBA4(gobj);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}

//800E1178
static void Mario_Special_Neutral_SetActionState(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    Player_LoseGroundJump_ECBDisable(player);
    Player_ChangeActionState(gobj, MARIO_ACTIONSTATE_SPECIAL_NEUTRAL, 0x5000, NULL, player->x894_action_state_frames, 1.0f, 0.0f);
    player->x21BC_Projectile_Spawn = Mario_SpawnProjectile;
}
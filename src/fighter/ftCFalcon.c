#include "ftCFalcon.h"

//800E2AAC
static void __OnLoad(Player* player){
    f32* dat_file = player->x10C_ftData->x4_charAttributes;
    f32* player_afp = player->x2D8_player_article_floats;

    memcpy(player_afp, dat_file, 36*sizeof(f32));

    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}

//800E2AEC
void CFalcon_OnLoad(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    player->x2224_flags = (player->x2224_flags & 0xFE) | 1;

    Shared_OnLoad(GOBJ_PLAYER(gobj));
}

//800E2B40
void CFalcon_UpdateAttributes(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    f32* dat_file = player->x10C_ftData->x4_charAttributes;
    f32* player_afp = player->x2D4_player_article_floats;

    memcpy(player_afp, dat_file, 36*sizeof(f32));
}


//8014EE1C
void Ganon_OnLoad(HSD_GObj* gobj)
{
    Shared_OnLoad(GOBJ_PLAYER(gobj));
}


//8014EE40
void Ganon_UpdateAttributes(HSD_GObj* gobj)
{
    CFalcon_UpdateAttributes(gobj);
}

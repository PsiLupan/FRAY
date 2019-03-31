#include "ftCFalcon.h"

//800E2AAC
//8014EE1C - Technically just a caller that passes the player object to the function. Pointless.
void Ganon_OnLoad(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    
    u32** char_dat = (u32**)player->x10C_char_dat;
    f32* dat_file = (f32*)char_dat[1];
    f32* player_afp = player->x2D8_player_article_floats;
    
    u32 i = 17;
    u32 index = 0;
    do {
        player_afp[index] = dat_file[index];
        player_afp[index + 1] = dat_file[index + 1];
        index += 2;
        --i;
    } while(i > 0);
    player_afp[index] = dat_file[index];
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}

//800E2AEC
void CFalcon_OnLoad(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2224_flags = player->x2224_flags & 0xFE | 1;
    
    u32** char_dat = (u32**)player->x10C_char_dat;
    f32* dat_file = (f32*)char_dat[1];
    f32* player_afp = player->x2D8_player_article_floats;
    
    u32 i = 17;
    u32 index = 0;
    do {
        player_afp[index] = dat_file[index];
        player_afp[index + 1] = dat_file[index + 1];
        index += 2;
        --i;
    } while(i > 0);
    player_afp[index] = dat_file[index];
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}

//800E2B40
//8014EE40 - Ganondorf's UpdateAttributes that just calls this one
void CFalcon_UpdateAttributes(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    u32** char_dat = (u32**)player->x10C_char_dat;
    f32* dat_file = (f32*)char_dat[1];
    f32* player_afp = player->x2D4_player_article_floats;

    u32 i = 17;
    u32 index = 0;
    do {
        player_afp[index] = dat_file[index];
        player_afp[index + 1] = dat_file[index + 1];
        index += 2;
        --i;
    } while(i > 0);
    player_afp[index] = dat_file[index];
}
#include "ftCFalcon.h"

//800E2AEC
void CFalcon_OnLoad(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2224_flags = player->x2224_flags & 0xFE | 1;
    
    u32** char_dat = (u32**)player->x10C_char_dat;
    f32* dat_file = (f32*)char_dat[1];
    dat_file = dat_file - 2;
    f32* player_afp = player->x2D8_player_article_floats - 2;
    
    u32 i = 17;
    do {
        f32 data1 = dat_file + 2;
        dat_file += 2; //Update offset by 8 bytes, so 4 bytes are skipped between each AFP
        f32 data2 = dat_file + 1;
        player_afp[2] = data1;
        player_afp = player_afp + 2;
        player_afp[1] = data2;
        --i;
    } while(i > 0);
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}
#include "ftFox.h"

//800E5858
//80149D3C - Falco's caller function
void Fox_UpdateAttributes(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    u32** char_dat = (u32**)player->x10C_char_dat;
    f32* dat_file = (f32*)char_dat[1];
    f32* player_afp = player->x2D4_player_article_floats;

    u32 i = 26;
    u32 index = 0;
    do {
        player_afp[index] = dat_file[index];
        player_afp[index + 1] = dat_file[index + 1];
        index += 2;
        --i;
    } while(i > 0);
    player_afp[index] = dat_file[index];
}
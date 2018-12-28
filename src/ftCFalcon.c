#include "ftCFalcon.h"

//800E2AEC
void CFalcon_OnLoad(HSD_GObj* gobj){
    Player* player = GOBJ_PLAYER(gobj);
    player->x2224_flags = player->x2224_flags & 0xFE | 1;
    u8* char_dat = player->x10C_char_dat;
    u8* player_afp = player->x2D8_player_article_floats;
    char_dat = char_dat - 4;
    u8* player_afp = player->x2D8_player_article_floats - 8;
    
    u32 i = 17;
    do {
        f32 data1 = ((f32*)char_dat)[2];
        char_dat = char_dat + 8; //Update offset by 8 bytes, so 4 bytes are skipped between each AFP
        f32 data2 =  ((f32*)char_dat)[1];
        player_afp[2] = data1;
        player_afp = player_afp + 8;
        player_afp[1] = data2;
        --i;
    } while(i > 0);
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}
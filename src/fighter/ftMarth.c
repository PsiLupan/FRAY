#include "ftMarth.h"

//801364AC
void Marth_OnLoad(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);

    f32* dat_file = player->x10C_ftData->x4_charAttributes;
    f32* player_afp = player->x2D8_player_article_floats;

    memcpy(player_afp, dat_file, 38*sizeof(f32));
    
    player->x2D4_player_article_floats = player->x2D8_player_article_floats;
}

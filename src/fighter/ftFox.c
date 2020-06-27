#include "ftFox.h"

//800E5858
void Fox_UpdateAttributes(HSD_GObj* gobj)
{
    Player* player = GOBJ_PLAYER(gobj);
    f32* dat_file = player->x10C_ftData->x4_charAttributes;
    f32* player_afp = player->x2D8_player_article_floats;

    memcpy(player_afp, dat_file, 54*sizeof(f32));
}

//80149D3C
void Falco_UpdateAttributes(HSD_GObj* gobj)
{
    Fox_UpdateAttributes(gobj);
}

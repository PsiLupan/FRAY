#include "subaction.h"

//8007162C
void Subaction_Event_30(HSD_GObj* gobj, SubactionInfo* event){
    Player* player = GOBJ_PLAYER(gobj);
    
    u16 event_id = *(u16*)event->data_position; //Example is in Ness's jab: 30800004 where the hitbox ID is 1 and the data is 4
    u32 hitbox_id = (event_id >> 7 & 7);
    Hitbox* hitbox = &player->x914_hitbox[hitbox_id];

    u32 data = *event->data_position & 0x7FFFFF;

    sub_8007ABD0(hitbox, data, gobj, player);
    event->data_position += 4;
}
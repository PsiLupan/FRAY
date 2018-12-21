#include "subaction.h"

//8007168C
//800716F8
//80071774
//800717C8
//80071810
void Subaction_MoveToNextPosition(SubactionInfo* event){
    event->data_position += 4;
}

//8007162C
void Subaction_Event_30(HSD_GObj* gobj, SubactionInfo* event){ //Scale Hitbox Damage by Model Scale (Unstaled)
    Player* player = GOBJ_PLAYER(gobj);
    
    u16 event_id = *(u16*)event->data_position; //Example is in Ness's jab: 30800004 where the hitbox ID is 1 and the data is 4
    u32 hitbox_id = (event_id >> 7 & 7);
    Hitbox* hitbox = &player->x914_hitbox[hitbox_id];

    u32 data = *event->data_position & 0x7FFFFF;

    sub_8007ABD0(hitbox, data, gobj, player);
    event->data_position += 4;
}

//80071784
void Subaction_Event_3C(HSD_GObj* gobj, SubactionInfo* event){ //DeactivateHitboxByIndex
    u32 index = *event->data_position & 0x3FFFFFF;
    Hitbox_DeactivateByIndex(gobj, index);
    event->data_position += 4;
}

//800717D8
void Subaction_Event_40(HSD_GObj* gobj, SubactionInfo* event){ //DeactivateAllHitboxes
    Hitbox_DeactivateAll(gobj);
    event->data_position += 4;
}

//800718A4
void Subaction_Event_50(HSD_GObj* gobj, SubactionInfo* event){
    Player* player = GOBJ_PLAYER(gobj);
    BOOL data = *event->data_position & 0x3FFFFFF;
    if(data == TRUE){
        player->x2210_flags |= 0x8u;
    }else if(data == FALSE){
        player->x2210_flags |= 0x10u;
    }
    event->data_position += 4;
}

//80071908
void Subaction_Event_54(HSD_GObj* gobj, SubactionInfo* event){ //???
    Player* player = GOBJ_PLAYER(gobj);
    player->x2210_flags |= 0x40u;
    event->data_position += 4;
}

//8007192C
void Subaction_Event_58(HSD_GObj* gobj, SubactionInfo* event){ //???
    Player* player = GOBJ_PLAYER(gobj);
    player->x2210_flags |= 0x20u;
    event->data_position += 4;
}

//80071950
void Subaction_Event_5C(HSD_GObj* gobj, SubactionInfo* event){ //IASAEnable
    Player* player = GOBJ_PLAYER(gobj);
    player->x2218_flags |= 0x80u;
    event->data_position += 4;
}

//80071974
void Subaction_Event_60(HSD_GObj* gobj, SubactionInfo* event){ //CanSpawnProjectile
    Player* player = GOBJ_PLAYER(gobj);
    player->x2210_flags |= 0x80u;
    event->data_position += 4;
}

//80071A14
void Subaction_Event_68(HSD_GObj* gobj, SubactionInfo* event){ //BodyStateChange
    u32 data = *event->data_position & 0x3FFFFFF;
    sub_8007B62C(gobj, data);
    event->data_position += 4;
}

//80071A58
void Subaction_Event_6C(HSD_GObj* gobj, SubactionInfo* event){ //???
    u32 data = *event->data_position & 0x3FFFFFF;
    sub_8007B0C0(gobj, data);
    event->data_position += 4;
}
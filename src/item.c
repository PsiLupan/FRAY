#include "item.h"

//800C5240
BOOL Item_PlayerHasHammer(HSD_GObj* gobj)
{
    Player* ply = GOBJ_PLAYER(gobj);
    HSD_GObj* held_item = ply->x1974_held_item;
    if (held_item != NULL) {
        if (Item_GetItemType(held_item) == ITEM_HAMMER)
            return TRUE;
    }
    return FALSE;
}

//8026A8EC
void Item_DestroyItem(HSD_GObj* gobj)
{
    Item* item = GOBJ_ITEM(gobj);
    assert(GObj_IsItem(gobj) && item != NULL);

    sub_802725D4(gobj);
    sub_802675A8(gobj);
    //TODO: Complete remainder of function
}

//8026B294
void Item_GetPosition(HSD_GObj* gobj, f32* position)
{
    Item* item = GOBJ_ITEM(gobj);
    position[0] = item->x4C_position_x;
    position[1] = item->x50_position_y;
    position[2] = item->x54_position_z;
}

//8026B300
u32 Item_GetItemType(HSD_GObj* gobj)
{
    Item* item = GOBJ_ITEM(gobj);
    return item->x10_item_type;
}

//8026B54C
//8026B560
f32 Item_GetDefaultDuration(HSD_GObj* gobj)
{
    Item* item = GOBJ_ITEM(gobj);
    return item->xC4_item_attribs[0][1];
}

//8026B73C
void Item_8026B73C(HSD_GObj* gobj)
{
    Item* item = GOBJ_ITEM(gobj);
    if ((item->xDC8_flags & 1) != 0) {
        item->xDC8_flags = (item->xDC8_flags & 0xFB) | 4;
    }
    if ((item->xDC8_flags >> 4 & 1) == 0) {
        return;
    }
    item->xDC8_flags = item->xDC8_flags & 0xEF;
    return;
}

//8026B7E8
u32 Item_8026B7E8(HSD_GObj *gobj)
{
    Item* item = GOBJ_ITEM(gobj);
    return ((item->xDC8_flags) >> 6) & 1;
}


//8026BB20
void Item_Unhide(HSD_GObj* gobj){
    JObj_Unhide((HSD_JObj*)gobj->hsd_obj);
}
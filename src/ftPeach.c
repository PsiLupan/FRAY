#include "ftPeach.h"

//802BDB94
void Peach_RemoveParasol(HSD_GObj* gobj){
    Item* item = GOBJ_ITEM(gobj);
    if(item != NULL){
        HSD_GObj* owner = item->x518_owner;
        if(owner != NULL && Player_SwapItem(owner) == TRUE){
            item->x518_owner = NULL;
        }
        Item_DestroyItem(gobj);
    }
}
#include "item.h"

//800C5240
BOOL Item_PlayerHasHammer(HSD_GObj* gobj){
	Player* ply = GOBJ_PLAYER(gobj);
	Item* held_item = ply->x1974_held_item;
	if(held_item != NULL){
		if(Item_GetItemType(held_item) == ITEM_HAMMER)
			return TRUE;
	}
	return FALSE;
}

//8026B3000
u32 Item_GetItemType(Item *item){
	return item->item_type;
}

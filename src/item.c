#include "item.h"

//8026B3000
u32 Item_GetItemType(struct Item *item){
	return item->item_type;
}

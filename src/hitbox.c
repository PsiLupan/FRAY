#include "hitbox.h"

//80008428
void Hitbox_Deactivate(u32* hitbox){
	hitbox[0] = 0;
}

//80008434
void Hitbox_Activate(u32* hitbox){
	hitbox[0] = 1;
}
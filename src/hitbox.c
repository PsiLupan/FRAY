#include "hitbox.h"

//80008428
void Hitbox_Deactivate(struct Hitbox* hitbox){
	hitbox->x4_active = false;
}

//80008434
void Hitbox_Activate(struct Hitbox* hitbox){
	hitbox->x4_active = true;
}
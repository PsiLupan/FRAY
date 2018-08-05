#include "hitbox.h"

//80008428
void Hitbox_Deactivate(struct Hitbox* hitbox){
	hitbox->active = false;
}

//80008434
void Hitbox_Activate(struct Hitbox* hitbox){
	hitbox->active = true;
}
#include <gccore.h>
#include <ogcsys.h>
#include <ogc/system.h>

//80008428
void Hitbox_Deactivate(int* hitbox){
	hitbox[0] = 0;
}

//80008434
void Hitbox_Activate(int* hitbox){
	hitbox[0] = 1;
}
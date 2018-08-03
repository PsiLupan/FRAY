#include "entity.h"

bool Entity_IsPlayer(struct Entity* ent){
	if(ent->classifier == 0x04){
		return true;
	}
	return false;
}
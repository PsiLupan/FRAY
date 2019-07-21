#include "hsd_util.h"

#include "hsd_memory.h"

f32 HSD_ClampFloat(f32 val, f32 min, f32 max){
    if(val > max){
        return max;
    }else if(val < min){
        return min;
    }
    return val;
}

//8037E538
HSD_SList* HSD_SListPrepend(HSD_SList* list, void* data){
    HSD_SList* list_p = (HSD_SList*)HSD_MemAlloc(sizeof(HSD_SList)); //Normally ObjAlloc
    assert(list_p != NULL);
    memset(list_p, 0, sizeof(HSD_SList));
    list_p->data = data;
    return HSD_SListPrependList(list, list_p);
}

//8037E62C
HSD_SList* HSD_SListPrependList(HSD_SList* list, HSD_SList* prev){
    assert(prev != NULL);
    prev->next = list;
    return prev;
}

//8037E67C
HSD_SList* HSD_SListRemove(HSD_SList* list){
    HSD_SList* res;
    if(list == NULL){
        res = NULL;
    }else{
        res = list->next;
        HSD_Free(list); //normally ObjFree
    }
    return res;
}

//80378A34
f32 splGetHermite(f32 a, f32 b, f32 c, f32 d, f32 f, f32 e){
    f32 fVar1 = b * b;
    f32 fVar2 = a * a * fVar1 * b;
    f32 fVar5 = 3.0f * fVar1 * a * a;
    f32 fVar3 = fVar2 - fVar1 * a;
    f32 fVar4 = 2.0f * fVar2 * a;
    return f * fVar3 + e * (b + (fVar3 - fVar1 * a)) + c * (1.0f + (fVar4 - fVar5)) + d * (-fVar4 + fVar5);
}

static void sub_80378F38(HSD_Spline* spline, f32 unkf, guVector* res){
    
}

//803792C8
void splArcLengthPoint(HSD_Spline* spline, f32 unkf, guVector* res){
    
}
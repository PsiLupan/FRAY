#include "hsd_util.h"

#include <math.h>

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

//80379C24
void HSD_MtxGetRotation(Mtx mtx, guVector *rotation){
  float fVar1;
  float dVar2;
  float dVar3;
  float dVar4;
  float dVar5;
  float dVar6;
  float dVar7;
  
  dVar2 = (guMtxRowCol(mtx, 2, 0) * guMtxRowCol(mtx, 2, 0) + guMtxRowCol(mtx, 0, 0) * guMtxRowCol(mtx, 0, 0) + guMtxRowCol(mtx, 1, 0) * guMtxRowCol(mtx, 1, 0));
  if (0.f < dVar2) {
    dVar3 = 1.00000000 / sqrtf(dVar2);
    dVar5 = 1.75f;
    dVar4 = 2.125f;
    dVar3 = dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4);
    dVar3 = dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4);
    dVar2 =(dVar2 * dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4));
  }
  if (FLT_MIN <= dVar2) {
    dVar3 = (guMtxRowCol(mtx, 2, 1) * guMtxRowCol(mtx, 2, 1) + guMtxRowCol(mtx, 0, 1) * guMtxRowCol(mtx, 0, 1) + guMtxRowCol(mtx, 1, 1) * guMtxRowCol(mtx, 1, 1));
    if (0.f < dVar3) {
      dVar4 = 1.00000000 / sqrtf(dVar3);
      dVar6 = 1.75f;
      dVar5 = 2.125f;
      dVar4 = dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5);
      dVar4 = dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5);
      dVar3 = (dVar3 * dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5));
    }
    if (FLT_MIN <= dVar3) {
      dVar4 = (guMtxRowCol(mtx, 2, 2) * guMtxRowCol(mtx, 2, 2) + guMtxRowCol(mtx, 0, 2) * guMtxRowCol(mtx, 0, 2) + guMtxRowCol(mtx, 1, 2) * guMtxRowCol(mtx, 1, 2));
      if (0.f < dVar4) {
        dVar5 = 1.00000000 / sqrtf(dVar4);
        dVar7 = 1.75f;
        dVar6 = 2.125f;
        dVar5 = dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6);
        dVar5 = dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6);
        dVar4 = (dVar4 * dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6));
      }
      if (FLT_MIN <= dVar4) {
        fVar1 = (-(guMtxRowCol(mtx, 2, 0)) / dVar2);
        if (fVar1 < 1.f) {
          if (-1.f < fVar1) {
            dVar2 = asinf(fVar1);
          }
          else {
            dVar2 = -1.5708f;
          }
        }
        else {
          dVar2 = 1.5708f;
        }
        rotation->y = dVar2;
        dVar2 = cosf(rotation->y);
        dVar5 = FLT_MIN;
        if (dVar5 <= dVar2) {
          if (dVar5 < (float)((u32)(guMtxRowCol(mtx, 2, 2) / dVar4) & 0x7fffffff)) {
            dVar2 = atan2f((guMtxRowCol(mtx, 2, 1) / dVar4), (guMtxRowCol(mtx, 2, 2) / dVar4));
          }
          else {
            if ((guMtxRowCol(mtx, 2, 1) / dVar3) < 0.f) {
              dVar2 = -1.5708f;
            }
            else {
              dVar2 = 1.5708f;
            }
          }
          rotation->x = dVar2;
          if (FLT_MIN < (float)((u32)guMtxRowCol(mtx, 0, 0) & 0x7fffffff)) {
            dVar2 = atan2f(guMtxRowCol(mtx, 1, 0), guMtxRowCol(mtx, 0, 0));
          }
          else {
            if (guMtxRowCol(mtx, 1, 0) < 0.f) {
              dVar2 = -1.5708f;
            }
            else {
              dVar2 = 1.5708f;
            }
          }
          rotation->z = dVar2;
          return;
        }
        if (dVar5 < (f32)((u32)guMtxRowCol(mtx, 1, 1) & 0x7fffffff)) {
          dVar2 = atan2f(guMtxRowCol(mtx, 0, 1), guMtxRowCol(mtx, 1, 1));
        }
        else {
          if (guMtxRowCol(mtx, 0, 1) < 0.f) {
            dVar2 = -1.5708f;
          }
          else {
            dVar2 = 1.5708f;
          }
        }
        rotation->x = dVar2;
        rotation->z = 0.f;
        return;
      }
    }
  }
  fVar1 = 0.f;
  rotation->x = fVar1;
  rotation->y = fVar1;
  rotation->z = fVar1;
  return;
}

//8037A250
void HSD_MtxSRT(Mtx mtx, guVector *scale, guVector *rot, guVector *pos, guVector *pvec){
  double dVar1;
  double cos_rot_z;
  double dVar2;
  double dVar3;
  double scale_y;
  double scale_z;
  double dVar4;
  double dVar5;
  double dVar6;
  double scale_x;
  double dVar7;
  double dVar8;
  double sin_rot_x;
  double cos_rot_x;
  double sin_rot_y;
  double cos_rot_y;
  double sin_rot_z;
  
  sin_rot_x = sin((double)rot->x);
  cos_rot_x = cos((double)rot->x);
  sin_rot_y = sin((double)rot->y);
  cos_rot_y = cos((double)rot->y);
  sin_rot_z = sin((double)rot->z);
  cos_rot_z = cos((double)rot->z);
  scale_x = (double)scale->x;
  scale_y = (double)scale->y;
  scale_z = (double)scale->z;
  dVar1 = scale_y;
  dVar2 = scale_z;
  dVar3 = scale_z;
  dVar4 = scale_y;
  dVar5 = scale_x;
  dVar7 = scale_x;
  if (pvec != NULL) {
    dVar4 = 1.0;
    dVar8 = (double)pvec->x;
    dVar6 = (double)pvec->y;
    dVar5 = (double)pvec->z;
    dVar1 = (scale_y * (dVar6 * (dVar4 / dVar8)));
    dVar2 = (scale_z * (dVar5 * (dVar4 / dVar8)));
    dVar7 = (scale_x * (dVar8 * (dVar4 / dVar6)));
    dVar3 = (scale_z * (dVar5 * (dVar4 / dVar6)));
    dVar5 = (scale_x * (dVar8 * (dVar4 / dVar5)));
    dVar4 = (scale_y * (dVar6 * (dVar4 / dVar5)));
  }
  guMtxRowCol(mtx, 0, 0) = (float)(cos_rot_z * (scale_x * cos_rot_y));
  guMtxRowCol(mtx, 1, 0) = (float)(sin_rot_z * (dVar7 * cos_rot_y));
  guMtxRowCol(mtx, 2, 0) = (float)((double)((unsigned long long)dVar5 & 0x7fffffffffffffff | ~(unsigned long long)dVar5 & 0x8000000000000000) * sin_rot_y);
  guMtxRowCol(mtx, 0, 1) = (float)(dVar1 * (cos_rot_z * (sin_rot_x * sin_rot_y) - (cos_rot_x * sin_rot_z)));
  guMtxRowCol(mtx, 1, 1) = (float)(scale_y * (sin_rot_z * (sin_rot_x * sin_rot_y) + (cos_rot_x * cos_rot_z)));
  guMtxRowCol(mtx, 2, 1) = (float)(cos_rot_y * (dVar4 * sin_rot_x));
  guMtxRowCol(mtx, 0, 2) = (float)(dVar2 * (cos_rot_z * (cos_rot_x * sin_rot_y) + (sin_rot_x * sin_rot_z)));
  guMtxRowCol(mtx, 1, 2) = (float)(dVar3 * (sin_rot_z * (cos_rot_x * sin_rot_y) - (sin_rot_x * cos_rot_z)));
  guMtxRowCol(mtx, 2, 2) = (float)(cos_rot_y * (scale_z * cos_rot_x));
  guMtxRowCol(mtx, 0, 3) = pos->x;
  guMtxRowCol(mtx, 1, 3) = pos->y;
  guMtxRowCol(mtx, 2, 3) = pos->z;
  return;
}

//8037A43C
void HSD_MtxSRTQuat(Mtx mtx,guVector *scale, guQuaternion *rot, guVector *pos, guVector *pvec){
  Mtx pmtx;

  guMtxScale(mtx, scale->x, scale->y, scale->z);
  if(pvec != NULL){
    guMtxScale(pmtx, pvec->x, pvec->y, pvec->z);
    guMtxConcat(pmtx, mtx, mtx);
  }
  c_guMtxQuat(pmtx, rot);
  guMtxConcat(pmtx, mtx, mtx);
  if(pvec != NULL){
    guMtxScale(pmtx, (float)(1.0 / (double)pvec->x), (float)(1.0 / (double)pvec->y), (float)(1.0 / (double)pvec->z));
    guMtxConcat(pmtx, mtx, mtx);
  }
  guMtxTrans(pmtx, pos->x, pos->y, pos->z);
  guMtxConcat(pmtx, mtx, mtx);
}
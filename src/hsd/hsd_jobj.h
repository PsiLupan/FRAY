#ifndef _hsd_jobj_h_
#define _hsd_jobj_h_

#include "../../include/mtx/GeoTypes.h"

#include "hsd_dobj.h"

//Joint Object
typedef struct _HSD_JObj {
	struct _HSD_Info *info;
	HSD_Class parent;
	HSD_JObjDesc* next;
	HSD_JObjDesc* parent;
	HSD_JObjDesc* child;
	u32 flags;
	struct _HSD_DObjDesc* dobj;
	Quaternion rotation;
	Vec scale;
	Vec position;
	Mtx mtx;
	VecPtr pvec;
	MtxPtr pmtx;
	struct _HSD_AObj* aobj;
	struct _HSD_RObj* robj;
	HSD_JObjDesc* desc;
} HSD_JObj;

typedef struct _HSD_JObjDesc {
	char* class_name;
	u32 flags;
	HSD_JObj child;
	HSD_JObj next;
	struct _HSD_DObj* dobj;
	Vec rotation;
	Vec scale;
	Vec position;
	Mtx mtx;
	struct _HSD_RObjDesc* robj;
} HSD_JObjDesc;

typedef struct _HSD_JObjInfo {
	
} HSD_JObjInfo;

extern HSD_JObjInfo hsdJObj;

#endif
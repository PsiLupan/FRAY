#ifndef _ground_h_
#define _ground_h_

#include <gctypes.h>

typedef struct _Stage_Table {
    u32 id;
    struct _Stage_Subtable* func;
    char* filename;
    void (*StageInit)();
	void (*unk_func_1)();
	void (*OnLoad)();
	void (*OnGO)();
	void (*unk_func_3)();
	void (*unk_func_4)();
	void (*unk_func_5)();
	u32 unk_0;
	void *unk_1;
	void *unk_2;

} Stage_Table;

typedef struct _Stage_Subtable {
    void (*func1)();
    void (*func2)();
    void (*func3)();
    void (*func4)();
    u32 pad;
} _Stage_Subtable;

#endif
#ifndef _scGmRst_h_
#define _scGmRst_h_

#include <gctypes.h>

#include "hsd/hsd_archive.h"

#include "scene.h"

void Menu_CSS_Init();
void Menu_CSS_Load();
void Menu_CSS_VSMode_Prep(GameState*);
void Menu_CSS_VSMode_Decide(GameState*);
void Menu_SSS_VSMode_Prep(GameState*);
void Menu_SSS_VSMode_Decide(GameState*);

void Menu_CSS_OnLoad(void* unk_struct);
void Menu_CSS_OnFrame();
void Menu_CSS_OnLeave();

#endif
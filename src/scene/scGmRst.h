#ifndef _scGmRst_h_
#define _scGmRst_h_

#include <gctypes.h>

#include "scene.h"

void Menu_CSS_Init();
void Menu_CSS_Load();
void Menu_CSS_VSMode_Prep(GameState*);
void Menu_CSS_VSMode_Decide(GameState*);
void Menu_SSS_VSMode_Prep(GameState*);
void Menu_SSS_VSMode_Decide(GameState*);

#endif
#ifndef INIT_H
#define INIT_H

#include "defs.h"
#include "sound.h"
#include "action.h"
#include "draw.h"
#include "input.h"

// 초기화 및 정리 함수
void init_sdl(void);
void InitIngredient(void);
void InitTrail(void);
void cleanup_sdl(void);

#endif // INIT_H
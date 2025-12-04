#ifndef INIT_H
#define INIT_H

#include "defs.h"
#include "sound.h"
// 전역 변수 (외부 참조)
extern App app;
extern int g_game_running;

// 재료 프로토타입 데이터 (외부 참조)
extern Ingredient mushroom;
extern Ingredient cabbage;
extern Ingredient meat;
extern Ingredient beanSprouts;
extern Ingredient shoes;
extern Ingredient stone;

// 초기화 및 정리 함수
void init_sdl(void);
void InitIngredient(void);
void InitTrail(void);
void cleanup_sdl(void);

// 게임 루프 연결 함수
void update_game(void);
void render_game(void);

#endif // INIT_H

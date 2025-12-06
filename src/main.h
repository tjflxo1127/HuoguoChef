#ifndef MAIN_H
#define MAIN_H

#include "defs.h"
#include "init.h"
#include "input.h"
#include "action.h"
#include "draw.h"

// 전역 변수 정의 (defs.h의 App 구조체 사용)
App app;                        // 앱 객체
int g_game_running = 1;         // 게임 루프 플래그

// 재료 원본 데이터 생성
Ingredient mushroom, cabbage, meat, beanSprouts, shoes, stone;
Ingredient heart_red, heart_black;

// 텍스트 객체 생성
TextObject score_text, life_text, gameover_text, restart_text;

#endif



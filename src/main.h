/**
 * @file    main.h
 * @brief   각 모듈 헤더 파일 include 및 전역 변수 선언
 * @author  조정배 팀
 */
#ifndef MAIN_H
#define MAIN_H

#include "defs.h"
#include "init.h"
#include "input.h"
#include "action.h"
#include "draw.h"

// 색상 정의
SDL_Color white = {255, 255, 255, 255};
SDL_Color red = {255, 50, 50, 255};
SDL_Color yellow = {255, 255, 0, 255};
SDL_Color black = {0, 0, 0, 255};

// 전역 변수 정의 (defs.h의 App 구조체 사용)
App app;                        // 앱 객체
int g_game_running = 1;         // 게임 루프 플래그

// 재료 원본 데이터 생성
Ingredient mushroom, cabbage, meat, beanSprouts, shoes, stone;
Ingredient heart_red, heart_black;

// 텍스트 객체 생성
TextObject score_text, gameover_text, restart_text, title_text, subtitle_text;
TextObject rule1_text, rule2_text, rule3_text, gameover_score_text;

#endif



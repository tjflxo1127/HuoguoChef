/**
 * @file    defs.h
 * @brief   게임 구현을 위한 매크로 상수, 구조체 정의 및 전역 변수 선언
 * @author  조정배 팀
 */
#ifndef DEFS_H
#define DEFS_H

// 표준 라이브러리
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// SDL2 라이브러리
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

//action 관련 상수

#define SPAWN_INTERVAL_MAX 40        // 초기 스폰 간격 (40프레임 = 약 0.67초)
#define SPAWN_INTERVAL_MIN 5       // 최소 스폰 간격 (10프레임 = 약 0.17초)
#define DIFFICULTY_SCORE_RATE 10  // 50점마다 스폰 간격이 1씩 감소

#define LAUNCH_SPEED_MIN 20.0f
#define LAUNCH_SPEED_MAX 27.0f

// 상수 정의
#define FPS 60                //게임 FPS
#define BUFSIZE 1024          //문자열 버퍼크기

#define SCREEN_WIDTH 800      //화면 너비
#define SCREEN_HEIGHT 600     //화면 높이

#define INGREDIENTS_SIZE 100  //재료 크기
#define HEART_SIZE 30         //하트(생명)크기

#define MAX_INGREDIENTS 50 // 화면에 나타날 수 있는 재료의 최대 개수

// 냄비(육수) 범위 및 스택 관련 상수
#define POT_X 70
#define POT_Y 250
#define POT_W 700
#define POT_H 300
#define MAX_STACK_INGREDIENTS 160 // 배경에 쌓이는 최대 재료 개수

#define GRAVITY 0.5f    //중력 상수
#define PI 3.14         //파이 상수

#define FONTSIZE 20     //폰트사이즈 상

#define TRAIL_LENGTH 10    // 마우스 궤적 길이

// UI 위치 상수
#define UI_SCORE_X 20           // 점수판 X 위치
#define UI_SCORE_Y 20           // 점수판 Y 위치
#define UI_HEARTS_X 20          // 하트 시작 X 위치
#define UI_HEARTS_Y 60          // 하트 시작 Y 위치
#define UI_HEART_GAP 35         // 하트 간격
#define UI_GAMEOVER_Y_OFFSET 30 // 게임 오버 텍스트 Y 오프셋
#define UI_ALPHA_OVERLAY 200    // 게임 오버 오버레이 투명도 (0-255)


// 재료 타입 열거형
typedef enum {MUSHROOM, CABBAGE, MEAT, BEANSPROUTS, SHOES, STONE} TypeIngredient;

// 텍스트 객체 구조체
typedef struct {
    SDL_Texture *texture; // 글자 이미지 데이터
    SDL_Rect rect;        // 위치(x, y)와 크기(w, h) 정보 
} TextObject;

// 재료 구조체
typedef struct {
    float x, y;         // 위치 (물리 연산을 위해 float 사용)
    float dx, dy;       // 속도 (dx: x축 변화량, dy: y축 변화량)
    int type;           // TypeIngredient 값 (0~3:식재료, 4:신발, 5:돌)
    int is_enemy;       // 1: 닿으면 목숨 깎임(함정), 0: 점수 획득
    int is_active;      // 1: 현재 화면에 존재함, 0: 비활성
    int is_sliced;      // 1: 베어짐, 0: 안 베어짐

    float target_y;     // 냄비 범위 내 정지하는 랜덤 깊이 (y좌표)

    int w, h;           // 크기
    SDL_Texture *texture; // 이미지 텍스처

    SDL_Texture *sliced_tex1;   // 왼쪽 조각 (s_*.1.png)
    SDL_Texture *sliced_tex2;   // 오른쪽 조각 (s_*.2.png)

} Ingredient;

// 배경에 쌓인 재료 구조체
typedef struct {
    int x, y;
    int w, h;
    SDL_Texture *texture; // 잘린 단면 텍스처
    int is_valid;         // 1: 그려야 함, 0: 비어있음
} StackIngredient;

// 게임 상태 구조체
typedef struct {
    int score;          // 현재 점수
    int lives;          // 남은 목숨

    int game_over;      // 게임 종료 여부
    int title_screen;   // 타이틀 화면 표시 여부

    // 게임 내 모든 재료 관리 배열
    Ingredient ingredients[MAX_INGREDIENTS];

    // 배경에 쌓인 재료 관리 (원형 버퍼 방식)
    StackIngredient stack[MAX_STACK_INGREDIENTS];
    int stack_head; // 다음에 저장할 인덱스 (오래된 것부터 덮어씀)
} Game;

// 애플리케이션 구조체
typedef struct {
    SDL_Window *g_window;
    SDL_Renderer *g_renderer;
    TTF_Font *font;
    TTF_Font *title_font; // 타이틀용 큰 폰트

    Game game; // 게임 상태 포함

    // 마우스 슬라이스 궤적 저장용
    SDL_Point trail_points[TRAIL_LENGTH];
    int trail_head; // 순환 버퍼 인덱스

    // 마우스 상태 추가
    int mouse_x;
    int mouse_y;
    int mouse_down;

    SDL_Texture *background_texture;
} App;

// 색상 정의
extern SDL_Color white;
extern SDL_Color red;
extern SDL_Color black;
extern SDL_Color yellow;

// 전역 변수 선언 (extern)
extern App app;
extern int g_game_running;

// 재료 원본 데이터
extern Ingredient mushroom, cabbage, meat, beanSprouts, shoes, stone;
extern Ingredient heart_red, heart_black;

// UI 텍스트 객체
extern TextObject score_text, gameover_text, restart_text, title_text, subtitle_text;
extern TextObject rule1_text, rule2_text, rule3_text, gameover_score_text;

#endif // DEFS_H

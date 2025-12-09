/**
 * @file    draw.h
 * @brief   텍스처 렌더링을 위한 함수 선언 
 * @author  조정배 팀
 */
#ifndef DRAW_H
#define DRAW_H 

#include "defs.h"

// 기본 화면 제어
void ClearWindow(App *app);
void ShowWindow(App *app);

// 텍스처 헬퍼 함수
void TextureSmallText(App *app, TextObject *text_obj, char *str, SDL_Color color);
void TextureBigText(App *app, TextObject *text_obj, char *str, SDL_Color color);

// 개별 객체 그리기 
void RenderEntity(App *app, Ingredient *ing);
void RenderStack(App *app);
void RenderScoreBoard(App *app, TextObject *text_obj);

// 전체 화면 구성(게임 전체 그리기)
void DrawGame(App *app, TextObject *score, TextObject *go, TextObject *restart);

// 메인 렌더링 루프
void render_game(void);

#endif

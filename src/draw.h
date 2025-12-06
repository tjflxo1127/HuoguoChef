#ifndef DRAW_H
#define DRAW_H

#include "defs.h"

void ClearWindow(App *app);
void ShowWindow(App *app);

// [수정] 인자에 Game Over 텍스트들 추가
void DrawGame(App *app, TextObject *score, TextObject *go, TextObject *restart);

void RenderEntity(App *app, Ingredient *ing);
void RenderScoreBoard(App *app, TextObject *text_obj);
void UpdateScoreBoard(App *app, TextObject *text_obj, char *str, SDL_Color color);

#endif
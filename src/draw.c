/*
 * draw.c
 * 게임의 모든 시각적 요소(재료, UI, 이펙트)를 렌더링하는 함수 구현
 */

#include "draw.h"

// 1. 화면 지우기
void ClearWindow(App *app) {
    // 배경색: 약간 어두운 주방 느낌 (R:30, G:30, B:30)
    SDL_SetRenderDrawColor(app->g_renderer, 30, 30, 30, 255);
    SDL_RenderClear(app->g_renderer);

    if (app->background_texture != NULL) {
        SDL_RenderCopy(app->g_renderer, app->background_texture, NULL, NULL);
    }
}

// 2. 화면 보여주기
void ShowWindow(App *app) {
    SDL_RenderPresent(app->g_renderer);
}

// [추가] 스택에 쌓인 재료 그리기
void RenderStack(App *app) {
    for (int i = 0; i < MAX_STACK_INGREDIENTS; i++) {
        StackIngredient *s_ing = &app->game.stack[i];
        
        if (s_ing->is_valid && s_ing->texture != NULL) {
            SDL_Rect dest = { s_ing->x, s_ing->y, s_ing->w, s_ing->h };
            
            // 약간 어둡게 처리하여 물에 잠긴 느낌 (선택 사항)
            SDL_SetTextureColorMod(s_ing->texture, 200, 200, 200);
            
            SDL_RenderCopy(app->g_renderer, s_ing->texture, NULL, &dest);

            // 렌더링 후 색상 복구 (필요시)
            SDL_SetTextureColorMod(s_ing->texture, 255, 255, 255);
        }
    }
}

// 3. 게임 전체 그리기 (메인 함수)
// [수정] 인자 리스트에서 life 등 불필요한 것 제거
void DrawGame(App *app, TextObject *score, TextObject *go, TextObject *restart) {
    
    // [추가] 냄비에 쌓인 재료 그리기 (배경 위, 활성 재료 아래)
    RenderStack(app);

    // A. 모든 재료 그리기
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        Ingredient *ing = &app->game.ingredients[i];
        
        if (ing->is_active) {
            RenderEntity(app, ing);
        }
    }

    // B. 마우스 궤적(칼날) 그리기 (마우스 눌렀을 때만 그리기로 수정)
    if (app->mouse_down) {
        SDL_SetRenderDrawColor(app->g_renderer, 0, 0, 0, 255); // 검은색
        
        /* 수정 전: 마우스 궤적이 다각형을 뜸
        for (int i = 0; i < TRAIL_LENGTH - 1; i++) {
            int idx1 = (app->trail_head + i) % TRAIL_LENGTH;
            int idx2 = (app->trail_head + i + 1) % TRAIL_LENGTH;

            // 좌표가 유효한 경우에만 선 그리기
            if (app->trail_points[idx1].x != -1 && app->trail_points[idx2].x != -1) {
                SDL_RenderDrawLine(app->g_renderer, 
                    app->trail_points[idx1].x, app->trail_points[idx1].y,
                    app->trail_points[idx2].x, app->trail_points[idx2].y);
            }
        }*/

        // 수정 후 : 
        for (int i = 0; i < TRAIL_LENGTH - 1; i++) {
            // 현재 인덱스 (가장 최신부터 시작)
            int idx1 = (app->trail_head - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            // 다음 인덱스 (그 다음 과거)
            int idx2 = (app->trail_head - (i + 1) + TRAIL_LENGTH) % TRAIL_LENGTH;

            // 두 점이 모두 유효한 좌표일 때만 선 그리기
            if (app->trail_points[idx1].x != -1 && app->trail_points[idx2].x != -1) {
                
                // [추가] 점 사이의 거리가 너무 멀면 그리지 않음 (화면 건너뛰기 방지)
                int dist_x = abs(app->trail_points[idx1].x - app->trail_points[idx2].x);
                int dist_y = abs(app->trail_points[idx1].y - app->trail_points[idx2].y);
                
                if (dist_x < 100 && dist_y < 100) { 
                    SDL_RenderDrawLine(app->g_renderer, 
                        app->trail_points[idx1].x, app->trail_points[idx1].y,
                        app->trail_points[idx2].x, app->trail_points[idx2].y);
                }
            }
        }
    }

    // C. 점수판 (Score) 그리기
    RenderScoreBoard(app, score);

    // [수정] 목숨 하트 그리기 위치 조정
    // 점수판(score) 바로 밑에 그리기 위해 start_y 값을 수정합니다.
    // score 텍스트가 대략 y=20 위치에 그려진다면, 하트는 y=60 정도로 내리면 적당합니다.
    int start_x = 20;  // 점수판과 같은 왼쪽 정렬 (x=20)
    int start_y = 60;  // 점수판 아래로 위치 이동 (y=20 + 높이 + 여백)
    int gap = 35;      // 하트 간격

    for (int i = 1; i <= 3; i++) {
        SDL_Rect dest = {start_x + (i-1)*gap, start_y, HEART_SIZE, HEART_SIZE};
        
        // 현재 남은 목숨 수에 따라 빨간 하트 또는 검은 하트 그리기
        if (app->game.lives >= i) {
            if (heart_red.texture) SDL_RenderCopy(app->g_renderer, heart_red.texture, NULL, &dest);
        } else {
            if (heart_black.texture) SDL_RenderCopy(app->g_renderer, heart_black.texture, NULL, &dest);
        }
    }

    // D. 게임 오버 화면 처리
    if (app->game.game_over) {
        // 반투명 검은 막 씌우기
        SDL_SetRenderDrawBlendMode(app->g_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app->g_renderer, 0, 0, 0, 200); // Alpha 200
        SDL_Rect screen_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(app->g_renderer, &screen_rect);
        SDL_SetRenderDrawBlendMode(app->g_renderer, SDL_BLENDMODE_NONE); // 복구

        // 게임 오버 및 재시작 텍스트 그리기
        RenderScoreBoard(app, go);
        RenderScoreBoard(app, restart);
    }
}

// 4. 개별 재료 그리기
void RenderEntity(App *app, Ingredient *ing) {
    if (ing->texture == NULL) return;

    SDL_Rect dest;
    dest.x = (int)ing->x;
    dest.y = (int)ing->y;
    dest.w = ing->w;
    dest.h = ing->h;

    // 잘린 재료 시각 효과
    if (ing->is_sliced) {
        // 1. 색상을 어둡게 (R, G, B 값을 줄임)
        SDL_SetTextureColorMod(ing->texture, 150, 150, 150);
        // 2. 약간 반투명하게
        SDL_SetTextureAlphaMod(ing->texture, 200);
    } else {
        // 원래 색상과 불투명도 복구
        SDL_SetTextureColorMod(ing->texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(ing->texture, 255);
    }


    // 텍스처 그리기
    SDL_RenderCopy(app->g_renderer, ing->texture, NULL, &dest);
}

// 5. 텍스트 객체 그리기
void RenderScoreBoard(App *app, TextObject *text_obj) {
    if (text_obj->texture != NULL) {
        SDL_RenderCopy(app->g_renderer, text_obj->texture, NULL, &text_obj->rect);
    }
}

// 6. 텍스트 내용 업데이트 (Surface -> Texture 변환)
void UpdateScoreBoard(App *app, TextObject *text_obj, char *str, SDL_Color color) {
    // 폰트 로드 실패 시 중단
    if (app->font == NULL) return;

    // 기존 텍스처 삭제 (메모리 누수 방지)
    if (text_obj->texture != NULL) {
        SDL_DestroyTexture(text_obj->texture);
        text_obj->texture = NULL;
    }

    // Surface 생성
    SDL_Surface *surface = TTF_RenderText_Solid(app->font, str, color);
    if (surface == NULL) return;

    // Texture 생성
    text_obj->texture = SDL_CreateTextureFromSurface(app->g_renderer, surface);
    
    if (text_obj->texture != NULL) {
        text_obj->rect.w = surface->w;
        text_obj->rect.h = surface->h;
    }

    SDL_FreeSurface(surface);
}
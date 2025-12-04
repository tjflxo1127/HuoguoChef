/*
 * draw.c
 * 게임의 모든 시각적 요소(재료, UI, 이펙트)를 렌더링하는 함수 구현
 */

#include "draw.h"

// [중요] 다른 파일(init.c)에 정의된 전역 변수 참조 선언
extern Ingredient heart_red;
extern Ingredient heart_black;

// 색상 상수 정의
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_RED = {255, 50, 50, 255};
const SDL_Color COLOR_YELLOW = {255, 255, 0, 255};

// 1. 화면 지우기
void ClearWindow(App *app) {
    // 배경색: 약간 어두운 주방 느낌 (R:30, G:30, B:30)
    SDL_SetRenderDrawColor(app->g_renderer, 30, 30, 30, 255);
    SDL_RenderClear(app->g_renderer);
}

// 2. 화면 보여주기
void ShowWindow(App *app) {
    SDL_RenderPresent(app->g_renderer);
}

// 3. 게임 전체 그리기 (메인 함수)
// [수정] 인자 리스트에서 life 등 불필요한 것 제거
void DrawGame(App *app, TextObject *score, 
              TextObject *go, TextObject *restart) {
    
    // A. 모든 재료 그리기
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        Ingredient *ing = &app->game.ingredients[i];
        
        if (ing->is_active) {
            RenderEntity(app, ing);
        }
    }

    // B. 마우스 궤적(칼날) 그리기
    SDL_SetRenderDrawColor(app->g_renderer, 255, 255, 255, 255); // 흰색
    for (int i = 0; i < TRAIL_LENGTH - 1; i++) {
        int idx1 = (app->trail_head + i) % TRAIL_LENGTH;
        int idx2 = (app->trail_head + i + 1) % TRAIL_LENGTH;

        if (app->trail_points[idx1].x != -1 && app->trail_points[idx2].x != -1) {
            SDL_RenderDrawLine(app->g_renderer, 
                app->trail_points[idx1].x, app->trail_points[idx1].y,
                app->trail_points[idx2].x, app->trail_points[idx2].y);
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

    // 함정(신발, 돌)인 경우 빨간 테두리로 경고 표시 (선택 사항)
    if (ing->is_enemy && !ing->is_sliced) {
        SDL_SetRenderDrawColor(app->g_renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(app->g_renderer, &dest);
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
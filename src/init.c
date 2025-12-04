#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "action.h"
#include "draw.h"
#include "input.h"
#include "sound.h"
// #include "score.h" // [추가]


Ingredient mushroom, cabbage, meat, beanSprouts, shoes, stone;
Ingredient heart_red, heart_black;

TextObject score_text, life_text;
// [추가] 게임 오버용 텍스트 객체
TextObject gameover_text, restart_text;

/*TextObject score_text, life_text;
TextObject high_score_text; // [추가]
TextObject gameover_text, restart_text;
TextObject leaderboard_texts[MAX_LEADERBOARD_ENTRIES]; // [추가] 리더보드 줄마다 하나씩 */


void InitTrail(void) {
    app.trail_head = 0;
    for (int i = 0; i < TRAIL_LENGTH; i++) {
        app.trail_points[i].x = -1;
        app.trail_points[i].y = -1;
    }
}

void InitIngredient(void) {
    #define LOAD_TEX(obj, path) \
        obj.texture = IMG_LoadTexture(app.g_renderer, path); \
        if (!obj.texture) printf("Failed to load %s: %s\n", path, IMG_GetError());

    LOAD_TEX(cabbage, "./gfx/cabbage.png");
    LOAD_TEX(meat, "./gfx/meat.png");
    LOAD_TEX(mushroom, "./gfx/mushroom.png");
    LOAD_TEX(beanSprouts, "./gfx/bean_sprouts.png");
    LOAD_TEX(shoes, "./gfx/shoes.png");
    
    // [수정 4번 관련] 돌 이미지 로드
    LOAD_TEX(stone, "./gfx/stone.png"); 

    // [수정 1번 해결] 하트 이미지 로드 추가
    LOAD_TEX(heart_red, "./gfx/heart_red.png");
    LOAD_TEX(heart_black, "./gfx/heart_black.png");
}

void init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);
    if (TTF_Init() < 0) exit(1);
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) exit(1);

    app.font = TTF_OpenFont(FONT_PATH, FONTSIZE);
    
    if (!app.font) {
        printf("[1;31m[ERROR] Font Load Fail: %s\n", TTF_GetError());
        printf("Current Path Check: please move '%s' to executable folder.\n", FONT_PATH);
        // 폰트 없으면 게임 진행이 어려우므로 종료
    }

    app.g_window = SDL_CreateWindow("Huoguo Chef", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    app.g_renderer = SDL_CreateRenderer(app.g_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(app.g_renderer, SDL_BLENDMODE_BLEND);
    
    InitIngredient();
    InitTrail();
    // === [사운드 시스템/효과음 초기화 추가] ===
    InitSound();       // 사운드 시스템 초기화
    LoadEffects();     // 효과음(.wav) 파일 로드

    // 배경 설정
    app.background_texture = IMG_LoadTexture(app.g_renderer, "./gfx/background.png");
    if(!app.background_texture) printf("배경 로드 실패: %s\n", IMG_GetError());
    
    score_text.texture = NULL;
    life_text.texture = NULL;
    gameover_text.texture = NULL;
    restart_text.texture = NULL;

    // [추가] 고정 텍스트 미리 생성 (게임 오버, 재시작)
    SDL_Color red = {255, 50, 50, 255};
    SDL_Color white = {255, 255, 255, 255};
    UpdateScoreBoard(&app, &gameover_text, "GAME OVER", red);
    UpdateScoreBoard(&app, &restart_text, "Press 'R' to Restart", white);

    // 위치 설정 (화면 중앙)
    gameover_text.rect.x = (SCREEN_WIDTH - gameover_text.rect.w) / 2;
    gameover_text.rect.y = SCREEN_HEIGHT / 2 - 50;
    restart_text.rect.x = (SCREEN_WIDTH - restart_text.rect.w) / 2;
    restart_text.rect.y = SCREEN_HEIGHT / 2 + 10;
}

void update_game(void) {
    ActGame();
}

void render_game(void) {
    ClearWindow(&app);

    // 점수 및 목숨 업데이트 (매 프레임)
    char score_str[64];
    char life_str[64];
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 50, 50, 255};

    sprintf(score_str, "Score: %d", app.game.score);
    sprintf(life_str, "Lives: %d", app.game.lives);

    UpdateScoreBoard(&app, &score_text, score_str, white);
    UpdateScoreBoard(&app, &life_text, life_str, red);

    score_text.rect.x = 20; score_text.rect.y = 20;
    life_text.rect.x = 20; life_text.rect.y = 50;

    // [수정] DrawGame에 모든 텍스트 객체 전달
    DrawGame(&app, &score_text, &gameover_text, &restart_text);

    ShowWindow(&app);
}

void cleanup_sdl(void) {
    if (cabbage.texture) SDL_DestroyTexture(cabbage.texture);
    if (meat.texture) SDL_DestroyTexture(meat.texture);
    if (mushroom.texture) SDL_DestroyTexture(mushroom.texture);
    if (beanSprouts.texture) SDL_DestroyTexture(beanSprouts.texture);
    if (shoes.texture) SDL_DestroyTexture(shoes.texture);
    if (stone.texture) SDL_DestroyTexture(stone.texture);
    if (app.background_texture) SDL_DestroyTexture(app.background_texture);
    
    if (score_text.texture) SDL_DestroyTexture(score_text.texture);
    if (life_text.texture) SDL_DestroyTexture(life_text.texture);
    if (gameover_text.texture) SDL_DestroyTexture(gameover_text.texture);
    if (restart_text.texture) SDL_DestroyTexture(restart_text.texture);

    if (app.font) TTF_CloseFont(app.font);
    if (app.g_renderer) SDL_DestroyRenderer(app.g_renderer);
    if (app.g_window) SDL_DestroyWindow(app.g_window);

    CleanupSound(); //사운드 시스템 정리 추가

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

}

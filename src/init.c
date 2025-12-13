    /**
 * @file    init.c
 * @brief   게임 객체 및 SDL 요소 초기화 함수 정의
 * @author  조정배 팀
 */
#include "init.h"

// SDL 및 모든 하위 시스템 초기화 
void init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[ERROR] in InitSDL(): %s\n", SDL_GetError());
        exit(1);
    }

    app.g_window = SDL_CreateWindow("Huoguo Chef", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    app.g_renderer = SDL_CreateRenderer(app.g_window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(app.g_renderer, SDL_BLENDMODE_BLEND); // blend mode 투명도 처리 활성화(게임 오버 시 화면을 반투명하게 나타내기 위해 필요)
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);


    // 폰트 초기화
    if (TTF_Init() < 0){
        fprintf(stderr, "[ERROR] in InitTTF(): %s\n", TTF_GetError());
        exit(1);
    }
    app.font = TTF_OpenFont("./ttf/NotoSansKR-Regular.ttf", FONTSIZE);
    if (!app.font) {
        fprintf(stderr, "[WARNING] 폰트 로드 실패: %s\n", TTF_GetError());
        fprintf(stderr, "폰트 없이 게임을 계속 진행합니다.\n");
    }
    app.title_font = TTF_OpenFont("./ttf/NotoSansKR-Regular.ttf", FONTSIZE * 2); // 타이틀용 큰 폰트 로드 (기본 폰트의 2배 크기)
    if (!app.title_font) {
        fprintf(stderr, "[WARNING] 타이틀 폰트 로드 실패: %s\n", TTF_GetError());
        fprintf(stderr, "기본 폰트를 사용합니다.\n");
        // 기본 폰트가 있으면 그것을 사용, 없으면 title_font도 NULL
        if (app.font) {
            app.title_font = app.font;  // 같은 폰트 재사용 (크기만 다름)
        }
    }
    
    // 배경 초기화
    app.background_texture = IMG_LoadTexture(app.g_renderer, "./gfx/background.png");
    if(!app.background_texture){
        fprintf(stderr, "[WARNING] 배경 이미지 로드 실패: %s\n", IMG_GetError());
        fprintf(stderr, "검은 배경으로 게임을 계속 진행합니다.\n");
        app.background_texture = NULL;
    }
    
    // UI 및 텍스트 초기화
    InitUI();

    // 게임 객체 (재료, 궤적)
    InitIngredient();
    InitTrail();

    // 사운드
    InitSound();       // 사운드 시스템 초기화
    LoadEffects();     // 효과음(.wav) 파일 로드
    Mix_VolumeMusic(64);// BGM 볼륨 낮추기 (절반으로 낮추었다)

}

// UI 및 텍스트 초기화 함수
void InitUI(void){
    score_text.texture = NULL;
    gameover_text.texture = NULL;
    restart_text.texture = NULL;
    title_text.texture = NULL;
    subtitle_text.texture = NULL;
    rule1_text.texture = NULL;
    rule2_text.texture = NULL;
    rule3_text.texture = NULL;


    TextureBigText(&app, &title_text, "Huoguo Chef", red);
    
    TextureSmallText(&app, &subtitle_text, "Press R to Start", yellow);
    TextureSmallText(&app, &rule1_text, "Meat: 30 points, Mushroom/Cabbage/BeanSprouts: 10 points", black);
    TextureSmallText(&app, &rule2_text, "Shoes/Stone: Game Over!", black);
    TextureSmallText(&app, &rule3_text, "Miss Ingredient: Life -1 (Total 3)", black);

    TextureSmallText(&app, &gameover_text, "GAME OVER", red);
    TextureSmallText(&app, &restart_text, "Press 'R' to Restart", white);


    // 위치 설정 (화면 중앙)
    gameover_text.rect.x = (SCREEN_WIDTH - gameover_text.rect.w) / 2;
    gameover_text.rect.y = SCREEN_HEIGHT / 2 - 70;
    restart_text.rect.x = (SCREEN_WIDTH - restart_text.rect.w) / 2;
    restart_text.rect.y = SCREEN_HEIGHT / 2 + 10;

     // 게임 오버 점수 텍스트 초기 위치 설정 (실제 위치는 게임 중에 업데이트)
    gameover_score_text.rect.x = (SCREEN_WIDTH - 200) / 2; // 대략적인 중앙
    gameover_score_text.rect.y = SCREEN_HEIGHT / 2 - 30;

    // 타이틀 텍스트 위치 설정
    title_text.rect.x = (SCREEN_WIDTH - title_text.rect.w) / 2;
    title_text.rect.y = SCREEN_HEIGHT / 2 - 120;
    subtitle_text.rect.x = (SCREEN_WIDTH - subtitle_text.rect.w) / 2;
    subtitle_text.rect.y = SCREEN_HEIGHT / 2 - 70;

    // 규칙 텍스트 위치 설정
    rule1_text.rect.x = (SCREEN_WIDTH - rule1_text.rect.w) / 2;
    rule1_text.rect.y = SCREEN_HEIGHT / 2 - 40;
    rule2_text.rect.x = (SCREEN_WIDTH - rule2_text.rect.w) / 2;
    rule2_text.rect.y = SCREEN_HEIGHT / 2 - 10;
    rule3_text.rect.x = (SCREEN_WIDTH - rule3_text.rect.w) / 2;
    rule3_text.rect.y = SCREEN_HEIGHT / 2 + 20;
}

// 마우스 궤적 초기화 함수
void InitTrail(void) {
    app.trail_head = 0;
    for (int i = 0; i < TRAIL_LENGTH; i++) {
        app.trail_points[i].x = -1;
        app.trail_points[i].y = -1;
    }
}

// 재료 이미지 초기화 함수 
void InitIngredient(void) {
    #define LOAD_TEX(obj, path) \
        obj.texture = IMG_LoadTexture(app.g_renderer, path); \
        if (!obj.texture) fprintf(stderr, "[WARNING] 이미지 로드 실패 %s: %s\n", path, IMG_GetError());

    #define LOAD_SPLIT_TEX(obj, path1, path2) \
        obj.sliced_tex1 = IMG_LoadTexture(app.g_renderer, path1); \
        obj.sliced_tex2 = IMG_LoadTexture(app.g_renderer, path2); \
        if (!obj.sliced_tex1) fprintf(stderr, "[WARNING] 이미지 로드 실패 %s: %s\n", path1, IMG_GetError()); \
        if (!obj.sliced_tex2) fprintf(stderr, "[WARNING] 이미지 로드 실패 %s: %s\n", path2, IMG_GetError());

    // 1. 배추
    LOAD_TEX(cabbage, "./gfx/cabbage.png");
    LOAD_SPLIT_TEX(cabbage, "./gfx/s_cabbage1.png", "./gfx/s_cabbage2.png");

    // 2. 고기
    LOAD_TEX(meat, "./gfx/meat.png");
    LOAD_SPLIT_TEX(meat, "./gfx/s_meat1.png", "./gfx/s_meat2.png");

    // 3. 버섯
    LOAD_TEX(mushroom, "./gfx/mushroom.png");
    LOAD_SPLIT_TEX(mushroom, "./gfx/s_mushroom1.png", "./gfx/s_mushroom2.png");

    // 4. 콩나물
    LOAD_TEX(beanSprouts, "./gfx/bean_sprouts.png");
    LOAD_SPLIT_TEX(beanSprouts, "./gfx/s_bean_sprouts1.png", "./gfx/s_bean_sprouts2.png");

    // 5. 함정 (신발, 돌) - 잘린 이미지가 없으므로 NULL
    LOAD_TEX(shoes, "./gfx/shoes.png");
    shoes.sliced_tex1 = NULL; shoes.sliced_tex2 = NULL;

    LOAD_TEX(stone, "./gfx/stone.png");
    stone.sliced_tex1 = NULL; stone.sliced_tex2 = NULL;


    LOAD_TEX(heart_red, "./gfx/heart_red.png");
    LOAD_TEX(heart_black, "./gfx/heart_black.png");
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
    if (gameover_text.texture) SDL_DestroyTexture(gameover_text.texture);
    if (restart_text.texture) SDL_DestroyTexture(restart_text.texture);
    if (gameover_score_text.texture) SDL_DestroyTexture(gameover_score_text.texture);
    if (title_text.texture) SDL_DestroyTexture(title_text.texture);
    if (subtitle_text.texture) SDL_DestroyTexture(subtitle_text.texture);
    if (rule1_text.texture) SDL_DestroyTexture(rule1_text.texture);
    if (rule2_text.texture) SDL_DestroyTexture(rule2_text.texture);
    if (rule3_text.texture) SDL_DestroyTexture(rule3_text.texture);

    if (app.title_font && app.title_font != app.font) TTF_CloseFont(app.title_font);
    if (app.font) TTF_CloseFont(app.font);
    if (app.g_renderer) SDL_DestroyRenderer(app.g_renderer);
    if (app.g_window) SDL_DestroyWindow(app.g_window);

    if (cabbage.sliced_tex1) SDL_DestroyTexture(cabbage.sliced_tex1);
    if (cabbage.sliced_tex2) SDL_DestroyTexture(cabbage.sliced_tex2);
    
    if (meat.sliced_tex1) SDL_DestroyTexture(meat.sliced_tex1);
    if (meat.sliced_tex2) SDL_DestroyTexture(meat.sliced_tex2);
    
    if (mushroom.sliced_tex1) SDL_DestroyTexture(mushroom.sliced_tex1);
    if (mushroom.sliced_tex2) SDL_DestroyTexture(mushroom.sliced_tex2);
    
    if (beanSprouts.sliced_tex1) SDL_DestroyTexture(beanSprouts.sliced_tex1);
    if (beanSprouts.sliced_tex2) SDL_DestroyTexture(beanSprouts.sliced_tex2);



    CleanupSound(); //사운드 시스템 정리 추가

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

}

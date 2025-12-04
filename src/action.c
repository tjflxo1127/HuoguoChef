#include "action.h"
#include "utils.h"
#include "sound.h"  

extern App app;

extern Ingredient cabbage;
extern Ingredient meat;
extern Ingredient mushroom;
extern Ingredient beanSprouts;
extern Ingredient shoes;
extern Ingredient stone;

#define SPAWN_INTERVAL 40
#define LAUNCH_SPEED_MIN 18.0f 
#define LAUNCH_SPEED_MAX 25.0f 

static int spawn_timer = 0;

void SpawnIngredient(void);
void CreateFragment(Ingredient *parent, int part);

void ActGame(void) {
    if (app.game.game_over) return;

    spawn_timer++;
    if (spawn_timer > SPAWN_INTERVAL) {
        SpawnIngredient();
        spawn_timer = 0;
    }

    ActIngredients(app.game.ingredients, MAX_INGREDIENTS);
}

// [조각 생성 함수]
// parent: 원래 재료, part: 1(왼쪽) 또는 2(오른쪽)
void CreateFragment(Ingredient *parent, int part) {
    // 빈 슬롯 찾기
    int idx = -1;
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        if (!app.game.ingredients[i].is_active) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return; // 슬롯 꽉 참

    Ingredient *frag = &app.game.ingredients[idx];

    // 1. 기본 상태 설정
    frag->is_active = 1;
    frag->is_sliced = 1; // 이미 잘린 상태로 취급 (점수 중복 획득 방지)
    frag->is_enemy = 0;
    frag->type = parent->type;

    // 2. 텍스처 설정 (잘린 이미지 사용)
    SDL_Texture *use_tex = (part == 1) ? parent->sliced_tex1 : parent->sliced_tex2;
    frag->texture = use_tex;
    
    // 텍스처가 없으면 생성 취소 (함정 아이템 등 잘린 이미지가 없는 경우)
    if (!use_tex) { 
        frag->is_active = 0; 
        return; 
    }

    // [크기 설정] 
    // 부모의 절반 너비, 높이는 동일하게 설정 (비율 유지된 부모 크기 기준)
    frag->w = parent->w / 2; 
    frag->h = parent->h;

    // 3. 위치 및 물리 설정
    // 부모 위치를 기준으로 왼쪽 조각은 약간 왼쪽, 오른쪽 조각은 약간 오른쪽에 배치
    int offset = parent->w / 4; 
    frag->x = parent->x + (part == 1 ? -offset : offset); 
    frag->y = parent->y;
    
    // 속도 설정: 원래 속도 유지하되, 좌우로 퍼지게
    float spread = (part == 1) ? -4.0f : 4.0f; // 왼쪽은 -속도, 오른쪽은 +속도
    frag->dx = parent->dx + spread; 
    frag->dy = parent->dy - 3.0f; // 베였을 때 살짝 위로 튀어오름
}


void SpawnIngredient(void) {
    int idx = -1;
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        if (!app.game.ingredients[i].is_active) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return;

    Ingredient *ing = &app.game.ingredients[idx];

    ing->is_active = 1;
    ing->is_sliced = 0;
    

    int center_x = SCREEN_WIDTH / 2;
    int offset_x = RandInt(-50, 51);
    ing->x = (float)(center_x + offset_x);
    ing->y = (float)SCREEN_HEIGHT;

    double angle_deg = RandDouble(60.0, 120.0);
    double angle_rad = DegToRad(angle_deg);
    double speed = RandDouble(LAUNCH_SPEED_MIN, LAUNCH_SPEED_MAX);

    ing->dx = (float)(speed * cos(angle_rad));
    ing->dy = (float)(-speed * sin(angle_rad));

    ing->type = RandInt(0, 6);

    switch (ing->type) {
        case MUSHROOM: ing->texture = mushroom.texture; ing->is_enemy = 0; break;
        case CABBAGE: ing->texture = cabbage.texture; ing->is_enemy = 0; break;
        case MEAT: ing->texture = meat.texture; ing->is_enemy = 0; break;
        case BEANSPROUTS: ing->texture = beanSprouts.texture; ing->is_enemy = 0; break;
        case SHOES: ing->texture = shoes.texture; ing->is_enemy = 1; break;
        case STONE: ing->texture = stone.texture; ing->is_enemy = 1; break;  // 함정 아이템인 신발과 돌은 1 나머지는 0
        default: ing->texture = cabbage.texture; ing->is_enemy = 0; break;
    }
    
    //비율 유지하며 크기 조절하는 로직
    int origin_w, origin_h;
    // 현재 연결된 이미지의 실제 크기를 알아냅니다.
    SDL_QueryTexture(ing->texture, NULL, NULL, &origin_w, &origin_h);

    float scale = 1.0f;
    float target_size = (float)INGREDIENTS_SIZE; // 목표 크기 (80px)

    if (origin_w > 0 && origin_h > 0) {
        // 가로가 더 길면 가로를 80에 맞추고, 세로가 더 길면 세로를 80에 맞춤
        if (origin_w > origin_h) {
            scale = target_size / (float)origin_w;
        } else {
            scale = target_size / (float)origin_h;
        }
    }

    // 2. 계산된 비율대로 크기 적용
    ing->w = (int)(origin_w * scale);
    ing->h = (int)(origin_h * scale);
    // 원래 : 이제 이미지가 아무리 커도 8x8 크기로 찌그러져서 나옴 -> 비율 이상해짐 -> 위 코드로 해결
}

void ActIngredients(Ingredient *ingredients, int count) {
    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        if (ing->is_active) {
            ing->dy += GRAVITY;
            ing->x += ing->dx;
            ing->y += ing->dy;

            if (CheckOutBound(ing)) {
                if (!ing->is_sliced && !ing->is_enemy) {
                    app.game.lives--;
                    if (app.game.lives <= 0) {
                        app.game.game_over = 1;
                        StopBGM(); // 게임 오버 시 BGM 정지
                    }
                }
                ing->is_active = 0;
            }
        }
    }
}



void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2) {
    if (app.game.game_over) return;

    int hit_indices[MAX_INGREDIENTS];
    int hit_count = 0;
    int has_enemy = 0;

    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        if (ing->is_active && !ing->is_sliced) {
            SDL_Rect rect = { (int)ing->x, (int)ing->y, ing->w, ing->h };

            if (CheckLineRectHit(x1, y1, x2, y2, &rect)) {
                hit_indices[hit_count++] = i;
                if (ing->is_enemy) has_enemy = 1;
            }
        }
    }

    if (hit_count > 0) {
        if (has_enemy) {
            PlayBombSound(); // 폭탄, 신발, 돌 베었을 때 터짐
            if (app.game.lives > 0) {  // 라이프가 0보다 클 때만 깎음
                app.game.lives--;
            }
            if (app.game.lives <= 0) {
                app.game.game_over = 1;
                StopBGM(); // 게임 오버 시 BGM 정지
            }
        } else {
            PlaySlashSound(); // 일반 재료 베면 효과음!
            for (int k = 0; k < hit_count; k++) {
                int idx = hit_indices[k];
                Ingredient *target = &ingredients[idx];
                target->is_sliced = 1;
                app.game.score += 10;
                target->dy = -5.0f;
            }
        }
    }
}

void ActGameOver(void) {}

// [추가] 게임 재시작 로직
void ResetGame(void) {
    app.game.score = 0;
    app.game.lives = 3;
    app.game.game_over = 0;
    
   // 모든 재료 제거
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        app.game.ingredients[i].is_active = 0;
    }

    // 게임 재시작 시 BGM 다시 재생
    PlayBGM();
}

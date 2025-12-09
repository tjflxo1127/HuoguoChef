/**
 * @file    action.c
 * @brief   재료의 움직임을 구현한 함수 정의 
 * @author  조정배 팀
 */
#include "action.h"

static int spawn_timer = 0;

static int GetCurrentSpawnInterval(void) {
    // 점수에 비례하여 감소할 값 계산 (DIFFICULTY_SCORE_RATE값 당당 1 감소)
    int reduction = app.game.score / DIFFICULTY_SCORE_RATE; 

    // 최대값에서 감소분을 빼서 현재 간격을 구함
    int current_interval = SPAWN_INTERVAL_MAX - reduction;

    // 간격이 최소값(SPAWN_INTERVAL_MIN)보다 작아지지 않도록 보정
    if (current_interval < SPAWN_INTERVAL_MIN) {
        current_interval = SPAWN_INTERVAL_MIN;
    }

    return current_interval;
}

// [추가] 스택에 재료 추가하는 내부 함수
static void AddToStack(Ingredient *ing) {
    int idx = app.game.stack_head;
    
    // 현재 정보를 스택에 저장 (이미지, 위치, 크기)
    app.game.stack[idx].x = (int)ing->x;
    app.game.stack[idx].y = (int)ing->y;
    app.game.stack[idx].w = ing->w;
    app.game.stack[idx].h = ing->h;
    app.game.stack[idx].texture = ing->texture;
    app.game.stack[idx].is_valid = 1;

    // 인덱스 증가 (원형 버퍼: 꽉 차면 0번부터 다시 덮어씀 -> 오래된 것 자동 삭제 효과)
    app.game.stack_head = (idx + 1) % MAX_STACK_INGREDIENTS;
}

// [추가] 화면 밖으로 나갔을 때 강제로 냄비 안에 랜덤 배치하는 함수
static void AddToStackRandom(Ingredient *ing) {
    // 냄비 범위 내 랜덤 위치 설정
    ing->x = (float)RandInt(POT_X, POT_X + POT_W - ing->w);
    ing->y = (float)RandInt(POT_Y, POT_Y + POT_H - ing->h);
    
    AddToStack(ing);
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
    //재료 포인터 ing
    Ingredient *ing = &app.game.ingredients[idx];
    //재료 성질 초기화(존재하고, 안잘림)
    ing->is_active = 1;
    ing->is_sliced = 0;                                       
    
    //재료 위치 초기화(x:중심에서 랜덤 offset 거리, y:화면 바닥)
    int center_x = SCREEN_WIDTH / 2;
    int offset_x = RandInt(-50, 51);
    ing->x = (float)(center_x + offset_x);
    ing->y = (float)SCREEN_HEIGHT;                      
    double angle_deg = RandDouble(60.0, 120.0);
    double angle_rad = angle_deg * (PI / 180.0); // 각도 -> 라디안
    double speed = RandDouble(LAUNCH_SPEED_MIN, LAUNCH_SPEED_MAX);
    
    //재료 속도 초기화
    ing->dx = (float)(speed * cos(angle_rad));
    ing->dy = (float)(-speed * sin(angle_rad));            
    //재료 종류 랜덤 초기화
    ing->type = RandInt(0, 6);                         
    //switch문 이용해서 초기화된 재료에 맞게 이미지 할당, 적인지 확인
    switch (ing->type) {                                
        case MUSHROOM: 
            ing->texture = mushroom.texture; 
            ing->sliced_tex1 = mushroom.sliced_tex1; 
            ing->sliced_tex2 = mushroom.sliced_tex2;
            ing->is_enemy = 0; 
            break;
        case CABBAGE: 
            ing->texture = cabbage.texture; 
            ing->sliced_tex1 = cabbage.sliced_tex1; 
            ing->sliced_tex2 = cabbage.sliced_tex2;
            ing->is_enemy = 0; 
            break;
        case MEAT: 
            ing->texture = meat.texture; 
            ing->sliced_tex1 = meat.sliced_tex1; 
            ing->sliced_tex2 = meat.sliced_tex2;
            ing->is_enemy = 0; 
            break;
        case BEANSPROUTS: 
      
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

    // [추가] 초기 target_y 설정 (일반 재료는 사용 안 하지만 초기화)
    ing->target_y = 0;
}

// 조각 생성 함수
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
    //재료 포인터 frag 
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

    // frag 크기 설정
    // 부모의 절반 너비, 높이는 동일하게 설정 (비율 유지된 부모 크기 기준)
    frag->w = parent->w / 2; 
    frag->h = parent->h;

    // 3. 위치 및 물리 설정
    // 부모 위치를 기준으로 왼쪽 조각은 약간 왼쪽, 오른쪽 조각은 약간 오른쪽에 배치
    int offset = parent->w / 4; 
    frag->x = parent->x + (part == 1 ? -offset : offset); //삼항연산자를 이용해 x위치 배치
    frag->y = parent->y;
    
    // 잘랐을 때 자연스러운 모션을 위해 좌우로 퍼지게, 위로 약간 튀어오르게 속도를 조정
    float spread = (part == 1) ? -4.0f : 4.0f; //삼항연산자를 이용해 왼쪽은 -속도, 오른쪽은 +속도
    frag->dx = parent->dx + spread; 
    frag->dy = parent->dy - 3.0f; 

    // [추가] 냄비 안 어디에서 멈출지 결정 (target_y)
    // 냄비 시작 Y(350) 부터 냄비 바닥(350+250) 사이의 랜덤 위치
    // 단, 재료 크기(h)를 고려하여 바닥을 뚫지 않게 조정
    int min_y = POT_Y;
    int max_y = POT_Y + POT_H - frag->h;
    if (max_y < min_y) max_y = min_y; // 예외 처리
    frag->target_y = (float)RandInt(min_y, max_y);

}

void ActIngredients(Ingredient *ingredients, int count) {
    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        if (ing->is_active) {
            ing->dy += GRAVITY;    //y방향으로 중력가속도 받도록 설정
            ing->x += ing->dx;    //속도(dx,dy)에 따라 x,y위치 변화
            ing->y += ing->dy;

        // 1. 왼쪽 벽 충돌 (x < 0)
            if (ing->x < 0) {
                ing->x = 0;          // 벽 밖으로 나가지 않게 위치 보정
                ing->dx = -ing->dx;  // 속도 반전 (튕기기)
            }
            // 2. 오른쪽 벽 충돌 (x + w > SCREEN_WIDTH) 재료의 너비인 w 고려!
            else if (ing->x + ing->w > SCREEN_WIDTH) {
                ing->x = SCREEN_WIDTH - ing->w; // 벽 안쪽으로 위치 보정
                ing->dx = -ing->dx;             // 속도 반전
            }

            
            // [추가] 냄비에 쌓이는 로직 (조건 1, 2, 3)
            // 조건: 잘린 재료이고(is_sliced), 적(함정)이 아님
            if (ing->is_sliced && !ing->is_enemy) {
                // X축 범위 확인: 냄비 범위(POT_X ~ POT_X + POT_W) 안에 있는가?
                if (ing->x + ing->w > POT_X && ing->x < POT_X + POT_W) {
                    // Y축 깊이 확인: 목표 지점(target_y)에 도달했는가?
                    if (ing->y >= ing->target_y) {
                        // 위치 고정 및 스택 추가
                        ing->y = ing->target_y; // 정확한 위치로 보정
                        AddToStack(ing);        // 배경 스택에 추가
                        ing->is_active = 0;     // 활성 목록에서 제거 (물리 연산 중단)
                        continue;               // 다음 재료로 넘어감
                    }
                }
            }

            if (ing->y > SCREEN_HEIGHT) {  // 재료가 화면에 나타날 때

                //잘린 재료가 냄비를 빗나가서 화면 밖으로 나가면 냄비 안 랜덤 위치에 강제로 쌓임 
                if (ing->is_sliced && !ing->is_enemy) { 
                    AddToStackRandom(ing);
                }
                // 적이거나 안 잘린 재료가 떨어지면 목숨 차감 로직
                else if (!ing->is_sliced && !ing->is_enemy) {
                    app.game.lives--;
                    PlayMissingredientSound(); // 재료 놓침 효과음
                    if (app.game.lives <= 0) {
                        app.game.game_over = 1;
                        StopBGM(); // 게임 오버 시 BGM 정지
                        PlayGameOverSound(); // 게임 오버 효과음
                    }
                }
                ing->is_active = 0;
            }
        }
    }
}

void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2) {
    if (app.game.game_over) return;

    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        // 활성 상태이고 아직 잘리지 않은 재료만 체크
        if (ing->is_active && !ing->is_sliced) {
            SDL_Rect rect = { (int)ing->x, (int)ing->y, ing->w, ing->h };

            if (CheckLineRectHit(x1, y1, x2, y2, &rect)) {
                
                // 1. 함정(적)인 경우: 즉시 게임 오버
                if (ing->is_enemy) {
                    PlayBombSound();
                    app.game.lives = 0;     // 라이프 즉시 0
                    app.game.game_over = 1; // 게임 오버
                    StopBGM();              // BGM 정지
                    ing->is_active = 0; 
                } 
                // 2. 일반 재료인 경우: 점수 추가 및 조각 생성
                else {
                    PlaySlashSound();
                    if (ing->type == MEAT) {
                        app.game.score += 30; //우삼겹 30점!!
                    } else {
                        app.game.score += 10;
                    }
                    
                    // 원본 재료는 화면에서 제거
                    ing->is_active = 0;

                    // [핵심] 조각 2개(왼쪽, 오른쪽) 생성
                    CreateFragment(ing, 1); 
                    CreateFragment(ing, 2); 
                }
            }
        }
    }
}

void update_game(void) {
    if (app.game.game_over || app.game.title_screen) return;


    // [수정] 현재 난이도에 맞는 스폰 간격을 계산
    int current_interval = GetCurrentSpawnInterval(); 

    spawn_timer++;
    if (spawn_timer > current_interval) {
        SpawnIngredient();
        spawn_timer = 0;
    }

    ActIngredients(app.game.ingredients, MAX_INGREDIENTS);
}

// 게임 재시작 함수
void ResetGame(void) {
    app.game.score = 0;
    app.game.lives = 3;
    app.game.game_over = 0;
    app.game.title_screen = 0;  // 게임 재시작 시 타이틀 화면 표시 안 함
    
   // 모든 재료 제거
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        app.game.ingredients[i].is_active = 0;
    }

    // [추가] 스택(냄비에 쌓인 재료) 초기화
    app.game.stack_head = 0;
    for (int i = 0; i < MAX_STACK_INGREDIENTS; i++) {
        app.game.stack[i].is_valid = 0;
        app.game.stack[i].texture = NULL;
    }

    // 게임 재시작 시 BGM 다시 재생
    PlayBGM();
}

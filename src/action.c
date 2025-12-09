/**
 * @file    action.c
 * @brief   재료의 움직임과 게임 로직을 구현한 함수 정의 
 * @author  조정배 팀
 * 
 * 이 파일은 게임의 핵심 로직을 담당한다:
 * - 재료 생성 및 스폰 (난이도에 따른 스폰 간격 조절)
 * - 재료의 물리 연산 (중력, 충돌, 냄비에 쌓이기)
 * - 베기 충돌 감지 및 조각 생성
 * - 게임 상태 업데이트
 */
#include "action.h"

// 재료 스폰 타이머 (프레임 단위)
static int spawn_timer = 0;

/**
 * @brief   현재 점수에 따라 스폰 간격을 계산한다.
 * @return  현재 난이도에 맞는 스폰 간격 (프레임 단위)
 * 
 * 점수가 높아질수록 스폰 간격이 짧아져 게임이 어려워진다.
 * DIFFICULTY_SCORE_RATE 점수마다 간격이 1프레임씩 감소한다.
 * 최소 간격(SPAWN_INTERVAL_MIN)은 보장된다.
 */
static int GetCurrentSpawnInterval(void) {
    // 점수에 비례하여 감소할 값 계산 (DIFFICULTY_SCORE_RATE값 당 1 감소)
    int reduction = app.game.score / DIFFICULTY_SCORE_RATE; 

    // 최대값에서 감소분을 빼서 현재 간격을 구함
    int current_interval = SPAWN_INTERVAL_MAX - reduction;

    // 간격이 최소값(SPAWN_INTERVAL_MIN)보다 작아지지 않도록 보정
    if (current_interval < SPAWN_INTERVAL_MIN) {
        current_interval = SPAWN_INTERVAL_MIN;
    }

    return current_interval;
}

/**
 * @brief   잘린 재료를 냄비 스택에 추가한다.
 * @param   ing 스택에 추가할 재료 포인터
 * 
 * 재료의 현재 위치, 크기, 텍스처 정보를 스택에 저장한다.
 * 원형 버퍼를 사용하여 MAX_STACK_INGREDIENTS 개수만큼만 유지하며,
 * 초과 시 가장 오래된 재료부터 덮어쓴다.
 */
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

/**
 * @brief   화면 밖으로 나간 잘린 재료를 냄비 안 랜덤 위치에 강제 배치한다.
 * @param   ing 배치할 재료 포인터
 * 
 * 재료가 냄비를 빗나가 화면 밖으로 떨어졌을 때,
 * 냄비 범위 내의 랜덤 위치에 배치하여 시각적 연속성을 유지한다.
 */
static void AddToStackRandom(Ingredient *ing) {
    // 냄비 범위 내 랜덤 위치 설정 (재료 크기를 고려하여 경계를 넘지 않도록)
    ing->x = (float)RandInt(POT_X, POT_X + POT_W - ing->w);
    ing->y = (float)RandInt(POT_Y, POT_Y + POT_H - ing->h);
    
    AddToStack(ing);
}

/**
 * @brief   새로운 재료를 화면 아래에서 생성하고 발사한다.
 * 
 * 활성화되지 않은 슬롯을 찾아 새로운 재료를 생성한다.
 * 재료는 화면 하단 중앙에서 약간의 랜덤 오프셋을 가지고
 * 60~120도 각도로 랜덤 속도로 발사된다.
 * 재료 타입은 랜덤하게 결정되며, 식재료 또는 함정(신발/돌)이 될 수 있다.
 */
void SpawnIngredient(void) {
    // 비활성 슬롯 찾기
    int idx = -1;
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        if (!app.game.ingredients[i].is_active) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return; // 슬롯이 모두 사용 중이면 생성 불가
                    
    Ingredient *ing = &app.game.ingredients[idx];
    
    // 재료 기본 상태 초기화 (활성화, 미절단 상태)
    ing->is_active = 1;
    ing->is_sliced = 0;                                       
    
    // 재료 위치 초기화: 화면 하단 중앙에서 랜덤 오프셋
    int center_x = SCREEN_WIDTH / 2;
    int offset_x = RandInt(-50, 51); // -50 ~ 50 픽셀 랜덤 오프셋
    ing->x = (float)(center_x + offset_x);
    ing->y = (float)SCREEN_HEIGHT; // 화면 바닥에서 시작
                      
    // 발사 각도와 속도 설정 (60~120도, 중력에 의해 포물선 운동)
    double angle_deg = RandDouble(60.0, 120.0); // 위로 향하는 각도 범위
    double angle_rad = angle_deg * (PI / 180.0); // 각도 -> 라디안 변환
    double speed = RandDouble(LAUNCH_SPEED_MIN, LAUNCH_SPEED_MAX); // 랜덤 속도
    
    // 재료 속도 벡터 초기화 (각도와 속도로부터 x, y 속도 계산)
    ing->dx = (float)(speed * cos(angle_rad));  // x축 속도 (좌우)
    ing->dy = (float)(-speed * sin(angle_rad)); // y축 속도 (위쪽, 음수)            
    
    // 재료 종류 랜덤 선택 (0~5: 버섯, 배추, 고기, 콩나물, 신발, 돌)
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
            ing->texture = beanSprouts.texture; 
            ing->sliced_tex1 = beanSprouts.sliced_tex1; 
            ing->sliced_tex2 = beanSprouts.sliced_tex2;
            ing->is_enemy = 0; 
            break;
        case SHOES: 
            ing->texture = shoes.texture; 
            ing->sliced_tex1 = shoes.sliced_tex1; 
            ing->sliced_tex2 = shoes.sliced_tex2;
            ing->is_enemy = 1; 
            break;
        case STONE: 
            ing->texture = stone.texture; 
            ing->sliced_tex1 = stone.sliced_tex1; 
            ing->sliced_tex2 = stone.sliced_tex2;
            ing->is_enemy = 1; 
            break;
    }
    
    // 텍스처 크기 계산 (모든 재료 타입에 공통)
    int origin_w, origin_h;
    SDL_QueryTexture(ing->texture, NULL, NULL, &origin_w, &origin_h);

    float scale = 1.0f;
    float target_size = (float)INGREDIENTS_SIZE; // 목표 크기 (100px)

    if (origin_w > 0 && origin_h > 0) {
        // 가로가 더 길면 가로를 목표 크기에 맞추고, 세로가 더 길면 세로를 목표 크기에 맞춤
        if (origin_w > origin_h) {
            scale = target_size / (float)origin_w;
        } else {
            scale = target_size / (float)origin_h;
        }
    }

    // 계산된 비율대로 크기 적용
    ing->w = (int)(origin_w * scale);
    ing->h = (int)(origin_h * scale);

    // 초기 target_y 설정 (일반 재료는 사용 안 하지만 초기화)
    ing->target_y = 0;
}

/**
 * @brief   베어진 재료로부터 좌우 조각을 생성한다.
 * @param   parent 베어진 원본 재료 포인터
 * @param   part 조각 번호 (1: 왼쪽 조각, 2: 오른쪽 조각)
 * 
 * 재료가 베어지면 원본은 제거되고 좌우 두 개의 조각이 생성된다.
 * 각 조각은 잘린 텍스처를 사용하며, 좌우로 퍼지며 떨어지는 물리 효과를 받는다.
 * 조각은 냄비에 떨어지면 스택에 쌓인다.
 */
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

/**
 * @brief   모든 활성 재료의 물리 연산 및 상태 업데이트를 수행한다.
 * @param   ingredients 재료 배열 포인터
 * @param   count 재료 배열 크기
 * 
 * 이 함수는 매 프레임 호출되어 다음을 처리한다:
 * - 중력 적용 및 위치 업데이트
 * - 좌우 벽 충돌 처리 (튕기기)
 * - 냄비 범위 내 잘린 재료를 스택에 추가
 * - 화면 밖으로 나간 재료 처리 (목숨 차감 또는 강제 배치)
 */
void ActIngredients(Ingredient *ingredients, int count) {
    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        if (ing->is_active) {
            // 중력 적용: 매 프레임마다 y축 속도 증가
            ing->dy += GRAVITY;
            
            // 속도에 따른 위치 업데이트
            ing->x += ing->dx;
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

/**
 * @brief   마우스 궤적(칼날)과 재료의 충돌을 검사하고 베기 처리를 한다.
 * @param   ingredients 재료 배열 포인터
 * @param   count 재료 배열 크기
 * @param   x1 마우스 궤적 시작점 x 좌표
 * @param   y1 마우스 궤적 시작점 y 좌표
 * @param   x2 마우스 궤적 끝점 x 좌표
 * @param   y2 마우스 궤적 끝점 y 좌표
 * 
 * 마우스가 움직일 때 호출되어 선분(마우스 궤적)과 재료 사각형의 교차를 검사한다.
 * 일반 재료는 베어서 점수를 얻고, 함정(신발/돌)은 게임 오버를 유발한다.
 */
void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2) {
    // 게임 오버 상태에서는 베기 불가
    if (app.game.game_over) return;

    for (int i = 0; i < count; i++) {
        Ingredient *ing = &ingredients[i];

        // 활성 상태이고 아직 잘리지 않은 재료만 충돌 검사
        if (ing->is_active && !ing->is_sliced) {
            // 재료의 사각형 영역 정의
            SDL_Rect rect = { (int)ing->x, (int)ing->y, ing->w, ing->h };

            // 선분(마우스 궤적)과 사각형(재료)의 충돌 검사
            if (CheckLineRectHit(x1, y1, x2, y2, &rect)) {
                
                // 함정(신발/돌)인 경우: 즉시 게임 오버
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

/**
 * @brief   게임 상태를 매 프레임 업데이트한다.
 * 
 * 이 함수는 게임 루프에서 매 프레임 호출된다.
 * 타이틀 화면이나 게임 오버 상태에서는 업데이트를 수행하지 않는다.
 * - 난이도에 따른 재료 스폰
 * - 모든 재료의 물리 연산 및 상태 업데이트
 */
void update_game(void) {
    // 타이틀 화면이나 게임 오버 상태에서는 게임 로직 실행 안 함
    if (app.game.game_over || app.game.title_screen) return;

    // 현재 점수에 따른 난이도에 맞는 스폰 간격 계산
    int current_interval = GetCurrentSpawnInterval(); 

    // 스폰 타이머 증가 및 재료 생성
    spawn_timer++;
    if (spawn_timer > current_interval) {
        SpawnIngredient(); // 새로운 재료 생성
        spawn_timer = 0;   // 타이머 리셋
    }

    // 모든 활성 재료의 물리 연산 수행
    ActIngredients(app.game.ingredients, MAX_INGREDIENTS);
}

/**
 * @brief   게임 상태를 초기 상태로 리셋한다.
 * 
 * 게임 오버 후 재시작 시 또는 처음 시작할 때 호출된다.
 * 점수, 목숨, 모든 재료와 스택을 초기화하고 BGM을 재생한다.
 */
void ResetGame(void) {
    // 게임 상태 초기화
    app.game.score = 0;
    app.game.lives = 3;
    app.game.game_over = 0;
    app.game.title_screen = 0;  // 게임 재시작 시 타이틀 화면 표시 안 함
    
    // 모든 활성 재료 제거
    for (int i = 0; i < MAX_INGREDIENTS; i++) {
        app.game.ingredients[i].is_active = 0;
    }

    // 스택(냄비에 쌓인 재료) 초기화
    app.game.stack_head = 0;
    for (int i = 0; i < MAX_STACK_INGREDIENTS; i++) {
        app.game.stack[i].is_valid = 0;
        app.game.stack[i].texture = NULL;
    }

    // 게임 재시작 시 BGM 다시 재생
    PlayBGM();
}

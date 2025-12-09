/**
 * @file    draw.c
 * @brief   텍스처 렌더링을 위한 함수 정의 
 * @author  조정배 팀
 * 이 파일은 게임의 모든 렌더링 관련 함수들을 포함한다.
 * - 화면 초기화 및 업데이트
 * - 텍스트 렌더링 (일반 텍스트, 타이틀 텍스트)
 * - 게임 객체 렌더링 (재료, 스택, UI 요소)
 * - 마우스 궤적 시각화
 */
#include "draw.h"

// 1. 화면 초기화
/**
 * @brief   렌더러를 검은색으로 초기화하고 배경 이미지를 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 이 함수는 매 프레임마다 호출되어 화면을 초기화한다.
 * 배경 이미지가 로드되지 않은 경우 검은 배경을 사용한다.
 */
void ClearWindow(App *app) {
    SDL_SetRenderDrawColor(app->g_renderer, 0, 0, 0, 255); // 배경 이미지를 사용할 것이지만 이미지 로드 실패했을 경우 검은 배경 초기화
    SDL_RenderClear(app->g_renderer);

    // 배경 이미지 화면 출력
    if (app->background_texture != NULL) {//  (로드된 경우에만)
        SDL_RenderCopy(app->g_renderer, app->background_texture, NULL, NULL);
    }
}

// 2. 화면 보여주기
/**
 * @brief   렌더링된 내용을 화면에 표시한다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 이 함수는 더블 버퍼링의 백 버퍼에 그려진 모든 내용을
 * 프론트 버퍼로 교체하여 화면에 표시한한다.
 */
void 
void ShowWindow(App *app) {
    SDL_RenderPresent(app->g_renderer);
}

// 3. 글자 -> 그림으로 변환 (String -> Texture 변환)
/**
 * @brief   문자열을 작은 폰트로 텍스처로 변환한다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   text_obj 텍스트 객체 포인터 (결과가 저장됨)
 * @param   str 렌더링할 문자열
 * @param   color 텍스트 색상
 * 
 * 이 함수는 문자열을 SDL Surface로 렌더링한 후 Texture로 변환한다.
 * 기존 텍스처가 있으면 먼저 해제하여 메모리 누수를 방지한다.
 */
void TextureSmallText(App *app, TextObject *text_obj, char *str, SDL_Color color) {
    // 폰트 로드 실패 시 중단
    if (app->font == NULL)
        return;

    // 기존 텍스처 삭제 (메모리 누수 방지)
    if (text_obj->texture != NULL) {
        SDL_DestroyTexture(text_obj->texture);
        text_obj->texture = NULL;
    }

    // Surface 생성
    SDL_Surface *surface = TTF_RenderText_Solid(app->font, str, color);
    if (surface == NULL)
        return;

    // Texture 생성
    text_obj->texture = SDL_CreateTextureFromSurface(app->g_renderer, surface);
    
    if (text_obj->texture != NULL) {
        text_obj->rect.w = surface->w;
        text_obj->rect.h = surface->h;
    }

    SDL_FreeSurface(surface);
}

// 3-1. 타이틀용 큰 텍스트 그림으로 변환 
/**
 * @brief   문자열을 큰 폰트(타이틀용)로 텍스처로 변환한다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   text_obj 텍스트 객체 포인터 (결과가 저장됨)
 * @param   str 렌더링할 문자열
 * @param   color 텍스트 색상
 * 
 * 일반 텍스트보다 2배 큰 폰트를 사용하여 타이틀 화면에 사용된다.
 */
void TextureBigText(App *app, TextObject *text_obj, char *str, SDL_Color color) {
    // 타이틀 폰트 로드 실패 시 중단
    if (app->title_font == NULL) return;

    // 기존 텍스처 삭제 (메모리 누수 방지)
    if (text_obj->texture != NULL) {
        SDL_DestroyTexture(text_obj->texture);
        text_obj->texture = NULL;
    }

    // Surface 생성 (큰 폰트 사용)
    SDL_Surface *surface = TTF_RenderText_Solid(app->title_font, str, color);
    if (surface == NULL) return;

    // Texture 생성
    text_obj->texture = SDL_CreateTextureFromSurface(app->g_renderer, surface);

    if (text_obj->texture != NULL) {
        text_obj->rect.w = surface->w;
        text_obj->rect.h = surface->h;
    }

    SDL_FreeSurface(surface);
}

// 4. 개별 재료 그리기 (날아다니는 재료들)
/**
 * @brief   개별 재료(식재료 또는 함정)를 화면에 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   ing 그릴 재료 객체 포인터
 * 
 * 재료가 잘린 상태(is_sliced)인 경우 어둡고 반투명하게 렌더링하여
 * 시각적으로 구분할 수 있도록 한다. 
 */
void RenderEntity(App *app, Ingredient *ing) {
    if (ing->texture == NULL) return;

    SDL_Rect dest; 
    dest.x = (int)ing->x; // 재료의 좌표(float타입)을 화면 픽셀 좌표(int타입)으로 바꿔서 저장
    dest.y = (int)ing->y;
    dest.w = ing->w;
    dest.h = ing->h;

    // 잘린 재료 시각 효과 : 잘리지 않은 재료와 구분
    if (ing->is_sliced) {
        // 잘린 후 색상을 어둡게 (R, G, B 값을 줄임)
        SDL_SetTextureColorMod(ing->texture, 150, 150, 150);
        // 약간 반투명하게 바꿈
        SDL_SetTextureAlphaMod(ing->texture, 200);
    } else {
        // 원래 색상과 불투명도 복구
        SDL_SetTextureColorMod(ing->texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(ing->texture, 255);
    }


    // 텍스처 그리기
    SDL_RenderCopy(app->g_renderer, ing->texture, NULL, &dest);
}

// 5. 스택에 쌓인 재료 그리기
/**
 * @brief   냄비 안에 쌓인 재료들을 배경으로 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 잘려서 냄비 안에 떨어진 재료들은 스택에 저장되며,
 * 배경 레이어로 렌더링된다. 활성 재료보다 아래에 그려져
 * 냄비 안에 쌓인 느낌을 연출한다.
 * 어둡게 처리하여 물에 잠긴 느낌을 준다.
 */
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

// 6. 텍스트 객체 그리기
/**
 * @brief   텍스트 객체를 화면에 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   text_obj 그릴 텍스트 객체 포인터
 * 
 * 점수판, 게임 오버 메시지 등 모든 텍스트 UI 요소를
 * 이 함수를 통해 렌더링한다.
 */
void RenderScoreBoard(App *app, TextObject *text_obj) {
    if (text_obj->texture != NULL) {
        SDL_RenderCopy(app->g_renderer, text_obj->texture, NULL, &text_obj->rect);
    }
}

// 7. 게임 전체 그리기 (메인 함수)
/**
 * @brief   게임 화면의 모든 요소를 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   score 점수판 텍스트 객체
 * @param   go 게임 오버 텍스트 객체
 * @param   restart 재시작 안내 텍스트 객체
 * 
 * 렌더링 순서:
 * 1. 배경 (ClearWindow에서 처리)
 * 2. 냄비에 쌓인 재료 (RenderStack)
 * 3. 활성 재료들 (RenderEntity)
 * 4. 마우스 궤적 (칼날)
 * 5. UI 요소 (점수, 목숨, 게임 오버 메시지)
 */
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

        // 수정 후 : // 순환 버퍼를 역순으로 순회하여 최신 궤적부터 과거 순으로 선을 그린다
        // 이렇게 하면 마우스를 빠르게 움직였을 때도 자연스러운 칼날 효과를 얻을 수 있다
        for (int i = 0; i < TRAIL_LENGTH - 1; i++) {
            // 현재 인덱스 (가장 최신 궤적 점부터 시작)
            int idx1 = (app->trail_head - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            // 다음 인덱스 (그 다음 과거의 궤적 점)
            int idx2 = (app->trail_head - (i + 1) + TRAIL_LENGTH) % TRAIL_LENGTH;

            // 두 점이 모두 유효한 좌표일 때만 선 그리기 (-1은 초기화 되지 않은 좌표를 나타냄)
            if (app->trail_points[idx1].x != -1 && app->trail_points[idx2].x != -1) {
                
                // 점 사이의 거리가 너무 멀면 그리지 않음
                // 마우스를 화면 밖으로 빠르게 이동할 때 이상한 선이 그려지는 것을 방지
                int dist_x = abs(app->trail_points[idx1].x - app->trail_points[idx2].x);
                int dist_y = abs(app->trail_points[idx1].y - app->trail_points[idx2].y);

                // 100픽셀 이내의 거리만 선으로 연결
                if (dist_x < 100 && dist_y < 100) { 
                    SDL_RenderDrawLine(app->g_renderer, 
                        app->trail_points[idx1].x, app->trail_points[idx1].y,
                        app->trail_points[idx2].x, app->trail_points[idx2].y);
                }
            }
        }
        /* 각자 테스트 해보시길...
        // [디버깅용] 냄비 범위 눈으로 확인하기 (빨간 테두리)
        // 위치: DrawGame 함수 맨 끝, 닫는 괄호 } 바로 위에 넣으세요.
        SDL_SetRenderDrawColor(app->g_renderer, 255, 0, 0, 255); // 빨간색
        SDL_Rect debug_rect = { POT_X, POT_Y, POT_W, POT_H };
        SDL_RenderDrawRect(app->g_renderer, &debug_rect);
        */
    }

    // C. 점수판 (Score) 그리기
    RenderScoreBoard(app, score);

    
    // 목숨 하트 그리기 - 점수판 아래에 3개의 하트 표시
    // 남은 목숨 수만큼 빨간 하트, 잃은 목숨만큼 검은 하트로 표시
    for (int i = 1; i <= 3; i++) {
        SDL_Rect dest = {UI_HEARTS_X + (i-1)*UI_HEART_GAP, UI_HEARTS_Y, HEART_SIZE, HEART_SIZE};
        
        // 현재 남은 목숨 수에 따라 빨간 하트 또는 검은 하트 그리기
        if (app->game.lives >= i) {
            // 아직 남은 목숨 : 빨간 하트
            if (heart_red.texture) SDL_RenderCopy(app->g_renderer, heart_red.texture, NULL, &dest);
        } else {
            // 잃은 목숨 : 검은 하트
            if (heart_black.texture) SDL_RenderCopy(app->g_renderer, heart_black.texture, NULL, &dest);
        }
    }

    // D. 게임 오버 화면 처리 - game이 종료된 경우에만 표시
    if (app->game.game_over) {
        // 반투명 검은 막 씌우기
        SDL_SetRenderDrawBlendMode(app->g_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app->g_renderer, 0, 0, 0, UI_ALPHA_OVERLAY);
        SDL_Rect screen_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(app->g_renderer, &screen_rect);
        SDL_SetRenderDrawBlendMode(app->g_renderer, SDL_BLENDMODE_NONE); // 복구

        // 게임 오버 메시지, 최종 점수, 재시작 안내 텍스트를 화면 중앙에 그리기
        RenderScoreBoard(app, go);
        RenderScoreBoard(app, &gameover_score_text);
        RenderScoreBoard(app, restart);
    }
}

// 8. 메인 렌더링 루프
/**
 * @brief   게임의 메인 렌더링 함수 (매 프레임 호출)
 * 
 * 이 함수는 게임 루프에서 매 프레임마다 호출된다.
 * 타이틀 화면과 게임 화면을 구분하여 적절한 UI를 렌더링한다.
 * 점수 텍스처는 점수가 변경될 때만 재생성하여 성능을 최적화한다.
 */
void render_game(void) {
    ClearWindow(&app); //화면 초기화 (배경 이미지 포함)

    // 타이틀 화면 표시 - 게임 시작 전 화면
    if (app.game.title_screen) {
        // 타이틀, 시작 안내, 게임 규칙 텍스트를 화면 중앙에 렌더링
        RenderScoreBoard(&app, &title_text);
        RenderScoreBoard(&app, &subtitle_text);
        RenderScoreBoard(&app, &rule1_text);
        RenderScoreBoard(&app, &rule2_text);
        RenderScoreBoard(&app, &rule3_text);
        ShowWindow(&app);
        return;
    }

    // 성능 최적화: 점수가 바뀌었을 때만 텍스처 재생성
    // 텍스트 텍스처 생성은 비용이 크므로 불필요한 재생성을 방지
    static int last_score = -1;
    if (app.game.score != last_score) {
        char score_str[64];
        sprintf(score_str, "Score: %d", app.game.score);
        TextureSmallText(&app, &score_text, score_str, black);
        // 점수판 위치 설정 (화면 왼쪽 상단)
        score_text.rect.x = UI_SCORE_X;
        score_text.rect.y = UI_SCORE_Y;
        
        last_score = app.game.score; // 현재 점수 기억하여 다음 비교에 사용
    }

    
    // [추가] 게임 오버 시 최종 점수 표시
    if (app.game.game_over) {
        char final_score_str[64];
        sprintf(final_score_str, "Final Score: %d", app.game.score);
        TextureSmallText(&app, &gameover_score_text, final_score_str, yellow);
        // 화면 중앙에 정렬하여 표시
        gameover_score_text.rect.x = (SCREEN_WIDTH - gameover_score_text.rect.w) / 2;
        gameover_score_text.rect.y = SCREEN_HEIGHT / 2 - UI_GAMEOVER_Y_OFFSET;
    }
    
    // 그리기 함수 호출
    // 게임 화면의 모든 요소 그리기 (재료, UI, 마우스 궤적 등)
    DrawGame(&app, &score_text, &gameover_text, &restart_text);

    // 렌더링 완료 후 화면에 표시 (더블 버퍼링)
    ShowWindow(&app);
}

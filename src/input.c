/**
 * @file    input.c
 * @brief   사용자 입력을 처리하는 함수 정의
 * @author  조정배 팀
 * 
 * 이 파일은 키보드와 마우스 입력을 처리한다:
 * - 키보드 입력 (ESC 종료, R 키 재시작/시작)
 * - 마우스 위치 추적 및 궤적 저장
 * - 마우스 클릭 시 베기 충돌 검사
 */
#include "input.h"

/**
 * @brief   SDL 이벤트를 처리하여 사용자 입력을 반영한다.
 * 
 * 이 함수는 게임 루프에서 매 프레임 호출된다.
 * 처리하는 입력:
 * - 윈도우 닫기 버튼 / ESC 키: 게임 종료
 * - R 키: 타이틀 화면에서 게임 시작, 게임 오버 시 재시작
 * - 마우스 이동: 위치 업데이트 및 베기 충돌 검사
 * - 마우스 클릭/해제: 궤적 기록 시작/중지
 */
void handle_events(void) {
    SDL_Event event;
    
    // SDL 이벤트 큐의 모든 이벤트 처리
    while (SDL_PollEvent(&event)) {
        // 윈도우 닫기 버튼 클릭 시 게임 종료
        if (event.type == SDL_QUIT) {
            g_game_running = 0;
        }
        
        // 키보드 입력 처리
        if (event.type == SDL_KEYDOWN) {
            // ESC 키: 게임 종료
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                g_game_running = 0;
            }
            // 타이틀 화면에서 R키로 게임 시작
            if (app.game.title_screen && event.key.keysym.sym == SDLK_r) {
                app.game.title_screen = 0;
            }
            // 게임 오버 시 R키로 재시작
            if (app.game.game_over && event.key.keysym.sym == SDLK_r) {
                ResetGame();
            }
        }

        // 마우스 움직임 처리
        if (event.type == SDL_MOUSEMOTION) {
            // 이전 마우스 위치 저장 (베기 충돌 검사를 위한 선분의 시작점)
            int prev_x = app.mouse_x;
            int prev_y = app.mouse_y;
            
            // 현재 마우스 위치 업데이트
            app.mouse_x = event.motion.x;
            app.mouse_y = event.motion.y;

            // 마우스 버튼을 누르고 있을 때만 궤적 업데이트 및 베기 판정
            // 타이틀 화면에서는 베기 불가
            if (app.mouse_down && !app.game.title_screen) {
                // 순환 버퍼에 새로운 궤적 점 추가
                app.trail_head = (app.trail_head + 1) % TRAIL_LENGTH;
                app.trail_points[app.trail_head].x = app.mouse_x;
                app.trail_points[app.trail_head].y = app.mouse_y;

                // 이전 위치와 현재 위치를 이은 선분으로 재료 베기 충돌 검사
                CheckSlice(app.game.ingredients, MAX_INGREDIENTS, prev_x, prev_y, app.mouse_x, app.mouse_y);
            }
        }


        // 마우스 버튼 클릭 처리 
        if (event.type == SDL_MOUSEBUTTONDOWN){
            app.mouse_down = 1; // 마우스 버튼이 눌림

            // 궤적 버퍼를 현재 마우스 위치로 초기화
            // 버튼을 누르는 순간부터 일관된 궤적이 그려지도록 한다
            for(int i=0; i<TRAIL_LENGTH; i++) {
                app.trail_points[i].x = event.button.x;
                app.trail_points[i].y = event.button.y;
            }
        }
        // 마우스 버튼 해제 처리
        if (event.type == SDL_MOUSEBUTTONUP)
            app.mouse_down = 0; //마우스 버튼이 해제됨
    }
}

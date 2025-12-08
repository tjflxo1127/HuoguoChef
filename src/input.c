#include "input.h"

void handle_events(void) {
    SDL_Event event; //이벤트 변수 선언 (SDL_Event 구조체: 모든 SDL이벤트 (키보드, 마우스, 창 등)를 담음)

    while (SDL_PollEvent(&event)) {  //이벤트 큐에서 이벤트를 하나 꺼내서 event에 저장한다 , 큐에 이벤트가 남아있는 동안 계속 처리 받는다 (반환값 : 이벤트 있으면 1 이벤트 없으면 0) 
        if (event.type == SDL_QUIT) { //프로그램 종료 이벤트 (창닫기 버튼 누름)
            g_game_running = 0; //메인 루프 종료 (g_game_running = 0)
        }

        if (event.type == SDL_KEYDOWN) { //키가 눌렸을 때 발생하는 이벤트 
            if (event.key.keysym.sym == SDLK_ESCAPE) { //프로그램 종료 이벤트 (눌린 키가 esc일때)
                g_game_running = 0; //esc키로 게임 즉시 종료 
            }

            // [추가] 타이틀 화면에서 R키로 게임 시작
            if (app.game.title_screen && event.key.keysym.sym == SDLK_r) { //현재 타이틀 화면이고 R키가 눌렸으면 
                app.game.title_screen = 0; //타이틀 화면 종료 & 게임시작
            }
            
            // [추가] 게임 오버 시 R키로 재시작
            if (app.game.game_over && event.key.keysym.sym == SDLK_r) { //게임 오버 상태이고 R키가 눌렸으면 
                ResetGame(); //점수 초기화, 목숨 리셋, 재료들 제거 하는 함수 실행
            }
         //기타 이벤트는 무시한다   
        }

        // 마우스 움직임 처리
        if (event.type == SDL_MOUSEMOTION) {
            int prev_x = app.mouse_x;
            int prev_y = app.mouse_y;

            app.mouse_x = event.motion.x;
            app.mouse_y = event.motion.y;

            // 마우스 누르고 있을 때만 궤적 업데이트하고 베기 판정 (타이틀 화면에서는 제외)
            if (app.mouse_down && !app.game.title_screen) {
                app.trail_head = (app.trail_head + 1) % TRAIL_LENGTH;
                app.trail_points[app.trail_head].x = app.mouse_x;
                app.trail_points[app.trail_head].y = app.mouse_y;

                CheckSlice(app.game.ingredients, MAX_INGREDIENTS, prev_x, prev_y, app.mouse_x, app.mouse_y);
            }
        }


        // 마우스 버튼 클릭 처리 
        if (event.type == SDL_MOUSEBUTTONDOWN){
            app.mouse_down = 1;

            for(int i=0; i<TRAIL_LENGTH; i++) {
                app.trail_points[i].x = event.button.x;
                app.trail_points[i].y = event.button.y;
            }
        }

        if (event.type == SDL_MOUSEBUTTONUP)
            app.mouse_down = 0;
    }
}

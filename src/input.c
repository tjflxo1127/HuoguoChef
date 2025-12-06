#include "input.h"

void handle_events(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            g_game_running = 0;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                g_game_running = 0;
            }
            // [추가] 게임 오버 시 R키로 재시작
            if (app.game.game_over && event.key.keysym.sym == SDLK_r) {
                ResetGame();
            }
        }

        // 마우스 움직임 처리
        if (event.type == SDL_MOUSEMOTION) {
            int prev_x = app.mouse_x;
            int prev_y = app.mouse_y;

            app.mouse_x = event.motion.x;
            app.mouse_y = event.motion.y;

            // 마우스 누르고 있을 때만 궤적 업데이트하고 베기 판정
            if (app.mouse_down) {
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
#include "input.h"
#include "action.h"

extern App app;
extern int g_game_running;

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

        if (event.type == SDL_MOUSEMOTION) {
            int prev_x = app.mouse_x;
            int prev_y = app.mouse_y;

            app.mouse_x = event.motion.x;
            app.mouse_y = event.motion.y;

            app.trail_head = (app.trail_head + 1) % TRAIL_LENGTH;
            app.trail_points[app.trail_head].x = app.mouse_x;
            app.trail_points[app.trail_head].y = app.mouse_y;

            if (app.mouse_down) {
                CheckSlice(app.game.ingredients, MAX_INGREDIENTS, prev_x, prev_y, app.mouse_x, app.mouse_y);
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN){
            app.mouse_down = 1;
            for(int i=0; i<TRAIL_LENGTH; i++) {
                app.trail_points[i].x = event.button.x;
                app.trail_points[i].y = event.button.y;
            }
        }


        if (event.type == SDL_MOUSEBUTTONUP)   app.mouse_down = 0;
    }
}
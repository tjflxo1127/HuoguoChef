#include <stdio.h>
#include <time.h>

#include "main.h"

int main(int argc, char *argv[]) {
    // 랜덤 시드 설정
    srand((unsigned int)time(NULL));

    // 1. 초기화
    init_sdl();

     // BGM 재생 시작
    PlayBGM();

    // [추가] 게임 상태 초기화
    app.game.score = 0;
    app.game.lives = 3;
    app.game.game_over = 0;

    // 2. 게임 루프
    while (g_game_running) {
        handle_events(); 
        update_game(); 
        render_game(); 
        SDL_Delay(1000 / 60); 
    }

    // 3. 종료
    cleanup_sdl();
    return 0;
}
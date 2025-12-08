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
             //다른 키를 누른 경우 무시한다   
        }

        // 마우스 움직임 처리
        if (event.type == SDL_MOUSEMOTION) { //마우스가 움직일때 매 프레임마다 발생
            int prev_x = app.mouse_x; //현재 좌표 백업 (임시저장) --> 슬라이스 판정용에 사용 (이전위치~현재위치 선분 생성에 사용)
            int prev_y = app.mouse_y;

            app.mouse_x = event.motion.x; //event.motion.x 마우스의 현재 스크린 좌표 
            app.mouse_y = event.motion.y; //마우스 위치를 전역 변수에 저장

            // 마우스 누르고 있을 때만 궤적 업데이트하고 베기 판정 (타이틀 화면에서는 제외)
            if (app.mouse_down && !app.game.title_screen) { //마우스 버튼이 눌려있는 상태이면서 타이틀 화면이 아닐때!
                app.trail_head = (app.trail_head + 1) % TRAIL_LENGTH;  //원형 버퍼 고정 크기: 10개의 좌표만 저장
/*오버플로우 방지: 오래된 데이터 자동 덮어쓰기
메모리 절약: 불필요한 히스토리 삭제*/
                app.trail_points[app.trail_head].x = app.mouse_x;
                app.trail_points[app.trail_head].y = app.mouse_y;
/*// 매 프레임마다 선분 생성
선분: (prev_x, prev_y) → (current_x, current_y)

// 10개의 궤적 점으로 구성된 경로
trail_points[0] → trail_points[1] → ... → trail_points[9]
          ↓             ↓                     ↓
       선분1         선분2                선분9
                CheckSlice(app.game.ingredients, MAX_INGREDIENTS, prev_x, prev_y, app.mouse_x, app.mouse_y);
            }
        }


        // 마우스 버튼 클릭 처리 
        if (event.type == SDL_MOUSEBUTTONDOWN){ 
            app.mouse_down = 1; /*SDL_MOUSEBUTTONDOWN: 마우스 버튼이 눌렸을 때 발생
app.mouse_down = 1: 슬라이스 모드 활성화
궤적 초기화: 모든 trail_points를 현재 마우스 위치로 설정
깔끔한 시작: 슬라이스 시작점을 일치시켜 선이 깨지지 않게 함*/

            for(int i=0; i<TRAIL_LENGTH; i++) {
                app.trail_points[i].x = event.button.x;
                app.trail_points[i].y = event.button.y;
            }
        }

        if (event.type == SDL_MOUSEBUTTONUP)
            app.mouse_down = 0; /*SDL_MOUSEBUTTONUP: 마우스 버튼에서 손을 뗐을 때 발생
app.mouse_down = 0: 슬라이스 모드 비활성화
슬라이스 종료: 마우스를 떼면 베기 기능 중단*/
    }/*
        마우스 버튼 다운
    ↓
app.mouse_down = 1
trail_points[] 초기화 (모두 현재 위치)
    ↓
마우스 움직임 (매 프레임)
    ↓
if (mouse_down && 게임 중)
    ↓
궤적 버퍼에 좌표 추가
슬라이스 판정 실행 (prev → current 선분)
    ↓
마우스 버튼 업
    ↓
app.mouse_down = 0
슬라이스 종료*/
}

/**
 * @file    draw.h
 * @brief   텍스처 렌더링을 위한 함수 선언 
 * @author  조정배 팀
 *
 * 이 파일은 게임의 모든 렌더링 관련 함수들을 선언한다. 
 * - 화면 초기화 및 업데이트
 * - 텍스트 렌더링 (일반 텍스트, 타이틀 텍스트)
 * - 게임 객체 렌더링 (재료, 스택, UI 요소)
 * - 마우스 궤적 시각화
 */
#ifndef DRAW_H
#define DRAW_H 

#include "defs.h"

/**
 * @brief   렌더러를 검은색으로 초기화하고 배경 이미지를 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 이 함수는 매 프레임마다 호출되어 화면을 초기화한다.
 * 배경 이미지가 로드되지 않은 경우 검은 배경을 사용한다.
 */
void ClearWindow(App *app);

/**
 * @brief   렌더링된 내용을 화면에 표시한다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 이 함수는 더블 버퍼링의 백 버퍼에 그려진 모든 내용을
 * 프론트 버퍼로 교체하여 화면에 표시한한다.
 */
void ShowWindow(App *app);


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
// 텍스처 헬퍼 함수
void CreateTextTexture(App *app, TextObject *text_obj, char *str, SDL_Color color, TTF_Font *font);

/**
 * @brief   문자열을 큰 폰트(타이틀용)로 텍스처로 변환한다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   text_obj 텍스트 객체 포인터 (결과가 저장됨)
 * @param   str 렌더링할 문자열
 * @param   color 텍스트 색상
 * 
 * 일반 텍스트보다 2배 큰 폰트를 사용하여 타이틀 화면에 사용된다.
 */
void TextureBigText(App *app, TextObject *text_obj, char *str, SDL_Color color);


/**
 * @brief   개별 재료(식재료 또는 함정)를 화면에 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   ing 그릴 재료 객체 포인터
 * 
 * 재료가 잘린 상태(is_sliced)인 경우 어둡고 반투명하게 렌더링하여
 * 시각적으로 구분할 수 있도록 한다. 
 */
void RenderEntity(App *app, Ingredient *ing);

/**
 * @brief   냄비 안에 쌓인 재료들을 배경으로 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * 
 * 잘려서 냄비 안에 떨어진 재료들은 스택에 저장되며,
 * 배경 레이어로 렌더링된다. 활성 재료보다 아래에 그려져
 * 냄비 안에 쌓인 느낌을 연출한다.
 * 어둡게 처리하여 물에 잠긴 느낌을 준다.
 */
void RenderStack(App *app);

/**
 * @brief   텍스트 객체를 화면에 그린다.
 * @param   app 게임 애플리케이션 구조체 포인터
 * @param   text_obj 그릴 텍스트 객체 포인터
 * 
 * 점수판, 게임 오버 메시지 등 모든 텍스트 UI 요소를
 * 이 함수를 통해 렌더링한다.
 */
void RenderScoreBoard(App *app, TextObject *text_obj);


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
void DrawGame(App *app, TextObject *score, TextObject *go, TextObject *restart);


/**
 * @brief   게임의 메인 렌더링 함수 (매 프레임 호출)
 * 
 * 이 함수는 게임 루프에서 매 프레임마다 호출된다.
 * 타이틀 화면과 게임 화면을 구분하여 적절한 UI를 렌더링한다.
 * 점수 텍스처는 점수가 변경될 때만 재생성하여 성능을 최적화한다.
 */
void render_game(void);

#endif

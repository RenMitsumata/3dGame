/////////////////////////////////////////////////////////////////
//		2018年度 4-2 HEW Project
//		「天使と悪魔」(Window.h)
//		Team Name:ACTIVESSSSSSSSS
//		Author   :Ren Mitsumata
//---------------------------------------------------------------
//　ウィンドウに関する設定を行うクラスの定義
/////////////////////////////////////////////////////////////////

#ifndef WINDOW_H
#define WINDOW_H
/***************************************************************/

/* ---　インクルードファイル　--- */
#include <Windows.h>

/* ---　構造体定義　--- */

/* ---　定数定義　--- */
#define WINDOW_WIDTH (1024)	//	ウィンドウの横幅
#define WINDOW_HEIGHT (576)	//　ウィンドウの縦幅


/*---------------------------------------------------
	クラス定義
---------------------------------------------------*/
class Window{
/*　シングルトンにて定義　*/
private:
	WNDCLASS wc;	
	HWND hWnd;
	MSG msg;
	
public:
	Window();
	~Window();
	void Init(HINSTANCE hInstance, int nCmdShow);
	void Uninit(void);
	MSG GetMsg(void);
	HWND GethWnd(void);
};
/***************************************************************/
#endif // WINDOW_H
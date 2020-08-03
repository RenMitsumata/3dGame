/*------------------------------------------------------------------
　		DirectX11　プログラム(Window.cpp)
		作成日：19/03/03
		作者：Ren Mitsumata
********************************************************************
	Windowsアプリケーションのウィンドウに関するクラスの定義
------------------------------------------------------------------*/


/* ---　マクロ定義　--- */


/* ---　インクルードファイル　--- */
#include "main.h"
#include "Window.h"

/* ---　グローバル変数　--- */

/* ---　静的クラス変数　--- */

/* ---　プロトタイプ宣言　--- */
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*-----------------------------
| 　　　　関数定義　　　　　　|
-----------------------------*/

/* ---　コンストラクタ　--- */
Window::Window(){

}

/* ---　デストラクタ　--- */
Window::~Window(){

}

/* ------------------------------------------------------
　　Init関数
  　　…ウィンドウの初期化及び生成を行う。
	　（戻り値）なし
　　　（引数）HINSTANCE型　インスタンスハンドル
   　　　　　 int型　ウィンドウの表示方法
		 ※引数は両方とも、WinMain関数の引数から用いる
------------------------------------------------------ */
void Window::Init(HINSTANCE hInstance, int nCmdShow) {
	//-------------------------------------------
	//	ウィンドウクラスの定義
	//-------------------------------------------
	ZeroMemory(&wc, sizeof(WNDCLASS));
	// =====以下、最低限決めなければいけない要素===== //
	wc.lpfnWndProc = WndProc;								//	ウィンドウプロシージャ（コールバック関数）
	//wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);			//	メニューバーを登録
	wc.lpszClassName = CLASS_NAME;							//	クラス名
	wc.hInstance = hInstance;								//	インスタンスハンドル
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//	マウスカーソル（必須ではないが、カーソルが変わらなくなる）
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);		//	背景カラー（設定値＋１にすると決まっている）
	RegisterClass(&wc);		//	ウィンドウクラスを登録

	//-------------------------------------------
	//	ウィンドウクラスの生成
	//-------------------------------------------
	int DisplayX = GetSystemMetrics(SM_CXSCREEN);			//	ディスプレイサイズを取得（プライマリディスプレイの幅（高さなら引数をSM_CYSCREEN））
	int DisplayY = GetSystemMetrics(SM_CYSCREEN);
	RECT window_rect = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };	//ウィンドウサイズ設定(RECT型構造体
															//RECT window_rect = { 0,0,DisplayX,DisplayY };	//ウィンドウサイズ設定(RECT型構造体)
	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME), FALSE);
	//AdjustWindowRect(RECT構造体のアドレス,ウィンドウのスタイル,メニュー)
	int ADJUST_WINDOW_WIDTH = window_rect.right - window_rect.left;		//アジャスト後のウィンドウの幅
	int ADJUST_WINDOW_HEIGHT = window_rect.bottom - window_rect.top;	//アジャスト後のウィンドウの高さ


	int WindowBeginX = DisplayX / 2;	//　ディスプレイの中心座標を求める
	int WindowBeginY = DisplayY / 2;
	WindowBeginX -= (ADJUST_WINDOW_WIDTH / 2);
	WindowBeginY -= (ADJUST_WINDOW_HEIGHT / 2);

	WindowBeginX = max(WindowBeginX, 0);
	WindowBeginY = max(WindowBeginY, 0);

	hWnd = CreateWindow(									//戻り値：HWND型（ウィンドウハンドル）　※ウィンドウ作成に失敗すると０が入る
		CLASS_NAME,											//クラス名
		WINDOW_CAPTION,										//キャプション
		WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME),	//ウィンドウスタイルフラグ（ビット）
		WindowBeginX,										//ウィンドウ座標(x)
		WindowBeginY,										//ウィンドウ座標(y)
		ADJUST_WINDOW_WIDTH,											//ウィンドウの幅
		ADJUST_WINDOW_HEIGHT,											//ウィンドウの高さ
		NULL,												//親ウィンドウのハンドル
		NULL,												//メニュー
		hInstance,											//インスタンスハンドル
		NULL												//ウィンドウメッセージで送れるオリジナルデータなどのポインタ
	);

	ShowWindow(hWnd, nCmdShow);	//ウィンドウを表示するための関数　引数：(ウィンドウハンドル,表示方法（基本的にnCmdShow）)
	UpdateWindow(hWnd);
	ZeroMemory(&msg, sizeof(MSG));
}

/* ------------------------------------------------------
　　Uninit関数
  　　…インスタンスを削除する。
	　（戻り値）なし
　　　（引数）なし
------------------------------------------------------ */
void Window::Uninit(void){
	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wc.hInstance);
}

////////////////////////////////////////
//	以下、各種ゲッター(public)
////////////////////////////////////////

MSG Window::GetMsg(void) {
	return msg;
}

HWND Window::GethWnd(void) {
	return hWnd;
}

////////////////////////////////////////
//	コールバック関数（ウィンドウプロシージャ）
////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//引数：（ウィンドウハンドル,DispatchMessageで送られた値,プロシージャに送られたメッセージに付随した値×２）
	//DefWindowProcを使えば問題ないが、自分が決めた操作をしたい場合は、自作関数で操作する（フック：操作権を奪う）
	switch (uMsg) {
	case WM_KEYDOWN:
	//　キーが押されたとき
		if (wParam == VK_ESCAPE) {
		// Escキーならば
		// WM_CLOSEをメッセージとして送る
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_CLOSE:
	//　WM_CLOSEのメッセージを受信したら
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
		//　メッセージボックスで終了が選択されたら
		//　プログラムを終了する
			DestroyWindow(hWnd);	
		}
		return 0;
	case WM_DESTROY:
	//　プログラムが終了したら
	//　OSに終了メッセージを送信する
		PostQuitMessage(0);
		return 0;
	};

	return DefWindowProc(hWnd, uMsg, wParam, lParam);	//値を与えれば基本的な動作を自動的に行う

}


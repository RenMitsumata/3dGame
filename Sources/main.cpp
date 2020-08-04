/*===========================================
	DirectX11 ゲームプログラム
	Author: Ren Mitsumata
*********************************************

===========================================*/

#include <windows.h>
#include "main.h"
#include "Manager.h"
#define SCREEN_WIDTH (1024)
#define SCREEN_HEIGHT (576)

#pragma comment (lib,"winmm.lib")


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// マネージャの初期化　※一番最初に行うこと！
	Manager* manager;
	manager = Manager::Get();
	manager->Init(hInstance, nCmdShow);

	//フレームカウント初期化
	DWORD dwExecLastTime;
	DWORD dwCurrentTime;
	timeBeginPeriod(1);
	dwExecLastTime = timeGetTime();
	dwCurrentTime = 0;



	// メッセージループ
	MSG msg;
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				manager->Update();

				// 描画処理
				manager->Draw();
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	
	// 終了処理
	manager->Uninit();

	return (int)msg.wParam;
}

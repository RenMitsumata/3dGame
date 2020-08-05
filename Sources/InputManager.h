
/*----------------------------
　XInput&Windows入力マネージャ
 （１人用を想定）
----------------------------*/

#pragma once



// キーボード用
#include <Windows.h>
#pragma comment(lib,"winmm.lib")
// コントローラ用
#include <XInput.h>
#pragma comment(lib,"xinput.lib")

class InputManager
{
// キーボード用
private:
	BYTE m_OldKeyState[256];
	BYTE m_KeyState[256];
public:
	InputManager();
	~InputManager();
	void Init();
	void Uninit();
	void Update();

	bool GetKeyPress(BYTE KeyCode);
	bool GetKeyTrigger(BYTE KeyCode);
// コントローラ用
private:
	bool isConnected = false;		// 接続されているかの確認
	XINPUT_STATE lastCS;			// 1F前の入力状態を保持(トリガー、リリース用)
	XINPUT_STATE controllerState;	// 現フレームでの入力状態
public:
	bool GetPadTrigger(WORD id);
	bool GetPadPress(WORD id);
	bool GetPadRelease(WORD id);
	bool GetLeftStickOperate();
	bool GetRightStickOperate();
	float GetLeftStickRad();	// 左スティックの真上方向を０とした、角度を返す。
	float GetRightStickX();		// 最大値との割合で値を返す(-1～1)
	float GetRightStickY();		// 最大値との割合で値を返す(-1～1)
	bool GetPadLTTrigger();		// ゲームパッドの左トリガーボタンが押された瞬間だけtrueを返す
	bool GetPadLTPress();		// ゲームパッドの左トリガーボタンが押されている時trueを返す
	bool GetPadLTRelease();		// ゲームパッドの左トリガーボタンが離された瞬間だけtrueを返す
	bool GetPadRTTrigger();		// ゲームパッドの右トリガーボタンが押された瞬間だけtrueを返す
	bool GetPadRTPress();		// ゲームパッドの右トリガーボタンが押されている時trueを返す
	bool GetPadRTRelease();		// ゲームパッドの右トリガーボタンが離された瞬間だけtrueを返す
	bool GetHat();
};


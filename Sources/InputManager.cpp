#include "InputManager.h"
#include "DXManager.h"

// ゲームパッドのトリガーボタンをtrueかfalseで返すときに判定に使う値(0～255のうち、この値以上なら押していると判定するため)
#define GAMEPAD_LEFTTRIGGER_DEADLINE (85)
#define GAMEPAD_RIGHTTRIGGER_DEADLINE (85)

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::Init()
{
	memset(m_OldKeyState, 0, 256);
	memset(m_KeyState, 0, 256);
	// コントローラの接続確認
	long isSuccess = XInputGetState(0, &controllerState);
	

	if (isSuccess != ERROR_SUCCESS) {
		MessageBox(nullptr, "コントローラが接続されていません。", "確認", MB_ICONASTERISK | MB_OK);
	}
	else {
		isConnected = true;
		lastCS = controllerState;
	}
}

void InputManager::Uninit()
{
}

void InputManager::Update()
{
	
	memcpy(m_OldKeyState, m_KeyState, 256);

	GetKeyboardState(m_KeyState);
	if (isConnected) {
		lastCS = controllerState;
		XInputGetState(0, &controllerState);
		// 切断時の処理も入れる？
		
	}
}

bool InputManager::GetKeyPress(BYTE KeyCode)
{
	return (m_KeyState[KeyCode] & 0x80);
}

bool InputManager::GetKeyTrigger(BYTE KeyCode)
{
	return ((m_KeyState[KeyCode] & 0x80) && !(m_OldKeyState[KeyCode] & 0x80));
}

bool InputManager::GetPadTrigger(WORD id)
{
	if (!isConnected) {
		return false;
	}
	return !(lastCS.Gamepad.wButtons & id) && (controllerState.Gamepad.wButtons & id);
}

bool InputManager::GetPadPress(WORD id)
{
	if (!isConnected) {
		return false;
	}
	return controllerState.Gamepad.wButtons & id;
}

bool InputManager::GetPadRelease(WORD id)
{
	if (!isConnected) {
		return false;
	}
	return (lastCS.Gamepad.wButtons & id) && !(controllerState.Gamepad.wButtons & id);
}

bool InputManager::GetLeftStickOperate()
{
	if (!isConnected) {
		return false;
	}
	return !((controllerState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			controllerState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(controllerState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			controllerState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

bool InputManager::GetRightStickOperate()
{
	if (!isConnected) {
		return false;
	}
	return !((controllerState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		controllerState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(controllerState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			controllerState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
}

float InputManager::GetLeftStickRad()
{
	SHORT lx = controllerState.Gamepad.sThumbLX;
	SHORT ly = controllerState.Gamepad.sThumbLY;
	float radian = atan2f(ly, -lx) - XMConvertToRadians(90.0f);	// 真上方向を０とするため
	return radian;
}

float InputManager::GetRightStickX()
{
	if (!isConnected) {
		return 0.0f;
	}
	SHORT rx = controllerState.Gamepad.sThumbRX;
	return rx / 32768.0f;
}

float InputManager::GetRightStickY()
{
	if (!isConnected) {
		return 0.0f;
	}
	SHORT ry = controllerState.Gamepad.sThumbRY;
	return ry / 32768.0f;
}

bool InputManager::GetPadLTTrigger()
{
	BYTE value = controllerState.Gamepad.bLeftTrigger;
	BYTE lastValue = lastCS.Gamepad.bLeftTrigger;
	bool isPush = value > GAMEPAD_LEFTTRIGGER_DEADLINE;
	bool isLastPush = lastValue > GAMEPAD_LEFTTRIGGER_DEADLINE;
	return isPush & (isPush ^ isLastPush);
}

bool InputManager::GetPadLTPress()
{
	BYTE value = controllerState.Gamepad.bLeftTrigger;
	return (value > GAMEPAD_LEFTTRIGGER_DEADLINE);
}

bool InputManager::GetPadLTRelease()
{
	BYTE value = controllerState.Gamepad.bLeftTrigger;
	BYTE lastValue = lastCS.Gamepad.bLeftTrigger;
	bool isPush = value > GAMEPAD_LEFTTRIGGER_DEADLINE;
	bool isLastPush = lastValue > GAMEPAD_LEFTTRIGGER_DEADLINE;
	return (!isPush) & (isPush ^ isLastPush);
}

bool InputManager::GetPadRTTrigger()
{
	BYTE value = controllerState.Gamepad.bRightTrigger;
	BYTE lastValue = lastCS.Gamepad.bRightTrigger;
	bool isPush = value > GAMEPAD_RIGHTTRIGGER_DEADLINE;
	bool isLastPush = lastValue > GAMEPAD_RIGHTTRIGGER_DEADLINE;
	return isPush & (isPush ^ isLastPush);
}

bool InputManager::GetPadRTPress()
{
	BYTE value = controllerState.Gamepad.bRightTrigger;
	return (value > GAMEPAD_RIGHTTRIGGER_DEADLINE);
}

bool InputManager::GetPadRTRelease()
{
	BYTE value = controllerState.Gamepad.bRightTrigger;
	BYTE lastValue = lastCS.Gamepad.bRightTrigger;
	bool isPush = value > GAMEPAD_RIGHTTRIGGER_DEADLINE;
	bool isLastPush = lastValue > GAMEPAD_RIGHTTRIGGER_DEADLINE;
	return (!isPush) & (isPush ^ isLastPush);
}

bool InputManager::GetHat()
{
	
	return false;
}

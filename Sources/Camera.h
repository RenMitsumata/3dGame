#pragma once

#include "window.h"
#include "DXManager.h"
#include "Scene.h"
#include "Manager.h"
#include "GameObject.h"

#define DEFAULT_NEAR (0.1f)
#define DEFAULT_FAR (100.0f)


class Camera : public GameObject
{

	
private:
	InputManager* input;
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projMat;
	ID3D11DeviceContext* context;
	GameObject* owner;	// 追尾用。定点カメラならnullptr
	XMFLOAT3 offset;	// ownerとのオフセット座標
	XMFLOAT3 upDst = { 0.0f,1.0f,0.0f };
	XMFLOAT3 at = { 0.0f,2.5f,0.0f };
	XMVECTOR up = XMLoadFloat3(&upDst);
	XMFLOAT3 deltaPos;
	XMFLOAT3 deltaRot;
	float focusLength;
	float nearDis = DEFAULT_NEAR;
	float farDis = DEFAULT_FAR;
	bool isMain = false;
	float playerDepth = 1.0f;
public:
	Camera();
	~Camera();
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void SetOwner(GameObject* obj) { owner = obj; }
	void AddDeltaRot(XMFLOAT3 rot);
	void SetDeltaPos(XMFLOAT3 pos) { deltaPos = pos; }
	void SetDeltaRot(XMFLOAT3 rot) { deltaRot = rot; }
	XMFLOAT2 GetNearAndFar() { return XMFLOAT2(nearDis, farDis); }
	bool GetMain() { return isMain; }
	float GetPlayerDepth() { return playerDepth; }
};


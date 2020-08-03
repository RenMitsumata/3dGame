#pragma once
#include "DXManager.h"
#include "ComponentFactory.h"
#include "Component.h"
#include <list>
#include "Scene.h"



class GameObject
{
protected:
	// メンバのpos,rot,sizはローカル座標
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 siz;

	XMFLOAT3 front;
	XMFLOAT3 up;
	XMFLOAT3 velocity;

	bool isDestroy = false;

	GameObject*				parent;			// 親GameObjectクラス（１つ）
	std::list<GameObject*>	childrensList;	// 子GameObjectクラス（複数）
	std::list<Component*>	componentsList;	// コンポーネント（複数）

	// 四分木関連
	struct DIVISION* division;	// 四分木の分割データ（ワールドの広さと分割数）へのアドレスポインタ
	float judgeWidth;	// 当たり判定のＸ軸方向の幅
	float judgeDepth;	// 当たり判定のＺ軸方向の幅
	DWORD divisionNum;	// 自分はどの区画に所属するか
	void UpdateBlock();	// 区画更新関数
public:
	GameObject();
	virtual ~GameObject();
	virtual void Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	XMFLOAT4X4 GetTransformMatrix();
	XMFLOAT3 GetPos() { return pos; }
	XMFLOAT3 GetRot() { return rot; }
	XMFLOAT3 GetSiz() { return siz; }
	DWORD GetDivisionNum() { return divisionNum; }
	void SetPos(XMFLOAT3 pos) { this->pos = pos; }
	void SetRot(XMFLOAT3 rot) 
	{
		this->rot = rot;
		
		XMMATRIX mat = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMFLOAT3 stdDir = { 0.0f,0.0f,1.0f };
		XMVECTOR stdDirVec = XMLoadFloat3(&stdDir);
		XMVECTOR frontVec = XMVector3TransformNormal(stdDirVec, mat);
		frontVec = XMVector3Normalize(frontVec);
		XMStoreFloat3(&front, frontVec);
	}
	void SetSiz(XMFLOAT3 siz) { this->siz = siz; }
	void AddPos(XMFLOAT3 pos) { this->pos += pos; }
	void AddRot(XMFLOAT3 rot)
	{ 
		this->rot += rot; 
		XMMATRIX mat = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMVECTOR frontVec = XMLoadFloat3(&front);
		frontVec = XMVector3TransformNormal(frontVec, mat);
		frontVec = XMVector3Normalize(frontVec);
		XMStoreFloat3(&front, frontVec);
	}
	void AddSiz(XMFLOAT3 siz) { this->siz += siz; }
	XMFLOAT3 GetFront() { return front; }
	XMFLOAT3 GetUp() { return up; }
	XMFLOAT3 GetVelocity() { return velocity; }
	void SetVelocity(XMFLOAT3 velocity) { this->velocity = velocity; }
	void AddVelocity(XMFLOAT3 velocity) { this->velocity += velocity; }
	bool isJudge(WORD otherNum);
	virtual void ColAction(Component* comp) {};
	void SetDestroy() { isDestroy = true; }
	bool IsDestroy() { return isDestroy; }
};


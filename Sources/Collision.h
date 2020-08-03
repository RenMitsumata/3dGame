#pragma once
#include "GameObject.h"
#include "Component.h"

// 前方宣言
class CollisionSphere;
class CollisionOBB;
class CollisionCapsule;
class CollisionCone;

enum Collision_Tag {
	e_COLTYPE_DEFAULT = 0,
	e_COLTYPE_MAPOBJECT,
	e_COLTYPE_PLAYER,
	e_COLTYPE_ENEMY,
	e_COLTYPE_ENEMY_EYESIGHT,
	e_COLTYPE_BULLET,
	e_COLTYPE_SOUND,
	e_COLTYPE_GOAL
};

enum Collision_Flag {
	e_COLFLAG_DEFAULT = 0x00,
	e_COLFLAG_IMPASSBLE = 0x01	// 通過不可オブジェクト(壁など)
};

class Collision : public Component
{
	
protected:
	Collision_Tag colTag = e_COLTYPE_DEFAULT;
	XMFLOAT3 lastPos;	// 通り抜け防止のため、１Ｆ前の座標を記録。OwnerのUpdate内、移動処理前にSetLastPosを呼ぶこと。
	XMFLOAT3 deltaPos = {0.0f,0.0f,0.0f};	// ownerからの位置差分
	BYTE colFlag = e_COLFLAG_DEFAULT;
public:
	Collision();
	virtual ~Collision();
	virtual void SetTag(Collision_Tag tag) { colTag = tag; }
	virtual Collision_Tag GetTag() { return colTag; }
	virtual void SetFlag(BYTE colFlag) { this->colFlag = colFlag; }
	virtual BYTE GetFlag() { return colFlag; }
	virtual void SetLastPos(XMFLOAT3 pos) { lastPos = pos; }
	virtual XMFLOAT3 GetLastPos() { return lastPos; }
	virtual XMFLOAT3 GetPos() { return owner->GetPos() + deltaPos; }
	virtual void SetDeltaPos(XMFLOAT3 delta) { deltaPos = delta; }
	virtual void Dispatch(Collision* other) = 0;
	virtual void CollisionAction(Collision* other) = 0;
	// collisionの派生クラス分、純粋仮想関数を用意
	virtual bool isCollision(CollisionSphere* other) = 0;
	virtual bool isCollision(CollisionOBB* other) = 0;
	virtual bool isCollision(CollisionCapsule* other) = 0;
	virtual bool isCollision(CollisionCone* other) = 0;
};


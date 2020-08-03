#pragma once
#include "Collision.h"
#include "Shader3D.h"

class Manager;

class CollisionOBB : public Collision
{
private:
	float frontLength;
	float rightLength;
	float upLength;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Manager* manager;
public:
	CollisionOBB(float f = 1.0f,float r = 1.0f,float u = 1.0f, XMFLOAT3 pos = XMFLOAT3(0.0f,0.0f,0.0f));
	~CollisionOBB();
	void Dispatch(Collision* other)override;
	void CollisionAction(Collision* other)override;
	bool isCollision(CollisionSphere* other)override;
	bool isCollision(CollisionOBB* other)override;
	bool isCollision(CollisionCapsule* other)override;
	bool isCollision(CollisionCone* other)override;
	XMFLOAT3 GetLengths() { return XMFLOAT3(frontLength, rightLength, upLength); }
	void SetParams(float f = 1.0f, float r = 1.0f, float u = 1.0f, XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void Draw()override;
	void SetOwner(GameObject* owner)override;
};


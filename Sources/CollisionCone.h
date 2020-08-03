#pragma once
#include "Collision.h"
class CollisionCone : public Collision
{
private:
	float minRadius;	// ç≈è¨îºåa
	float maxRadius;	// ç≈ëÂîºåa
	float length;		// â~êçÇÃí∑Ç≥
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Manager* manager;
public:
	CollisionCone();
	~CollisionCone();
	void Dispatch(Collision* other)override;
	void CollisionAction(Collision* other)override;
	bool isCollision(CollisionSphere* other)override;
	bool isCollision(CollisionOBB* other)override;
	bool isCollision(CollisionCapsule* other)override;
	bool isCollision(CollisionCone* other)override;
	XMFLOAT3 GetParams();
	void SetParams(float m = 0.0f, float M = 1.0f, float l = 1.0f);
	void Draw()override;
	void SetOwner(GameObject* owner)override;
};


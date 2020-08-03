#pragma once
#include "Collision.h"
class CollisionCapsule : public Collision
{
private:
	float height;	// ìõÇÃçÇÇ≥
	float radius;	// ìõãyÇ—îºãÖÇÃîºåa
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Manager* manager;
public:
	CollisionCapsule();
	~CollisionCapsule();
	void Dispatch(Collision* other)override;
	void CollisionAction(Collision* other)override;
	bool isCollision(CollisionSphere* other)override;
	bool isCollision(CollisionOBB* other)override;
	bool isCollision(CollisionCapsule* other)override;
	bool isCollision(CollisionCone* other)override;
	XMFLOAT2 GetParams();
	void SetParams(float h = 1.0f, float r = 0.5f);
	void Draw()override;
	void SetOwner(GameObject* owner)override;
};


#pragma once
#include "Collision.h"

class Shader3D;
class Manager;

class CollisionSphere :	public Collision
{
private:
	float radius;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Manager* manager;
public:
	CollisionSphere();
	~CollisionSphere();
	bool isCollision(CollisionSphere* other);
	bool isCollision(CollisionOBB* other);
	bool isCollision(CollisionCapsule* other);
	bool isCollision(CollisionCone* other);
	void Dispatch(Collision* other)override;
	void CollisionAction(Collision* other)override;
	float GetRadius() { return radius; }
	void SetRadius(float r) { radius = r; }
	void Draw()override;
	void SetOwner(GameObject* owner)override;
};


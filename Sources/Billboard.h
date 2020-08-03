#pragma once
#include "Component.h"
#include "Shader2D.h"

class Manager;
class Texture;

class Billboard : public Component
{
private:
	Manager* manager;
	DXManager* dxManager;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;
	Shader2D* shader;
	Texture* texture;
	XMFLOAT3 deltaPos;
public:
	Billboard();
	~Billboard();
	void Init()override;
	void Draw()override;
	void SetTexture(const char* filename);
	void SetDeltaPos(XMFLOAT3 pos) { deltaPos = pos; }
};


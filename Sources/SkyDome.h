#pragma once
#include "Component.h"
#include "Shader3D.h"

class Texture;

class SkyDome :	public Component
{
private:
	float radius;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Texture* texture;
	Manager* manager;
	LIGHT light = { XMFLOAT4(0.0f,1.0f,0.0f,0.0f),COLOR(1.0f,1.0f,1.0f,1.0f),COLOR(1.0f,1.0f,1.0f,1.0f) };
public:
	SkyDome();
	~SkyDome();
	void Set(float r);
	void Draw()override;
};


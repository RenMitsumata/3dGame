#pragma once
#include "Component.h"
#include "UI.h"
#include "Shader2D_PostEffect.h"
class Texture;


class DeferredPolygon :	public Component
{
private:
	DXManager* dxManager;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	Shader2D_PostEffect* shader;
public:
	DeferredPolygon();
	virtual ~DeferredPolygon();
	void Init()override;
	void Draw()override;
	void SetDeferredTexture(ID3D11ShaderResourceView* srv);
	void SetSize(float width, float height, XMFLOAT2 screenPos);
	void SetSize(float width, float height, XMFLOAT3 screenPos);
};


#pragma once
#include "GameObject.h"
#include "window.h"

class DeferredPolygon;

class DeferredObject : public GameObject
{
private:
	DeferredPolygon* polygon = nullptr;
public:
	DeferredObject();
	~DeferredObject();
	void Init()override;
	void SetTextureAndSize(const char* filename, XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f), float width = WINDOW_WIDTH, float height = WINDOW_HEIGHT);
	void SetDeferredTexture(ID3D11ShaderResourceView* srv);
};


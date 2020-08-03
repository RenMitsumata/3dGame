#pragma once
#include "GameObject.h"
#include "window.h"


struct VERTEX_UI {
	XMFLOAT2 pos;
	XMFLOAT2 texcoord;
};

class Texture;

class UI : public GameObject
{
private:
	class Polygon* polygon;
public:
	UI();
	~UI();
	void Init()override;
	void SetTextureAndSize(const char* filename, XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f), float width = WINDOW_WIDTH, float height = WINDOW_HEIGHT);
	void ChangeColor();
	void SetDeferredTexture(ID3D11ShaderResourceView* srv);
};


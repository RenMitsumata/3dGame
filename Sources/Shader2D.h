#pragma once

struct VERTEX_2D {
	XMFLOAT2 pos;
	XMFLOAT2 texcoord;
};

struct CONSTANT_UI {
	XMMATRIX projMat;
	float depth = 1.0f;
};

class Texture;

class Shader2D
{
private:
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	ID3D11VertexShader*     vertexShader;
	ID3D11PixelShader*      pixelShader;
	ID3D11InputLayout*      vertexLayout;
	ID3D11SamplerState*		samplerState = NULL;
	ID3D11Buffer*			constantBuffer;
	CONSTANT_UI				constantValue;
	XMFLOAT4				color = {1.0f,1.0f,1.0f,1.0f};
	ID3D11Buffer*			colorBuffer;
	unsigned int			time = 0;
public:
	Shader2D();
	~Shader2D();
	void Init(const char* VS_Filename = "VS_UI.cso", const char* PS_Filename = "PS_UI.cso");
	void Uninit();
	void Set();
	void SetTexture(Texture* texture);
	void SetTexture(ID3D11ShaderResourceView* srv);
	void SetProjMatrix(XMMATRIX mat);
	void SetDepth(float depth);
	void ChangeColor();
};


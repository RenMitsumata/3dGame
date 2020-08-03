#pragma once


struct CONSTANT_POSTEFFECT {
	XMMATRIX proj;
	XMFLOAT3 cameraPos;
};

class Shader2D_PostEffect
{
private:
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	ID3D11VertexShader*     vertexShader;
	ID3D11PixelShader*      pixelShader;
	ID3D11InputLayout*      vertexLayout;
	ID3D11SamplerState*		samplerState;
	ID3D11Buffer*			constantBuffer;
	CONSTANT_POSTEFFECT		constantValue;
	XMFLOAT4				color = { 1.0f,1.0f,1.0f,1.0f };
	ID3D11Buffer*			colorBuffer;
	unsigned int			time = 0;
public:
	Shader2D_PostEffect();
	~Shader2D_PostEffect();
	void Init(const char* VS_Filename = "VS_PostEffect.cso", const char* PS_Filename = "PS_PostEffect.cso");
	void Uninit();
	void Set();
	void SetTexture(ID3D11ShaderResourceView* srv);
	void SetProjMatrix(XMMATRIX mat);
};


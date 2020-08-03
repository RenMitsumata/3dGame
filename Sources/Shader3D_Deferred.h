#pragma once

struct CONSTANTS_DEFERRED {
	XMMATRIX worldMtx;
	XMMATRIX viewMtx;
	XMMATRIX projMtx;
	XMFLOAT2 nearAndFar;
};

class Shader3D_Deferred
{
private:
	Manager* manager;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* vertexLayout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* samplerState;
	CONSTANTS_DEFERRED constantsValue;
	ID3D11Buffer* playerDepthBuffer;
	
public:
	Shader3D_Deferred();
	~Shader3D_Deferred();
	void Init(const char* VS_Filename = "vertexShader3D.cso", const char* PS_Filename = "pixelShader3D.cso");
	void Uninit();
	void Set();
	void SetWorldMatrix(XMFLOAT4X4* worldMtx);
	void SetViewMatrix(XMFLOAT4X4* viewMtx);
	void SetProjMatrix(XMFLOAT4X4* projMtx);

};


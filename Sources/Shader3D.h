#pragma once

///////////////////////////////////////////////////////////
//　Shader3Dクラス
//　…外部的要素を適用せず、最も容易に描画する
//　（＝他の３Ｄシェーダーの基底クラス）
///////////////////////////////////////////////////////////
//
//　　定数バッファ情報
//			VS				PS
//--0	　WVP行列		マテリアル情報
//
///////////////////////////////////////////////////////////



// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;	// ローカル座標
	XMFLOAT3 Normal;	// 法線
	XMFLOAT4 Diffuse;	// 頂点カラー
	XMFLOAT2 TexCoord;	// UV座標
};



// 色構造体
struct COLOR
{
	COLOR() {}
	COLOR(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	float r;
	float g;
	float b;
	float a;

	COLOR operator = (XMFLOAT4 color) {
		this->r = color.x;
		this->g = color.y;
		this->b = color.z;
		this->a = color.w;
		return*this;
	}



};



// マテリアル構造体
struct MATERIAL
{
	COLOR		Ambient;
	COLOR		Diffuse;
	COLOR		Specular;
	COLOR		Emission;
	float		Shininess;
	float		Dummy[3];//16bit境界用
};


// シェーダ定数バッファ構造体
struct SHADER_CONSTANTS {
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projMatrix;
	//XMFLOAT2 nearAndFar;
};




class Texture;
class Manager;

class Shader3D
{
private:
	Manager*				manager;
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;
	ID3D11VertexShader*     vertexShader;
	ID3D11PixelShader*      pixelShader;
	ID3D11InputLayout*      vertexLayout;
	ID3D11SamplerState*		samplerState = NULL;

	ID3D11Buffer*			constantBuffer;
	SHADER_CONSTANTS		constantValue;

	/*
	ID3D11Buffer*			worldBuffer;
	ID3D11Buffer*			viewBuffer;
	ID3D11Buffer*			projectionBuffer;
	*/
	ID3D11Buffer*			materialBuffer;
	/*
	ID3D11Buffer*			lightBuffer;
	ID3D11Buffer*			cameraPosBuffer;
	ID3D11Buffer*			playerDepthBuffer;
	*/
	
public:
	Shader3D();
	~Shader3D();
	void Init(const char* VS_Filename = "VS_Shader3D.cso", const char* PS_Filename = "PS_Shader3D.cso");
	void Uninit();

	// シェーダ定数バッファ設定
	void Set();
	void SetWorldMatrix(XMFLOAT4X4* worldMatrix);
	void SetViewMatrix(XMFLOAT4X4* viewMatrix);
	void SetProjMatrix(XMFLOAT4X4* projMatrix);
	void SetMaterial(MATERIAL* material);
	void SetTexture(Texture* texture);
};


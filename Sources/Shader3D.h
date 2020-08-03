#pragma once



// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
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



/*
// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL		Material;
	class CTexture*	Texture;
};
*/


/*
// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};
*/

struct LIGHT
{
	XMFLOAT4	Direction;
	COLOR		Diffuse;
	COLOR		Ambient;
};

// シェーダ定数バッファ構造体
struct SHADER_CONSTANTS {
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projMatrix;
	XMFLOAT2 nearAndFar;
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
	ID3D11SamplerState* samplerState = NULL;

	ID3D11Buffer*			constantBuffer;
	SHADER_CONSTANTS		constantValue;

	/*
	ID3D11Buffer*			worldBuffer;
	ID3D11Buffer*			viewBuffer;
	ID3D11Buffer*			projectionBuffer;
	*/
	ID3D11Buffer*			materialBuffer;
	ID3D11Buffer*			lightBuffer;
	ID3D11Buffer*			cameraPosBuffer;
	ID3D11Buffer*			playerDepthBuffer;

	
public:
	Shader3D();
	~Shader3D();
	void Init(const char* VS_Filename = "VS_Deferred.cso", const char* PS_Filename = "PS_Deferred.cso");
	void Uninit();

	// シェーダ定数バッファ設定
	void Set();
	void SetWorldMatrix(XMFLOAT4X4* worldMatrix);
	void SetViewMatrix(XMFLOAT4X4* viewMatrix);
	void SetProjMatrix(XMFLOAT4X4* projMatrix);
	void SetMaterial(MATERIAL* material);
	void SetLight(LIGHT* light);
	void SetTexture(Texture* texture);
};


#pragma once

///////////////////////////////////////////////////////////
//�@Shader3D�N���X
//�@�c�O���I�v�f��K�p�����A�ł��e�Ղɕ`�悷��
//�@�i�����̂R�c�V�F�[�_�[�̊��N���X�j
///////////////////////////////////////////////////////////
//
//�@�@�萔�o�b�t�@���
//			VS				PS
//--0	�@WVP�s��		�}�e���A�����
//
///////////////////////////////////////////////////////////



// ���_�\����
struct VERTEX_3D
{
	XMFLOAT3 Position;	// ���[�J�����W
	XMFLOAT3 Normal;	// �@��
	XMFLOAT4 Diffuse;	// ���_�J���[
	XMFLOAT2 TexCoord;	// UV���W
};



// �F�\����
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



// �}�e���A���\����
struct MATERIAL
{
	COLOR		Ambient;
	COLOR		Diffuse;
	COLOR		Specular;
	COLOR		Emission;
	float		Shininess;
	float		Dummy[3];//16bit���E�p
};


// �V�F�[�_�萔�o�b�t�@�\����
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

	// �V�F�[�_�萔�o�b�t�@�ݒ�
	void Set();
	void SetWorldMatrix(XMFLOAT4X4* worldMatrix);
	void SetViewMatrix(XMFLOAT4X4* viewMatrix);
	void SetProjMatrix(XMFLOAT4X4* projMatrix);
	void SetMaterial(MATERIAL* material);
	void SetTexture(Texture* texture);
};


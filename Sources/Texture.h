#pragma once



class Texture :	public Component
{
private:
	ID3D11Texture2D* textureSrc;
	ID3D11ShaderResourceView*	shaderResourceView;
public:
	Texture();
	~Texture();
	void Load(const char* filename);
	void UnLoad();
	void LoadTextureFromMemory(const unsigned char * pData, int len);
	ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView; }
};


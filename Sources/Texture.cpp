#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

#include "DXManager.h"
#include "Manager.h"
#include "Component.h"
#include "Texture.h"



Texture::Texture()
{
}


Texture::~Texture()
{
}

void Texture::Load(const char * filename)
{
	unsigned char	header[18];
	unsigned char	*image;
	int	width, height;
	unsigned char	depth;
	int	bpp;
	unsigned int	size;

	image = stbi_load(filename, &width, &height, &bpp, 0);

	if (image == nullptr) {
		assert(false);
	}

	size = width * height * bpp;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = image;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = size;

	ID3D11Device* device = Manager::Get()->GetDXManager()->GetDevice();

	if (!device) {
		assert(false);
	}

	auto hr = device->CreateTexture2D(&desc, &initData, &textureSrc);
	if (FAILED(hr)) {
		assert(false);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(textureSrc, &SRVDesc, &shaderResourceView);
	if (FAILED(hr))
	{
		assert(false);
	}

	stbi_image_free(image);
}

void Texture::UnLoad() {
	shaderResourceView->Release();
	textureSrc->Release();	
}


// FBX内にデータとして埋め込まれている画像を読み込む関数
void Texture::LoadTextureFromMemory(const unsigned char* pData, int len) {

	// 第１引数：メモリ上の画像の先頭アドレス
	// 第２引数：画像のメモリサイズ
	unsigned char* pImage;

	int width;
	int height;
	int bpp;
	pImage = stbi_load_from_memory(pData, len, &width, &height, &bpp, STBI_rgb_alpha);
	unsigned int size = width * height * bpp;


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pImage;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = size;


	ID3D11Device* device = Manager::Get()->GetDXManager()->GetDevice();

	if (!device) {
		assert(false);
	}

	auto hr = device->CreateTexture2D(&desc, &initData, &textureSrc);
	if (FAILED(hr)) {
		assert(false);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(textureSrc, &SRVDesc, &shaderResourceView);
	if (FAILED(hr))
	{
		assert(false);
	}

	stbi_image_free(pImage);

}
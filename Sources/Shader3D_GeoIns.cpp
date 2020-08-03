#include "Manager.h"
#include "DXManager.h"
#include "Component.h"
#include "Texture.h"
#include "Shader3D_GeoIns.h"
#include <Windows.h>
#include <stdio.h>
#include <io.h>


Shader3D_GeoIns::Shader3D_GeoIns()
{
}


Shader3D_GeoIns::~Shader3D_GeoIns()
{
}

void Shader3D_GeoIns::Init(const char * VS_Filename, const char * PS_Filename)
{
	// 初期化に必要なデバイス取得
	device = Manager::Get()->GetDXManager()->GetDevice();
	context = Manager::Get()->GetDXManager()->GetDeviceContext();

	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "DirectXDeviceの取得に失敗しました", "シェーダ初期化失敗", MB_ICONHAND);
		exit(-1);
	}



	// 頂点シェーダ生成
	{
		FILE* file;
		long int fsize;

		file = fopen(VS_Filename, "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);


		device->CreateVertexShader(buffer, fsize, NULL, &vertexShader);



		// 入力レイアウト生成
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0		, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"		, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 4 * 3	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"		, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, 4 * 6	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT		, 0, 4 * 10	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WORLDMTX_L1" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 1, 4 * 0	, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WORLDMTX_L2" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 1, 4 * 4	, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WORLDMTX_L3" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 1, 4 * 8	, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WORLDMTX_L4" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 1, 4 * 12	, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};
		UINT numElements = ARRAYSIZE(layout);

		device->CreateInputLayout(layout,
			numElements,
			buffer,
			fsize,
			&vertexLayout);

		delete[] buffer;
	}



	// ピクセルシェーダ生成
	{
		FILE* file;
		long int fsize;

		file = fopen(PS_Filename, "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreatePixelShader(buffer, fsize, NULL, &pixelShader);

		delete[] buffer;
	}




	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(SHADER_CONSTANTS);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	device->CreateBuffer(&hBufferDesc, NULL, &constantBuffer);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	/*
	device->CreateBuffer(&hBufferDesc, NULL, &worldBuffer);
	context->VSSetConstantBuffers(0, 1, &worldBuffer);

	device->CreateBuffer(&hBufferDesc, NULL, &viewBuffer);
	context->VSSetConstantBuffers(1, 1, &viewBuffer);

	device->CreateBuffer(&hBufferDesc, NULL, &projectionBuffer);
	context->VSSetConstantBuffers(2, 1, &projectionBuffer);
	*/

	hBufferDesc.ByteWidth = sizeof(MATERIAL);

	device->CreateBuffer(&hBufferDesc, NULL, &materialBuffer);
	context->VSSetConstantBuffers(1, 1, &materialBuffer);
	context->PSSetConstantBuffers(0, 1, &materialBuffer);

	hBufferDesc.ByteWidth = sizeof(LIGHT);

	device->CreateBuffer(&hBufferDesc, NULL, &lightBuffer);
	context->VSSetConstantBuffers(2, 1, &lightBuffer);





	// 入力レイアウト設定
	context->IASetInputLayout(vertexLayout);

	// シェーダ設定
	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);



	// ライト初期化
	LIGHT light;
	light.Direction = XMFLOAT4(0.0f, -0.7f, 0.7f, 0.0f);
	light.Diffuse = COLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Ambient = COLOR(0.1f, 0.1f, 0.1f, 1.0f);
	SetLight(&light);


	// マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(&material);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


	device->CreateSamplerState(&samplerDesc, &samplerState);

	context->PSSetSamplers(0, 1, &samplerState);

}

void Shader3D_GeoIns::Uninit()
{
	if (lightBuffer) { lightBuffer->Release(); }
	if (materialBuffer) { materialBuffer->Release(); }
	if (constantBuffer) { constantBuffer->Release(); }
	if (vertexLayout) { vertexLayout->Release(); }
	if (pixelShader) { pixelShader->Release(); }
	if (vertexShader) { vertexShader->Release(); }

}


void Shader3D_GeoIns::Set()
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	// シェーダ設定
	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);


	// 入力レイアウト設定
	context->IASetInputLayout(vertexLayout);


	// 定数バッファ更新
	context->UpdateSubresource(constantBuffer, 0, NULL, &constantValue, 0, 0);

	// 定数バッファ設定(定数バッファの形が異なる場合、引数を変える)
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	context->VSSetConstantBuffers(1, 1, &materialBuffer);
	context->PSSetConstantBuffers(0, 1, &materialBuffer);
	//context->PSSetConstantBuffers(0, 1, &constantBuffer);
}

void Shader3D_GeoIns::SetWorldMatrix(XMFLOAT4X4* worldMatrix)
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	XMMATRIX matrix;
	matrix = XMLoadFloat4x4(worldMatrix);
	matrix = XMMatrixTranspose(matrix);
	constantValue.worldMatrix = matrix;
}

void Shader3D_GeoIns::SetViewMatrix(XMFLOAT4X4* viewMatrix)
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	XMMATRIX matrix;
	matrix = XMLoadFloat4x4(viewMatrix);
	matrix = XMMatrixTranspose(matrix);
	constantValue.viewMatrix = matrix;
}

void Shader3D_GeoIns::SetProjMatrix(XMFLOAT4X4* projMatrix)
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	XMMATRIX matrix;
	matrix = XMLoadFloat4x4(projMatrix);
	matrix = XMMatrixTranspose(matrix);
	constantValue.projMatrix = matrix;
}

void Shader3D_GeoIns::SetMaterial(MATERIAL* material)
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	context->UpdateSubresource(materialBuffer, 0, NULL, material, 0, 0);
}

void Shader3D_GeoIns::SetLight(LIGHT* light)
{
	if (device == nullptr || context == nullptr) {
		MessageBox(NULL, "シェーダが初期化されていません", "デバイス取得失敗", MB_ICONHAND);
		exit(-1);
	}
	context->UpdateSubresource(lightBuffer, 0, NULL, light, 0, 0);
}

void Shader3D_GeoIns::SetTexture(Texture* texture)
{
	ID3D11ShaderResourceView* srv[1] = { texture->GetShaderResourceView() };
	context->PSSetShaderResources(0, 1, srv);
}

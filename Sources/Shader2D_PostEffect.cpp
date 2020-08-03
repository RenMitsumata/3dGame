#include "Manager.h"
#include "DXManager.h"
#include "Shader2D_PostEffect.h"
#include "window.h"
#include <Windows.h>
#include <stdio.h>
#include <io.h>


Shader2D_PostEffect::Shader2D_PostEffect()
{
}


Shader2D_PostEffect::~Shader2D_PostEffect()
{
}

void Shader2D_PostEffect::Init(const char * VS_Filename, const char * PS_Filename)
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
	hBufferDesc.ByteWidth = sizeof(CONSTANT_POSTEFFECT);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	device->CreateBuffer(&hBufferDesc, NULL, &constantBuffer);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);

	device->CreateBuffer(&hBufferDesc, NULL, &colorBuffer);
	context->UpdateSubresource(colorBuffer, 0, NULL, &color, 0, 0);
	context->PSSetConstantBuffers(0, 1, &colorBuffer);




	// プロジェクション行列初期化
	constantValue.proj = XMMatrixOrthographicOffCenterLH(0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 1.0f);
	constantValue.proj = XMMatrixTranspose(constantValue.proj);
	context->UpdateSubresource(constantBuffer, 0, NULL, &constantValue, 0, 0);


	// 入力レイアウト設定
	context->IASetInputLayout(vertexLayout);

	// シェーダ設定
	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);



	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


	device->CreateSamplerState(&samplerDesc, &samplerState);

	context->PSSetSamplers(0, 1, &samplerState);

}

void Shader2D_PostEffect::Uninit()
{
	if (constantBuffer) { constantBuffer->Release(); }
	if (vertexLayout) { vertexLayout->Release(); }
	if (pixelShader) { pixelShader->Release(); }
	if (vertexShader) { vertexShader->Release(); }
}

void Shader2D_PostEffect::Set()
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

	// プロジェクション行列初期化（ウィンドウいっぱい、後から変えない設定）
	context->UpdateSubresource(constantBuffer, 0, NULL, &constantValue, 0, 0);
	context->PSSetSamplers(0, 1, &samplerState);
	context->PSSetConstantBuffers(0, 1, &colorBuffer);


	// 定数バッファ設定(定数バッファの形が異なる場合、引数を変える)
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
}


void Shader2D_PostEffect::SetTexture(ID3D11ShaderResourceView * srv)
{
	ID3D11ShaderResourceView* defSrv[1] = { srv };
	context->PSSetShaderResources(0, 1, defSrv);
}

void Shader2D_PostEffect::SetProjMatrix(XMMATRIX mat)
{
	constantValue.proj = XMMatrixTranspose(mat);
}


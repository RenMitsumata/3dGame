#include "Manager.h"
#include "DXManager.h"
#include "SkyDome.h"
#include "GameObject.h"
#include "Texture.h"


SkyDome::SkyDome()
{
	shader = new Shader3D;
	shader->Init();
	manager = Manager::Get();
	device = manager->GetDXManager()->GetDevice();
	context = manager->GetDXManager()->GetDeviceContext();
	texture = new Texture;
	texture->Load("Assets/Textures/t_sky001.png");
}


SkyDome::~SkyDome()
{
	delete texture;
}

void SkyDome::Set(float r)
{
	// 頂点情報の作製
	VERTEX_3D vertex[19 * 37];
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 37; j++) {
			float posX = r * sinf(XMConvertToRadians(i * 10)) * cosf(XMConvertToRadians(j * 10));
			float posY = r * cosf(XMConvertToRadians(i * 10));
			float posZ = r * sinf(XMConvertToRadians(i * 10)) * sinf(XMConvertToRadians(j * 10));
			vertex[i * 37 + j].Position = { posX,posY,posZ };			
			vertex[i * 37 + j].TexCoord = { (float)(36 - j) / 36,(float)i / 18 };		
			vertex[i * 37 + j].Diffuse = { 1.0f,1.0f,1.0f,1.0f };
			vertex[i * 37 + j].Normal = { 0.0f,-1.0f,0.0f };
		}
	}


	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * 19 * 37;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vertex;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vbData, &vertexBuffer);


	// インデックス情報の登録
	unsigned short indexList[1478];
	unsigned short Cnt = 0;
	
	for (int i = 0; i < 37; i++) {
		for (int j = 0; j < 19; j++) {
			if (i == 0) {
				indexList[Cnt] = j * 37 + i % 37;
				indexList[Cnt + 1] = (j + 1) * 37 + (i - 1) % 37;
			}
			else {
				indexList[Cnt] = j * 37 + i % 37;
				indexList[Cnt + 1] = j * 37 + (i - 1) % 37;
			}
			Cnt += 2;
		}
		if (i != 36) {
			indexList[Cnt] = indexList[Cnt - 1];
			indexList[Cnt + 1] = i + 1;
			Cnt += 2;
		}
	}



	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * 1478;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = indexList;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufferDesc, &ibData, &indexBuffer);
}

void SkyDome::Draw()
{
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	XMFLOAT4X4 MatLoc = owner->GetTransformMatrix();
	XMMATRIX worldMat = XMLoadFloat4x4(&MatLoc);
	worldMat = XMMatrixScaling(100.0f, 100.0f, 100.0f) * worldMat;
	XMStoreFloat4x4(&MatLoc, worldMat);
	shader->SetWorldMatrix(&MatLoc);
	shader->SetTexture(texture);
	shader->SetViewMatrix(manager->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(manager->GetScene()->GetProjectionMatrix());
	shader->SetLight(&light);

	shader->Set();
	context->DrawIndexed(1478, 0, 0);
}

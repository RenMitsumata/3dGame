#include "UI.h"
#include "Polygon.h"

#include "Manager.h"
#include "DXManager.h"

#include "Texture.h"
#include "window.h"


Polygon::Polygon()
{
	
}


Polygon::~Polygon()
{
	if (indexBuffer) {indexBuffer->Release();}
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (shader) {
		shader->Uninit();
		delete shader;
	}
}

void Polygon::Init()
{
	device = Manager::Get()->GetDXManager()->GetDevice();
	context = Manager::Get()->GetDXManager()->GetDeviceContext();
	dxManager = Manager::Get()->GetDXManager();	

	shader = new Shader2D;
	shader->Init();
}

void Polygon::Draw()
{
	
	UINT stride = sizeof(VERTEX_UI);
	UINT offset = 0;

	

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//dxManager->SetDepthTexture(0);
	if (texture != nullptr) {
		shader->SetTexture(texture);
	}
	//shader->SetProjMatrix(projMat);
	shader->Set();

	context->DrawIndexed(6, 0, 0);
}

void Polygon::SetTexture(const char* filename)
{
	texture = ComponentFactory::CreateComponent<Texture>();
	texture->Load(filename);
}

void Polygon::SetDeferredTexture(ID3D11ShaderResourceView* srv)
{
	shader->SetTexture(srv);
}

void Polygon::SetSize(float width, float height, XMFLOAT2 screenPos)
{
	VERTEX_UI vertex[4];
	for (int i = 0; i < 4; i++) {
		vertex[i].pos = XMFLOAT2((i % 2) * width + screenPos.x, (i / 2) * height + screenPos.y);
		vertex[i].texcoord = XMFLOAT2(i % 2, i / 2);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_UI) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//　頂点バッファ
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;
	device->CreateBuffer(&bd, &sd, &vertexBuffer);


	unsigned short indexSource[6] = { 0,1,2,1,2,3 };
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * 6;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = indexSource;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufferDesc, &ibData, &indexBuffer);
}

void Polygon::SetSize(float width, float height, XMFLOAT3 screenPos)
{
	XMFLOAT2 scrPos = { screenPos.x,screenPos.y };
	SetSize(width, height, scrPos);
	shader->SetDepth(screenPos.z);
}

void Polygon::ChangeColor()
{
	shader->ChangeColor();
}

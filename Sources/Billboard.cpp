#include "UI.h"
#include "Billboard.h"
#include "Manager.h"
#include "DXManager.h"
#include "Texture.h"
#include "window.h"


Billboard::Billboard()
{
}


Billboard::~Billboard()
{
}

void Billboard::Init()
{
	device = Manager::Get()->GetDXManager()->GetDevice();
	context = Manager::Get()->GetDXManager()->GetDeviceContext();
	manager = Manager::Get();
	dxManager = Manager::Get()->GetDXManager();
	shader = new Shader2D;
	shader->Init();

	VERTEX_UI vertex[4];
	for (int i = 0; i < 4; i++) {
		vertex[i].pos = XMFLOAT2((i % 2) * 0.3f, (i / 2) * 0.3f);
		vertex[i].texcoord = XMFLOAT2(i % 2, -i / 2);
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

void Billboard::Draw()
{
	if (!vertexBuffer || !indexBuffer) {
		return;
	}

	UINT stride = sizeof(VERTEX_UI);
	UINT offset = 0;



	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (texture) {
		shader->SetTexture(texture);
	}

	XMFLOAT3 curPos = owner->GetPos() + deltaPos;
	// rotは無視、sizはたぶん必要ないので未実装


	XMMATRIX mat = XMMatrixIdentity();



	// カメラの方向に向かせるための回転行列を計算する
	XMFLOAT3 cameraPos = *manager->GetScene()->GetCameraPos();
	XMFLOAT3 toEyeW = cameraPos - curPos;
	XMVECTOR toEyeVec = XMLoadFloat3(&toEyeW);
	toEyeVec = XMVector3Normalize(toEyeVec);
	XMFLOAT3 defaultVec = { 0.0f,0.0f,1.0f };
	XMVECTOR defVec = XMLoadFloat3(&defaultVec);
	float dotRes;
	XMStoreFloat(&dotRes, XMVector3Dot(toEyeVec, defVec));
	float degtoEye = acos(dotRes);
	XMFLOAT3 worldUp = { 0.0f,1.0f,0.0f };
	XMVECTOR worldUpVec = XMLoadFloat3(&worldUp);
	XMVECTOR rotateVec = XMQuaternionRotationAxis(worldUpVec, degtoEye);
	XMMATRIX rotateMtx = XMMatrixIdentity();
	rotateMtx = XMMatrixRotationQuaternion(rotateVec);

	//mat *= rotateMtx;
	mat *= XMMatrixTranslation(curPos.x, curPos.y, curPos.z);
	
	

	XMMATRIX view = XMLoadFloat4x4(manager->GetScene()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(manager->GetScene()->GetProjectionMatrix());
	XMMATRIX invView;
	XMFLOAT4X4 viewMat;
	XMStoreFloat4x4(&viewMat, view);
	viewMat._41 = 0.0f;
	viewMat._42 = 0.0f;
	viewMat._43 = 0.0f;
	invView = XMLoadFloat4x4(&viewMat);
	invView = XMMatrixTranspose(invView);


	mat = invView * mat;


	mat *= view;
	mat *= proj;



	


	shader->SetProjMatrix(mat);
	shader->Set();

	context->DrawIndexed(6, 0, 0);
}

void Billboard::SetTexture(const char * filename)
{
	texture = ComponentFactory::CreateComponent<Texture>();
	texture->Load(filename);
}


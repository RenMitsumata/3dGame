#include "Manager.h"
#include "DXManager.h"
#include "Shader3D.h"
#include "CollisionCone.h"
#include "CollisionCapsule.h"


CollisionCone::CollisionCone()
{
	Manager::Get()->GetScene()->GetCollisionManager()->AddCollision(this);
	shader = new Shader3D;
	shader->Init("VS_WireFrame.cso", "PS_WireFrame.cso");
	manager = Manager::Get();
	device = manager->GetDXManager()->GetDevice();
	context = manager->GetDXManager()->GetDeviceContext();
	
}


CollisionCone::~CollisionCone()
{
	shader->Uninit();
	delete shader;
}

void CollisionCone::Dispatch(Collision* other)
{
	bool judge = other->isCollision(this);
	if (judge) {
		CollisionAction(other);
	}
}

void CollisionCone::CollisionAction(Collision * other)
{
	//other->GetOwner()->SetDestroy();
	if (colTag == e_COLTYPE_ENEMY_EYESIGHT) {
		owner->ColAction(other);
	}
}

bool CollisionCone::isCollision(CollisionSphere * other)
{
	return false;
}

bool CollisionCone::isCollision(CollisionOBB * other)
{
	return false;
}

bool CollisionCone::isCollision(CollisionCapsule* other)
{
	// おかしい！！

	// カプセルとの最接近頂点を求める
	XMFLOAT3 front = GetOwner()->GetFront();
	XMVECTOR frontVec = XMLoadFloat3(&front);
	frontVec = XMVector3Normalize(frontVec);
	XMStoreFloat3(&front, frontVec);

	XMFLOAT3 otherUp = GetOwner()->GetUp();
	XMVECTOR otherUpVec = XMVector3Normalize(XMLoadFloat3(&otherUp));
	XMStoreFloat3(&otherUp, otherUpVec);


	XMFLOAT3 distance = GetPos() - other->GetPos();
	XMVECTOR distanceVec = XMLoadFloat3(&distance);

	XMFLOAT3 pointOther;
	XMFLOAT3 point;
	XMFLOAT3 nearest;
	XMVECTOR nearestVec;

	float nearestDis;
	float d1, d2, dv;
	float boxNearDis;
	float length = this->length;
	float otherMinRadius = this->minRadius;
	float otherMaxRadius = this->maxRadius;
	float height = other->GetParams().x;
	float radius = other->GetParams().y;

	bool skip = false;

	XMStoreFloat(&d1, XMVector3Dot(otherUpVec, distanceVec));
	XMStoreFloat(&d2, XMVector3Dot(frontVec, distanceVec));
	XMStoreFloat(&dv, XMVector3Dot(otherUpVec, frontVec));
	float s = (d1 * dv - d2) / (dv * dv - 1);
	float t = (d1 - d2 * dv) / (dv * dv - 1);

	if (fabs(t) > height + radius) {
		skip = true;
	}
	if (s > length + radius || s < radius) {
		skip = true;
	}

	if (!skip) {
		pointOther = GetPos() + t * otherUp;
		point = other->GetPos() + s * front;
		nearest = pointOther - point;
		nearestVec = XMLoadFloat3(&nearest);
		XMStoreFloat(&nearestDis, XMVector3Length(nearestVec));
		float coneRadius = otherMaxRadius;
		float capsuleRadius = radius;

		if (fabs(t) > height) {
			float per = (t - height) / radius;
			capsuleRadius *= cosf(XMConvertToRadians(90 * per));
		}
		if (s < 0) {
			coneRadius = otherMinRadius;
		}
		else if (0 < s && s < length) {
			coneRadius = (s / length) * (otherMaxRadius - otherMinRadius) + otherMinRadius;
		}
		else {
			coneRadius = otherMaxRadius;
		}

		if (nearestDis < (coneRadius + capsuleRadius)) {
			return true;
		}
	}

	return false;

}

bool CollisionCone::isCollision(CollisionCone * other)
{
	return false;
}

XMFLOAT3 CollisionCone::GetParams()
{
	return XMFLOAT3(minRadius,maxRadius,length);
}

void CollisionCone::SetParams(float m, float M,float l)
{
	minRadius = m;
	maxRadius = M;
	length = l;
}

void CollisionCone::Draw()
{
	if (!manager->GetDebug()) {
		return;
	}
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	XMFLOAT4X4 MatLoc = owner->GetTransformMatrix();
	XMMATRIX worldMat = XMLoadFloat4x4(&MatLoc);
	worldMat = XMMatrixScaling(100.0f, 100.0f, 100.0f) * worldMat;
	worldMat = XMMatrixTranslation(deltaPos.x, deltaPos.y, deltaPos.z) * worldMat;
	XMStoreFloat4x4(&MatLoc, worldMat);
	shader->SetWorldMatrix(&MatLoc);
	shader->SetViewMatrix(manager->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(manager->GetScene()->GetProjectionMatrix());

	shader->Set();
	context->DrawIndexed(216, 0, 0);
}

void CollisionCone::SetOwner(GameObject * owner)
{
	this->owner = owner;

	VERTEX_3D vertex[72];
	for (int i = 0; i < 36; i++) {
		vertex[i].Position = XMFLOAT3(cosf(XMConvertToRadians(10 * i))*minRadius, sinf(XMConvertToRadians(10 * i))*minRadius, 0.0f);
		vertex[i].Diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		vertex[i].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[i].TexCoord = XMFLOAT2(0.0f, 0.0f);
	}
	for (int i = 0; i < 36; i++) {
		vertex[i+36].Position = XMFLOAT3(cosf(XMConvertToRadians(10 * i))*maxRadius, sinf(XMConvertToRadians(10 * i))*maxRadius, -length);
		vertex[i+36].Diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		vertex[i+36].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[i+36].TexCoord = XMFLOAT2(0.0f, 0.0f);
	}

	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * 72;
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
	unsigned short indexList[216];
	unsigned int Cnt = 0;
	for (int i = 0; i < 36; i++) {
		indexList[Cnt] = i % 36;
		indexList[Cnt + 1] = (i + 1) % 36;
		Cnt += 2;
	}
	for (int i = 0; i < 36; i++) {
		indexList[Cnt] = i % 36 + 36;
		indexList[Cnt + 1] = (i + 1) % 36 + 36;
		Cnt += 2;
	}
	for (int i = 0; i < 36; i++) {
		indexList[Cnt] = i % 36;
		indexList[Cnt + 1] = i % 36 + 36;
		Cnt += 2;
	}



	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * 216;
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

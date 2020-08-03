#include "WayNodeManager.h"
#include "Enemy.h"
#include "EnemyState.h"
#include "EnemyStateAlert.h"
#include "DXManager.h"
#include "GameObject.h"
#include "CollisionSphere.h"
#include "CollisionOBB.h"
#include "CollisionCapsule.h"
#include "Manager.h"
#include "ImpactSound.h"
#include "EnemyManager.h"
#include "Result.h"


CollisionSphere::CollisionSphere() : radius(1.0f)
{
	Manager::Get()->GetScene()->GetCollisionManager()->AddCollision(this);
	shader = new Shader3D;
	shader->Init("VS_WireFrame.cso", "PS_WireFrame.cso");
	manager = Manager::Get();
	device = manager->GetDXManager()->GetDevice();
	context = manager->GetDXManager()->GetDeviceContext();
}


CollisionSphere::~CollisionSphere()
{
	if (indexBuffer) {
		indexBuffer->Release();
	}
	if (vertexBuffer) {
		vertexBuffer->Release();
	}
	if (shader) {
		shader->Uninit();
	}
	manager->GetScene()->GetCollisionManager()->Delete(this);
}

bool CollisionSphere::isCollision(CollisionSphere* other)
{
	GameObject* object = other->GetOwner();
	XMFLOAT3 otherPos = object->GetPos();
	float distance = (otherPos.x - owner->GetPos().x)*(otherPos.x - owner->GetPos().x) + (otherPos.y - owner->GetPos().y)*(otherPos.y - owner->GetPos().y) + (otherPos.z - owner->GetPos().z)*(otherPos.z - owner->GetPos().z);
	return distance < (other->GetRadius() * other->GetRadius()) + (this->radius * this->radius);
}

bool CollisionSphere::isCollision(CollisionOBB* other)
{
	XMFLOAT3 otherPos = other->GetOwner()->GetPos();



	XMFLOAT3 result = { 0.0f, 0.0f, 0.0f };   // 最終的に長さを求めるベクトル

	XMFLOAT3 distance = GetOwner()->GetPos() - other->GetOwner()->GetPos();
	XMVECTOR distanceVec = XMLoadFloat3(&distance);
	XMFLOAT3 front = other->GetOwner()->GetFront();
	XMFLOAT3 up = other->GetOwner()->GetUp();
	XMVECTOR frontDir = XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR upDir = XMVector3Normalize(XMLoadFloat3(&up));
	XMVECTOR rightDir = XMVector3Normalize(XMVector3Cross(upDir, frontDir));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightDir);
	XMFLOAT3 lengths = other->GetLengths();
	float frontLength = lengths.x;
	float upLength = lengths.y;
	float rightLength = lengths.z;

	// front方向の計算
	XMVECTOR lengthVec = XMVector3Length(XMVector3Dot(frontDir, distanceVec)) / frontLength;
	float scalarF;
	XMStoreFloat(&scalarF, lengthVec);
	scalarF = fabs(scalarF);
	if (scalarF > 1) {
		XMFLOAT3 delta = (1 - scalarF) * frontLength * front;
		result += delta;
	}

	// up方向の計算
	lengthVec = XMVector3Length(XMVector3Dot(upDir, distanceVec)) / upLength;
	XMStoreFloat(&scalarF, lengthVec);
	scalarF = fabs(scalarF);
	if (scalarF > 1) {
		XMFLOAT3 delta = (1 - scalarF) * upLength * up;
		result += delta;
	}

	// right方向の計算
	lengthVec = XMVector3Length(XMVector3Dot(rightDir, distanceVec)) / rightLength;
	XMStoreFloat(&scalarF, lengthVec);
	scalarF = fabs(scalarF);
	if (scalarF > 1) {
		XMFLOAT3 delta = (1 - scalarF) * rightLength * right;
		result += delta;
	}

	// result(はみ出し部分の長さ)を計る
	XMVECTOR resultLengthVec = XMVector3Length(XMLoadFloat3(&result));
	float resultLength;
	XMStoreFloat(&resultLength, resultLengthVec);

	return resultLength < radius;
}

bool CollisionSphere::isCollision(CollisionCapsule * other)
{
	XMFLOAT3 up = other->GetOwner()->GetUp();
	XMFLOAT2 params = other->GetParams();
	float otherHeight = params.x;
	float otherRadius = params.y;
	XMFLOAT3 otherPos = other->GetOwner()->GetPos();
	XMFLOAT3 distance = owner->GetPos() - otherPos;
	XMVECTOR distanceVec = XMLoadFloat3(&distance);
	XMVECTOR upVec = XMVector3Normalize(XMLoadFloat3(&up));

	XMVECTOR lengthVec;
	float length;

	lengthVec = XMVector3Dot(upVec, distanceVec);
	XMStoreFloat(&length, lengthVec);

	if (fabsf(length) < otherHeight) {
		// 筒との当たり判定
		float ref;
		XMVECTOR refVec = XMVector3Length(distanceVec);
		XMStoreFloat(&ref, refVec);
		return (otherRadius + radius) > ref;
	}
	else{
		float ref;
		XMVECTOR refVec;
		if (length > 0) {
			// up軸正方向の球と当たり判定
			otherPos = otherPos + otherHeight * up;
			distance = owner->GetPos() - otherPos;
			distanceVec = XMLoadFloat3(&distance);
			refVec = XMVector3Length(distanceVec);
			XMStoreFloat(&ref, refVec);
			return (otherRadius + radius) > ref;
		}
		else {
			// up軸負方向の球と当たり判定
			otherPos = otherPos - otherHeight * up;
			distance = owner->GetPos() - otherPos;
			distanceVec = XMLoadFloat3(&distance);
			refVec = XMVector3Length(distanceVec);
			XMStoreFloat(&ref, refVec);
			return (otherRadius + radius) > ref;
		}
	}



}

bool CollisionSphere::isCollision(CollisionCone * other)
{
	return false;
}

void CollisionSphere::Dispatch(Collision* other)
{
	bool judge = other->isCollision(this);
	if (judge) {
		CollisionAction(other);
	}
}

void CollisionSphere::CollisionAction(Collision* other)
{
	int a = 0;

	//other->GetOwner()->SetDestroy();
	if (colTag == e_COLTYPE_BULLET && other->GetTag() != e_COLTYPE_ENEMY) {
		owner->SetDestroy();
		Manager::Get()->GetScene()->AddGameObject<ImpactSound>(e_LAYER_UI);
	}
	else if (colTag == e_COLTYPE_SOUND && other->GetTag() == e_COLTYPE_ENEMY) {
		// 敵の聴覚範囲だったら？にする
		EnemyManager* eMgr = Manager::Get()->GetEnemyManager();
		WayNodeManager* nMgr = Manager::Get()->GetScene()->GetNodeManager();
		
		eMgr->ChangePhase(e_ENEMYPHASE_CAUTION);

		XMFLOAT3 enemyPos = other->GetOwner()->GetPos();
		XMFLOAT3 soundPos = GetOwner()->GetPos();

		// 経路探索
		std::vector<unsigned short> nodeList = nMgr->FindAStar(soundPos, enemyPos);
		Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
		EnemyStateAlert* eState = dynamic_cast<EnemyStateAlert*>(enemy->GetState());
		const int nodeListnum = nodeList.size();
		unsigned short* aNode = new unsigned short[nodeListnum];
		for (int i = 0; i < nodeListnum;i++) {
			aNode[i] = nodeList[i];
		}

		eState->SetNode(aNode,nodeListnum);

		delete[] aNode;

		owner->SetDestroy();
	}
	
	if (colTag == e_COLTYPE_GOAL) {
		// リザルト画面へ
		Scene* culScene = Manager::Get()->GetScene();
		culScene->Uninit();
		Manager::Get()->ChangeScene(new Result);
	}
}

void CollisionSphere::Draw()
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
	XMStoreFloat4x4(&MatLoc, worldMat);
	shader->SetWorldMatrix(&MatLoc);
	shader->SetViewMatrix(manager->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(manager->GetScene()->GetProjectionMatrix());

	shader->Set();
	context->DrawIndexed(264, 0, 0);
}

void CollisionSphere::SetOwner(GameObject* owner)
{
	this->owner = owner;
	// 頂点情報の作製
	VERTEX_3D vertex[62];
	vertex[0].Position = XMFLOAT3(0.0f, radius, 0.0f);
	for (int i = 1; i < 6; i++) {
		for (int j = 0; j < 12; j++) {
			vertex[(i - 1) * 12 + j + 1].Position = 
				XMFLOAT3(
					radius * cosf(XMConvertToRadians(i * 30 + 90)) * cosf(XMConvertToRadians(j * 30)),	// X
					radius * sinf(XMConvertToRadians(i * 30 + 90)),										// Y
					radius * cosf(XMConvertToRadians(i * 30 + 90)) * sinf(XMConvertToRadians(j * 30))	// Z
				);
		}
	}
	vertex[61].Position = XMFLOAT3(0.0f, -radius, 0.0f);
	for (int i = 0; i < 62; i++) {
		vertex[i].Diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		vertex[i].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[i].TexCoord = XMFLOAT2(0.0f, 0.0f);
	}
	   	 	

	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * 62;
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
	unsigned short indexList[264];
	unsigned short Cnt = 0;
	for (int i = 0; i < 12; i++, Cnt += 12) {
		indexList[Cnt] = 0;
		indexList[Cnt + 1] = indexList[Cnt + 2] = 1 + i;
		indexList[Cnt + 3] = indexList[Cnt + 4] = 13 + i;
		indexList[Cnt + 5] = indexList[Cnt + 6] = 25 + i;
		indexList[Cnt + 7] = indexList[Cnt + 8] = 37 + i;
		indexList[Cnt + 9] = indexList[Cnt + 10] = 49 + i;
		indexList[Cnt + 11] = 61;
	}
	for (int i = 0; i < 5; i++, Cnt += 24) {
		indexList[Cnt] = indexList[Cnt + 23] = 12 * i + 1;
		indexList[Cnt + 1] = indexList[Cnt + 2] = 12 * i + 2;
		indexList[Cnt + 3] = indexList[Cnt + 4] = 12 * i + 3;
		indexList[Cnt + 5] = indexList[Cnt + 6] = 12 * i + 4;
		indexList[Cnt + 7] = indexList[Cnt + 8] = 12 * i + 5;
		indexList[Cnt + 9] = indexList[Cnt + 10] = 12 * i + 6;
		indexList[Cnt + 11] = indexList[Cnt + 12] = 12 * i + 7;
		indexList[Cnt + 13] = indexList[Cnt + 14] = 12 * i + 8;
		indexList[Cnt + 15] = indexList[Cnt + 16] = 12 * i + 9;
		indexList[Cnt + 17] = indexList[Cnt + 18] = 12 * i + 10;
		indexList[Cnt + 19] = indexList[Cnt + 20] = 12 * i + 11;
		indexList[Cnt + 21] = indexList[Cnt + 22] = 12 * i + 12;
	}

	   	  
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * 264;
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

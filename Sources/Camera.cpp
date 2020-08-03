#define NOMINMAX
#include "Camera.h"


//#include "Model.h"

#include "InputManager.h"
#include "SkyDome.h"

using namespace DirectX;

Camera::Camera() :owner(nullptr)
{
}


Camera::~Camera()
{
}

void Camera::Init()
{
	
	context = Manager::Get()->GetDXManager()->GetDeviceContext();
	pos = XMFLOAT3(0.0f,2.5f, -2.5f);
	offset = XMFLOAT3(1.0f, 0.5f, -1.5f);
	focusLength = 5;
	input = Manager::Get()->GetInput();
	front = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//front = XMFLOAT3(0.0f, -1.0f, 0.0f);
	XMFLOAT3 upDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	up = XMLoadFloat3(&upDir);
	XMVECTOR bufv = XMLoadFloat3(&front);
	bufv = XMVector3Normalize(bufv);
	XMStoreFloat3(&front, bufv);
	Manager::Get()->GetScene()->SetCameraMatrix(&viewMat, &projMat);
	Manager::Get()->GetScene()->SetCameraPos(&pos);

	SkyDome* dome = ComponentFactory::CreateComponent<SkyDome>();
	dome->Set(1.0f);
	dome->SetOwner(this);
	componentsList.push_back(dome);

	front.y = -0.5f;
	
}



void Camera::Uninit()
{
}

void Camera::Update()
{
	//if (input->GetKeyPress('F')) {
	//	pos.z += 0.1f;
	//	at.z += 0.1f;
	//}
	//if (input->GetKeyPress('V')) {
	//	pos.z -= 0.1f;
	//	at.z -= 0.1f;
	//}
	//if (input->GetKeyPress('C')) {
	//	pos.x -= 0.1f;
	//	at.x -= 0.1f;
	//}
	//if (input->GetKeyPress('B')) {
	//	pos.x += 0.1f;
	//	at.x += 0.1f;
	//}
	/*if (input->GetKeyPress('H')) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(up, -0.05f);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		XMStoreFloat3(&front, frontVec);
	}

	if (input->GetKeyPress('K')) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(up,0.05f);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		XMStoreFloat3(&front, frontVec);
	}
	if (input->GetKeyPress('U')) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR right = XMVector3Cross(frontVec, up);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(right, 0.05f);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		up = XMVector3Rotate(up, RotateInfo);
		XMStoreFloat3(&front, frontVec);
	}
	if (input->GetKeyPress('J')) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR right = XMVector3Cross(frontVec, up);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(right, -0.05f);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		up = XMVector3Rotate(up, RotateInfo);
		XMStoreFloat3(&front, frontVec);
	}*/



	/*
	if (input->GetKeyPress(VK_LSHIFT)) {
		pos.y += 0.1f;
	}
	if (input->GetKeyPress(VK_RSHIFT)) {
		pos.y -= 0.1f;
	}
	*/
	

	if (owner) {
		// ポジション。プレイヤーの真後ろ
		XMFLOAT3 ownerRot = owner->GetRot();
		XMFLOAT3 defaultDir = XMFLOAT3(0.0f, 0.0f, -1.0f);
		XMMATRIX rotMtx = XMMatrixIdentity();
		rotMtx = XMMatrixRotationRollPitchYaw(ownerRot.x, ownerRot.y, ownerRot.z);
		XMVECTOR defaultVec = XMLoadFloat3(&defaultDir);
		XMVECTOR deltaPosVec = XMLoadFloat3(&deltaPos);
		defaultVec = XMVector3TransformNormal(defaultVec, rotMtx);
		deltaPosVec = XMVector3TransformNormal(deltaPosVec, rotMtx);
		XMFLOAT3 calcPos;
		XMStoreFloat3(&calcPos, deltaPosVec);


		pos = owner->GetPos() + calcPos;
		// 回転。一人称用にデルタ回転も
		// rot = owner->GetRot() + deltaRot;
		// 正面。at用。
		//XMStoreFloat3(&front, XMVector3Normalize(defaultVec));
		
		// ownerの深度を計算
		pos.x = owner->GetPos().x - front.x * focusLength;
		pos.z = owner->GetPos().z - front.z * focusLength;

		const XMFLOAT3 myPos = pos;
		const XMFLOAT3 ownerPos = owner->GetPos();
		// owner->カメラのベクトル
		XMVECTOR toOwner = XMLoadFloat3(&ownerPos) - XMLoadFloat3(&myPos);
		float lengthToOwner;
		XMStoreFloat(&lengthToOwner,XMVector3Length(toOwner));
		playerDepth = (lengthToOwner - nearDis) / (farDis - nearDis);

	}
	
	pos += deltaPos;
	//pos += offset;
	at = pos + front * focusLength;
}

void Camera::Draw()
{
	// 実質この関数が、Update関数

	// ビューポート設定(更新の必要ないなら、InitでOK？)
	D3D11_VIEWPORT dxViewport;
	dxViewport.Width = (float)WINDOW_WIDTH;
	dxViewport.Height = (float)WINDOW_HEIGHT;
	dxViewport.MinDepth = 0.0f;
	dxViewport.MaxDepth = 1.0f;
	dxViewport.TopLeftX = 0.0f;
	dxViewport.TopLeftY = 0.0f;

	context->RSSetViewports(1, &dxViewport);



	// ビューマトリクス設定

	/*
	XMMATRIX invMat;
	invMat = XMMatrixRotationRollPitchYaw(rot.x,rot.y,rot.z);
	invMat *= XMMatrixTranslation(pos.x,pos.y,pos.z);


	XMStoreFloat4x4(&viewMat, XMMatrixInverse(nullptr, invMat));
	*/

	XMStoreFloat4x4(&viewMat,XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&at), up));

	// プロジェクションマトリクス設定
	XMStoreFloat4x4(&projMat, XMMatrixPerspectiveFovLH(1.0f, dxViewport.Width / dxViewport.Height, nearDis, farDis));
	for (Component* comp : componentsList) {
		comp->Draw();
	}
}

void Camera::AddDeltaRot(XMFLOAT3 rot)
{
	XMFLOAT3 worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 worldRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMVECTOR upVec = XMLoadFloat3(&worldUp);
	XMVECTOR rightVec = XMLoadFloat3(&worldRight);


	if (fabsf(rot.x) > 0.01f) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(upVec, rot.x);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		XMStoreFloat3(&front, frontVec);
		up = XMVector3Rotate(up, RotateInfo);
	}

	
	/*if (fabsf(rot.y) > 0.01f) {
		XMVECTOR frontVec = XMLoadFloat3(&front);
		XMVECTOR right = XMVector3Cross(frontVec, up);
		XMVECTOR RotateInfo = XMQuaternionRotationAxis(rightVec,-rot.y);
		frontVec = XMVector3Rotate(frontVec, RotateInfo);
		up = XMVector3Rotate(up, RotateInfo);
		XMStoreFloat3(&front, frontVec);
	}*/

	pos.x = owner->GetPos().x - front.x * focusLength;
	pos.y += -deltaPos.y;
	pos.z = owner->GetPos().z - front.z * focusLength;
	pos += deltaPos;
	at = pos + front * focusLength;
}


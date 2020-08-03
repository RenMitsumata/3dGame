#include "Manager.h"
#include "GameObject.h"


GameObject::GameObject() :
	pos(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	rot(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	siz(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	front(XMFLOAT3(0.0f, 0.0f, 1.0f)),
	up(XMFLOAT3(0.0f,1.0f,0.0f)),
	velocity(XMFLOAT3(0.0f,0.0f,0.0f))
{
}


GameObject::~GameObject()
{
	//for (Component* comp : componentsList) {
	//	comp->Uninit();
	//	delete comp;
	//}
	//componentsList.clear();
	/*   子ゲームオブジェクトはすべてシーンが管理しているので、*/
	/*   親からは消さない（リストだけクリア）                  */
	//childrensList.clear();
}

void GameObject::Init()
{
	division = Manager::Get()->GetScene()->GetDivision();
}

void GameObject::Uninit()
{
	for (Component* comp : componentsList) {
		comp->Uninit();
		comp->SetOwner(nullptr);
		delete comp;
		comp = nullptr;
	}
	bool isEmpty = componentsList.empty();
	if (isEmpty) {
		int a = 0;
	}
	/*   子ゲームオブジェクトはすべてシーンが管理しているので、*/
	/*   親からは消さない（リストだけクリア）                  */
	//childrensList.clear();
}

void GameObject::Update()
{
	for (Component* comp : componentsList) {
		comp->Update();
	}
}

void GameObject::Draw()
{
	for (Component* comp : componentsList) {
		comp->Draw();
	}
}

XMFLOAT4X4 GameObject::GetTransformMatrix()
{
	XMMATRIX matrix = XMMatrixIdentity();
	matrix *= XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix *= XMMatrixScaling(siz.x, siz.y, siz.z);
	matrix *= XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMFLOAT4X4 ret;
	XMStoreFloat4x4(&ret, matrix);
	return ret;
}


void GameObject::UpdateBlock()
{
	// 中心を(0,0,0)と想定 -> 所属ブロック計算のため、ワールドの広さの半分を足す
	XMFLOAT3 leftAwayPos = XMFLOAT3(pos.x - judgeWidth + (division->worldWidth / 2), 0.0f, pos.z + judgeDepth + (division->worldWidth / 2));
	XMFLOAT3 rightNearPos = XMFLOAT3(pos.x + judgeWidth, 0.0f, pos.z - judgeDepth);
	float UnitLength = division->divisionNum / division->worldWidth;
	XMINT2 intLeftAwayPos = XMINT2((int)leftAwayPos.x / UnitLength, (int)leftAwayPos.z / UnitLength);
	XMINT2 intRightNearPos = XMINT2((int)rightNearPos.x / UnitLength, (int)rightNearPos.z / UnitLength);
	// 簡易当たり判定の左奥、右手前がどの区画に位置しているかを調査
	// まずは左奥
	DWORD L_A_P_PosX = (DWORD)intLeftAwayPos.x;
	DWORD L_A_P_PosY = (DWORD)intLeftAwayPos.y;
	L_A_P_PosX = (L_A_P_PosX << 8) | L_A_P_PosX;
	L_A_P_PosX & 0x00ff00ff;
	L_A_P_PosX = (L_A_P_PosX << 4) | L_A_P_PosX;
	L_A_P_PosX & 0x0f0f0f0f;
	L_A_P_PosX = (L_A_P_PosX << 2) | L_A_P_PosX;
	L_A_P_PosX & 0x33333333;
	L_A_P_PosX = (L_A_P_PosX << 1) | L_A_P_PosX;
	L_A_P_PosX & 0x55555555;
	L_A_P_PosY = (L_A_P_PosY << 8) | L_A_P_PosY;
	L_A_P_PosY & 0x00ff00ff;
	L_A_P_PosY = (L_A_P_PosY << 4) | L_A_P_PosY;
	L_A_P_PosY & 0x0f0f0f0f;
	L_A_P_PosY = (L_A_P_PosY << 2) | L_A_P_PosY;
	L_A_P_PosY & 0x33333333;
	L_A_P_PosY = (L_A_P_PosY << 1) | L_A_P_PosY;
	L_A_P_PosY & 0x55555555;
	L_A_P_PosY <<= 1;
	DWORD L_A_P_Pos = L_A_P_PosX | L_A_P_PosY;	// L_A_P_Posを数値型に代入すると、区画番号を得られる

	// 次に右手前

	DWORD R_N_P_PosX = (DWORD)intRightNearPos.x;
	DWORD R_N_P_PosY = (DWORD)intRightNearPos.y;
	R_N_P_PosX = (R_N_P_PosX << 8) | R_N_P_PosX;
	R_N_P_PosX & 0x00ff00ff;
	R_N_P_PosX = (R_N_P_PosX << 4) | R_N_P_PosX;
	R_N_P_PosX & 0x0f0f0f0f;
	R_N_P_PosX = (R_N_P_PosX << 2) | R_N_P_PosX;
	R_N_P_PosX & 0x33333333;
	R_N_P_PosX = (R_N_P_PosX << 1) | R_N_P_PosX;
	R_N_P_PosX & 0x55555555;
	R_N_P_PosY = (R_N_P_PosY << 8) | R_N_P_PosY;
	R_N_P_PosY & 0x00ff00ff;
	R_N_P_PosY = (R_N_P_PosY << 4) | R_N_P_PosY;
	R_N_P_PosY & 0x0f0f0f0f;
	R_N_P_PosY = (R_N_P_PosY << 2) | R_N_P_PosY;
	R_N_P_PosY & 0x33333333;
	R_N_P_PosY = (R_N_P_PosY << 1) | R_N_P_PosY;
	R_N_P_PosY & 0x55555555;
	R_N_P_PosY <<= 1;
	DWORD R_N_P_Pos = R_N_P_PosX | R_N_P_PosY;	// R_N_P_Posを数値型に代入すると、区画番号を得られる

	divisionNum = L_A_P_Pos ^ R_N_P_Pos;
	/*
	　　具体的に何番区画にいるかが重要ではなく、判定すべき相手が誰なのかをdivisionNumとして持っておく。
	　　所属する区画は、最下位ビットから２ビット区切りで、一番上の「０」じゃない２ビットの番号でわかる。
	  　例）divisionNum = 0x000000B3 -> 00…|00|10|11|00|11|
	   　　　この例だと、最下位ビットから数えて４番目区画まで値があるので、
		  　 所属するのは、一番細かい区画から数えて、３つ上の階層になる。
	*/
}

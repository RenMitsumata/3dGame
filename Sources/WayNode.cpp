#define NOMINMAX
#include "WayNode.h"
#include "Model.h"
#include "Manager.h"
#include "window.h"
#include "WayNodeManager.h"

WayNode::WayNode()
{
}


WayNode::~WayNode()
{
}

void WayNode::Init()
{
	model = new Model;
	model->Load("Assets/Models/redbox.fbx");
	model->SetOwner(this);
	componentsList.push_back(model);
	siz = XMFLOAT3(0.01f, 0.01f, 0.01f);
}

void WayNode::Uninit()
{
	if (neighborInfo) {
		delete[] neighborInfo;
	}
}

void WayNode::SetNeighbor(unsigned short* pArray, unsigned short arraySize)
{
	this->arraySize = arraySize;
	Scene* scene = Manager::Get()->GetScene();
	if (scene == nullptr) {
		MessageBox(Manager::Get()->GetWindow()->GethWnd(), "現在シーンを取得できませんでした", "WayNodeManager", MB_ICONHAND | MB_OK);
		exit(-1);
	}
	WayNodeManager* nodeMgr = scene->GetNodeManager();


	neighborInfo = new WAYNODE_STATUS[arraySize];
	for (int i = 0; i < arraySize; i++) {
		neighborInfo[i].neighborID = pArray[i];
		// お隣さんのIDが既に登録されているのなら、距離を求める
		if (nodeMgr->GetNode(neighborInfo[i].neighborID) != NULL) {
			XMFLOAT3 otherPos = nodeMgr->GetNode(neighborInfo[i].neighborID)->GetPos();
			XMFLOAT3 myPos = GetPos();
			XMVECTOR toOtherVec = XMLoadFloat3(&(otherPos - myPos));
			XMStoreFloat(&neighborInfo[i].length, XMVector3Length(toOtherVec));

			// さらに、相手にも計算結果を届ける
			nodeMgr->GetNode(neighborInfo[i].neighborID)->SetDistance(id, neighborInfo[i].length);

		}
		// お隣さんが未登録だったら、-1を入れておく
		else {
			neighborInfo[i].length = -1;
		}
	}
}

void WayNode::SetDistance(unsigned short id, float length)
{
	for (int i = 0; i < arraySize; i++) {
		if (neighborInfo[i].neighborID == id) {
			neighborInfo[i].length = length;
			return;
		}
	}
	MessageBox(Manager::Get()->GetWindow()->GethWnd(), "隣接ノードに距離を設定できません。", "エラー", MB_ICONHAND | MB_OK);
	exit(-1);
}

void WayNode::SetID(unsigned short myID)
{
	id = myID;
}

unsigned short WayNode::GetID()
{
	return id;
}

NEIGHBOR_INFO WayNode::GetNeighborInfo()
{
	NEIGHBOR_INFO ret;
	ret.neighborInfo = neighborInfo;
	ret.num = arraySize;
	return ret;
}



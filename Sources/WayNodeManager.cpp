#include "WayNodeManager.h"
#include "WayNode.h"
#include "Manager.h"
#include "window.h"
#include <iterator>
#include <algorithm>


WayNodeManager::WayNodeManager()
{
}


WayNodeManager::~WayNodeManager()
{
}

void WayNodeManager::Init()
{
}

void WayNodeManager::Uninit()
{
}

void WayNodeManager::AddList(WayNode* node)
{
	unsigned short id = node->GetID();
	if (NodeList[id] != nullptr) {
		MessageBox(Manager::Get()->GetWindow()->GethWnd(), "ノードIDが重複しています", "ノード登録エラー", MB_ICONHAND | MB_OK);
		exit(-1);
	}
	NodeList[id] = node;
}

WayNode* WayNodeManager::GetNode(unsigned short id)
{
	if (NodeList[id]) {
		return NodeList[id];
	}
	else return nullptr;
}

unsigned short WayNodeManager::FindDestNode(XMFLOAT3 pos)
{
	unsigned short dest = 0xFFFF;	// 返すノードＩＤ
	float shortest_length = 5.0f;	// 最近ノードまでの距離

	//　一定距離内のノードを検索
	
	for (std::unordered_map<unsigned short, WayNode*>::iterator itr = NodeList.begin(); itr != NodeList.end(); itr++) {
		// 連想配列を先頭から検索
		WayNode* way = itr->second;
		XMFLOAT3 posNode = way->GetPos();
		float length;
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&pos) - XMLoadFloat3(&posNode)));
		if (length < shortest_length) {
			dest = way->GetID();
			shortest_length = length;
		}
	}
	
	
	return dest;
}

std::vector<unsigned short> WayNodeManager::FindAStar(XMFLOAT3 destPos, XMFLOAT3 myPos)
{
	// 道順のリセット
	routeList.clear();

	// 自分の至近ノードを求める
	unsigned short myID = FindDestNode(myPos);

	// 目的地の至近ノードを求める
	unsigned short destID = FindDestNode(destPos);

	// 経路探索をする
	CanReach(myID, destID);

	return routeList;

}

bool WayNodeManager::CanReach(unsigned short curID, unsigned short destID)
{
	routeList.push_back(curID);

	NEIGHBOR_INFO neighbors = NodeList[curID]->GetNeighborInfo();
	
	std::vector<WAYNODE_STATUS> waynodeList;

	for (int i = 0; i < neighbors.num; i++) {
		WAYNODE_STATUS waynode;
		waynode.neighborID = neighbors.neighborInfo[i].neighborID;
		waynode.length = CalcDistance(neighbors.neighborInfo[i].neighborID, destID);
		waynodeList.push_back(waynode);
	}
	
	// 目的地への距離が近いノード順に並び替え
	std::sort(waynodeList.begin(), waynodeList.end(), [](WAYNODE_STATUS x, WAYNODE_STATUS y) {return x.length < y.length; });
	



	for (int i = 0; i < neighbors.num; i++) {
		
		// 一度通ったノードは通らない
		bool isOncePassed = false;

		for (auto t : routeList) {
			if (waynodeList[i].neighborID == t) {
				isOncePassed = true;
				break;
			}
		}

		if (isOncePassed) {
			continue;
		}
		

		// destIDと一致するなら、到達したことになる
		if (waynodeList[i].neighborID == destID) {
			return true;
		}

		// まだ通っていないノードの場合、さらに先を調べる(目的地への距離が近い順)
		bool isReach = CanReach(waynodeList[i].neighborID, destID);

		if (isReach) {
			return true;
		}		
	}

	// このノードからは到達できなかった
	routeList.pop_back();
	return false;
}

float WayNodeManager::CalcDistance(unsigned short startID, unsigned short destID)
{
	XMFLOAT3 startpos = NodeList[startID]->GetPos();
	XMFLOAT3 destpos = NodeList[destID]->GetPos();
	XMVECTOR lengthVec = XMVector3Length(XMLoadFloat3(&destpos) - XMLoadFloat3(&startpos));
	float ret;
	XMStoreFloat(&ret, lengthVec);
	return ret;
}

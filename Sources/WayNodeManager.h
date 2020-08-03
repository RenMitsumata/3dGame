#pragma once

#include <list>
#include <unordered_map>
#include "DXManager.h"

class WayNode;

class WayNodeManager
{
private:
	std::unordered_map<unsigned short, WayNode*> NodeList;
	std::vector<unsigned short> routeList;
	bool CanReach(unsigned short curID, unsigned short destID);			// 再起関数。目的地へ到達できるならばtrue、できないならfalseを返す
	float CalcDistance(unsigned short startID, unsigned short destID);	// 目的ノードへの距離を返す
public:
	WayNodeManager();
	~WayNodeManager();
	void Init();
	void Uninit();
	void AddList(WayNode* node);
	WayNode* GetNode(unsigned short id);
	unsigned short FindDestNode(XMFLOAT3 pos);	// 与えられた位置に対して、目標地点とすべきノードを返す(pos->最近ノード)
	std::vector<unsigned short> FindAStar(XMFLOAT3 destPos, XMFLOAT3 myPos);
};


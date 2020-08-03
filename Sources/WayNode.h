#pragma once

#include "GameObject.h"

class Model;

struct WAYNODE_STATUS {
	unsigned short neighborID;
	float length;
};

struct NEIGHBOR_INFO {
	int num;									// 配列の要素数
	WAYNODE_STATUS* neighborInfo = nullptr;		// 隣情報
};


class WayNode :	public GameObject
{
private:
	unsigned short id;
	Model* model = nullptr;
	WAYNODE_STATUS* neighborInfo = nullptr;
	unsigned short arraySize;
public:
	WayNode();
	~WayNode();
	void Init();
	void Uninit();
	void SetNeighbor(unsigned short* pArray, unsigned short arraySize);	// マネージャで呼ぶ想定
	void SetDistance(unsigned short id, float length);
	void SetID(unsigned short myID);
	unsigned short GetID();
	NEIGHBOR_INFO GetNeighborInfo();
};


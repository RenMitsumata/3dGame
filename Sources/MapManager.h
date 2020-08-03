#pragma once

// マップを外部ファイルから呼び出す
#include <mutex>

class MapManager
{
public:
	MapManager();
	~MapManager();
	void Init();
	void Uninit();
	void ReadMap(const char* filename, std::mutex& mut);
};


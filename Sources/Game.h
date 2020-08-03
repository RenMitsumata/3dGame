#pragma once
#include "Scene.h"


class UI;
class DeferredObject;

class Game : public Scene
{
public:
	enum Phase {
		e_LOADING,
		e_SCENE,
		e_CHOOSEITEM,
		e_CLEARMOVIE,
		e_CLEAR
	};
private:	
	Phase phase = e_LOADING;
	ID3D11DeviceContext* context = nullptr;
	DXManager* dxManager = nullptr;
	UI* loadingPolygon = nullptr;
	UI* deferredPolygon = nullptr;
	UI* albedoPolygon = nullptr;
	UI** equip = nullptr;
	unsigned short curEquip = 0;

public:
	Game();
	~Game();
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
	void DrawDeferred()override;
	void SetPhase(Phase phase);
	void SetMapManager(MapManager* mapMgr);
};


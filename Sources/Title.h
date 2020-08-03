#pragma once
#include "Scene.h"

class InputManager;
class UI;

class Title : public Scene
{
private:
	InputManager* input;
	DXManager* dxManager = nullptr;
	UI* albedoPolygon = nullptr;

public:
	Title();
	~Title();
	void Init()override;
	void Update()override;
	void DrawDeferred()override;
};


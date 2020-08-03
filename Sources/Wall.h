#pragma once
#include "GameObject.h"
class Wall :
	public GameObject
{
public:
	Wall();
	~Wall();
	void Update()override;
};


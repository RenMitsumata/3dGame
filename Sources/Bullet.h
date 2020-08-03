#pragma once
#include "GameObject.h"
class Bullet : public GameObject
{
public:
	Bullet();
	~Bullet();
	void Init()override;
	void Update()override;
};


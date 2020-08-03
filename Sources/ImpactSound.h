#pragma once
#include "GameObject.h"
class ImpactSound :	public GameObject
{
public:
	ImpactSound();
	~ImpactSound();
	void Init()override;
	void Update()override;
};


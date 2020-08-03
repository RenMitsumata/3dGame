#include "ImpactSound.h"
#include "CollisionSphere.h"


ImpactSound::ImpactSound()
{
}


ImpactSound::~ImpactSound()
{
	int a = 0;
}

void ImpactSound::Init()
{
	CollisionSphere* col = new CollisionSphere();
	col->Init();
	col->SetRadius(10.0f);
	col->SetTag(e_COLTYPE_SOUND);
	col->SetOwner(this);
	componentsList.push_back(col);
}

void ImpactSound::Update()
{
	SetDestroy();
}

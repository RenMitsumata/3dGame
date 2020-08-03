#pragma once

class GameObject;

class Component
{
protected:
	GameObject* owner;
public:
	Component();
	virtual ~Component();
	virtual void Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	virtual void SetOwner(GameObject* owner) { this->owner = owner; }
	GameObject* GetOwner() { return owner; }
};


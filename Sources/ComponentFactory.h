#pragma once




// 前方宣言
class Model;
class CollisionOBB;
class CollisionSphere;


class ComponentFactory
{
public:
	enum ComponentType {
		e_Component_Model,
		// コンポーネントの種類を追加
	};

	ComponentFactory();
	~ComponentFactory();

	template <class T>
	static T* CreateComponent(){
		T* comp = new T;
		comp->Init();
		return comp;
	}
};


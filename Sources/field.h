#pragma once



#include "Shader3D_Normal.h"

class Field : public GameObject
{
private:

	ID3D11Buffer*	m_VertexBuffer = NULL;
	ID3D11Buffer*	m_IndexBuffer = NULL;
	Texture*		m_Texture = NULL;
	Texture*		m_Normal;
	Shader3D_Normal* shader;

	static const int FIELD_X = 32;
	static const int FIELD_Z = 32;

	VERTEX_3D_Normal m_Vertex[FIELD_X * FIELD_Z];


public:
	void Init();
	void Uninit();
	void Update();
	void Draw();

};
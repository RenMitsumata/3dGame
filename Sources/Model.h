#pragma once

#include "main.h"
#include "Component.h"
#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
//#include <assimp\matrix4x4.h>
//#include <assimp/texture.h>
//#pragma comment (lib,"assimp.lib")

#include "DXManager.h"
#include "Manager.h"
#include "Shader3D.h"
#include "Texture.h"
#include "GameObject.h"
#include <unordered_map>
#include <vector>

// ノード構造体。子ノードなどの情報を確保
struct NODE {	// <-- delete[]を忘れずに！
	NODE* parentNode;
	NODE** childNode;
	unsigned int childNum;
	unsigned int* meshIndex;
	unsigned int meshNum;
	const char* nodeName;
	XMFLOAT4X4 offsetMatrix;
};

// メッシュ構造体
struct MESH {
	unsigned int indexOffset;
	unsigned int indexNum;
	unsigned int materialIndex;
};

// アニメーション用頂点構造体
struct DEFORM_VERTEX {
	XMFLOAT3 position;
	XMFLOAT3 deformPosition;
	XMFLOAT3 normal;
	XMFLOAT3 deformNormal;
	int boneNum;
	// int boneIndex[4];	←早いほう
	std::string boneName[4];
	float boneWeight[4];

};

// アニメーション用ボーンの構造体
struct BONE {
	// std::string name;
	XMFLOAT4X4 matrix;
	XMFLOAT4X4 animationMatrix;
	XMFLOAT4X4 offsetMatrix;
};

// アニメーションのデータを格納する構造体
struct ANIM_CHANNEL {
	std::unordered_map<std::string, XMFLOAT4>* rotDatas;	// クォータニオン
	std::unordered_map<std::string, XMFLOAT3>* posDatas;
};

struct ANIM_DATA {
	ANIM_CHANNEL* channels;
};



class Model : public Component
{
private:
	Manager* manager;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Texture** textureList;
	unsigned int textureNum;
	MATERIAL** materialList;
	unsigned int materialNum;
	VERTEX_3D* vertexList;
	MESH* meshList;
	unsigned short* indexList;
	NODE* rootNode;

	// アニメーション関連
	std::vector<DEFORM_VERTEX>* pDeformVertexs = nullptr;
	std::unordered_map<std::string, BONE> Bones;
	ANIM_DATA* animData;
	unsigned int animNum;
	unsigned int* animChannels;	// FBXファイル１つにつき、複数アニメーションにするときは配列にする

	void CreateBone(NODE* node);
	void UpdateBoneMatrix(NODE* node, XMFLOAT4X4* defMat);

	NODE* RegisterNode(aiNode* pNode);
	void DeleteNode(NODE* node);
	void DrawNode(NODE* node, XMMATRIX mat);
public:
	Model();
	~Model();
	void Update()override;
	void Draw()override;
	void Load(const char* filename);
	void Uninit()override;
};


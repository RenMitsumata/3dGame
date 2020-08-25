#pragma once

#include "main.h"
#include "Component.h"
#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp\matrix4x4.h>
#include <assimp/texture.h>

#pragma comment (lib,"assimp.lib")

#include "DXManager.h"
#include "Manager.h"
#include "Shader3D.h"
#include "Texture.h"
#include "GameObject.h"
#include <unordered_map>
#include <vector>

// ノード構造体。子ノードなどの情報を確保
struct NODE {	// <-- delete[]を忘れずに！
	NODE* parentNode;			// 親のノード（ポインタ）
	NODE** childNode;			// 子のノード（ポインタの配列）
	unsigned int childNum;		// ↑の配列のサイズ
	unsigned int* meshIndex;	// ノードに属するメッシュのインデックス（配列・メッシュデータは別の場所に保存）
	unsigned int meshNum;		// ↑の配列のサイズ
	std::string nodeName;
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
	XMFLOAT4 diffuse;
	XMFLOAT2 texcoord;
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
	std::string nodeName;
	std::unordered_map<std::string, XMFLOAT4>* rotDatas;	// クォータニオン
	std::unordered_map<std::string, XMFLOAT3>* posDatas;
	std::unordered_map<std::string, XMFLOAT3>* sizDatas;
	unsigned int rotKeyNum;
	unsigned int posKeyNum;
	unsigned int sizKeyNum;
};

struct ANIM_DATA {
	ANIM_CHANNEL* channels;		// １つのアニメーションのデータ（歩く、走るetc）
	unsigned int channelNum;	// アニメーションチャンネルの数
};



class Model : public Component
{
private:
	// マネージャ、D3D11関係
	Manager* manager;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// テクスチャ（シェーダーリソース）関係
	Texture** textureList;		// テクスチャ（クラスのポインタ）の配列
	unsigned int textureNum;	// ↑の配列サイズ

	// マテリアル（シェーダー定数）関係
	MATERIAL** materialList;	// マテリアル（構造体のポインタ）の配列
	unsigned int materialNum;	// ↑の配列サイズ
	
	// メッシュ関係
	VERTEX_3D* vertexList;		// 頂点バッファ用の座標データの配列（ポーズ時）
	MESH* meshList;				// 頂点バッファのインデックスのオフセット、プリミティブ数、マテリアル情報などを収納
	unsigned short* indexList;	// インデックスバッファ用のデータ
	NODE* rootNode;				// ルートノードのポインタ



	// アニメーション関係
	unsigned int animNum;		// FBXファイルに搭載されているアニメーションの数
	unsigned int* channelNum;	// 　　　　　　〃　　　　　　 チャンネルの数（アニメーション数分の配列）

	void CalcAnimVertex(const aiMatrix4x4& matrix, const unsigned int& frame, const aiScene* pScene, const aiNodeAnim* pChannel,const unsigned int& channelNum, const aiNode* pNode, VERTEX_3D** ppVertex);

	

	/*
	unsigned int VerNum;
	VERTEX_3D* ver;

	// アニメーション関連
	std::vector<DEFORM_VERTEX>* pDeformVertexs = nullptr;
	unsigned short meshNum;
	std::unordered_map<std::string, BONE> Bones;
	ANIM_DATA* animData = nullptr;
	unsigned int* animChannels;	// FBXファイル１つにつき、複数アニメーションにするときは配列にする
	bool isAnimated = true;	// アニメーション中か
	unsigned int animNum;
	unsigned int curAnimNum = 0;	// 再生中のアニメーションの番号
	unsigned int animFrame = 0; // 再生中のアニメーションのフレーム番号

	bool isTransition = false;				// アニメーションが遷移中かどうか
	unsigned int lastAnim = 0;				// 前のアニメーション番号（遷移用）
	unsigned int lastAnimFrame = 0;			// ↑のアニメーションのフレーム番号（遷移用）
	unsigned short transCount = 0;			// アニメーション遷移完了までの所要フレーム
	unsigned short currentTransCount = 0;	// ↑の現在フレーム


	*/



	void CreateBone(NODE* node);
	void UpdateBoneMatrix(NODE* node, XMFLOAT4X4* defMat);
	void UpdateAnimation();

	NODE* RegisterNode(aiNode* pNode);
	void DeleteNode(NODE* node);
	void DrawNode(NODE* node, XMFLOAT4X4* mat);
	XMFLOAT4X4 Convert_aiMatrix(const aiMatrix4x4& aiMat);
public:
	Model();
	~Model();
	void Update()override;
	void Draw()override;
	void Load(const char* filename);
	void Uninit()override;
};


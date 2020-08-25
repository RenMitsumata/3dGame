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

// �m�[�h�\���́B�q�m�[�h�Ȃǂ̏����m��
struct NODE {	// <-- delete[]��Y�ꂸ�ɁI
	NODE* parentNode;			// �e�̃m�[�h�i�|�C���^�j
	NODE** childNode;			// �q�̃m�[�h�i�|�C���^�̔z��j
	unsigned int childNum;		// ���̔z��̃T�C�Y
	unsigned int* meshIndex;	// �m�[�h�ɑ����郁�b�V���̃C���f�b�N�X�i�z��E���b�V���f�[�^�͕ʂ̏ꏊ�ɕۑ��j
	unsigned int meshNum;		// ���̔z��̃T�C�Y
	std::string nodeName;
	XMFLOAT4X4 offsetMatrix;
};

// ���b�V���\����
struct MESH {
	unsigned int indexOffset;
	unsigned int indexNum;
	unsigned int materialIndex;
};

// �A�j���[�V�����p���_�\����
struct DEFORM_VERTEX {
	XMFLOAT3 position;
	XMFLOAT3 deformPosition;
	XMFLOAT3 normal;
	XMFLOAT3 deformNormal;
	XMFLOAT4 diffuse;
	XMFLOAT2 texcoord;
	int boneNum;
	// int boneIndex[4];	�������ق�
	std::string boneName[4];
	float boneWeight[4];

};

// �A�j���[�V�����p�{�[���̍\����
struct BONE {
	// std::string name;
	XMFLOAT4X4 matrix;
	XMFLOAT4X4 animationMatrix;
	XMFLOAT4X4 offsetMatrix;
};

// �A�j���[�V�����̃f�[�^���i�[����\����
struct ANIM_CHANNEL {
	std::string nodeName;
	std::unordered_map<std::string, XMFLOAT4>* rotDatas;	// �N�H�[�^�j�I��
	std::unordered_map<std::string, XMFLOAT3>* posDatas;
	std::unordered_map<std::string, XMFLOAT3>* sizDatas;
	unsigned int rotKeyNum;
	unsigned int posKeyNum;
	unsigned int sizKeyNum;
};

struct ANIM_DATA {
	ANIM_CHANNEL* channels;		// �P�̃A�j���[�V�����̃f�[�^�i�����A����etc�j
	unsigned int channelNum;	// �A�j���[�V�����`�����l���̐�
};



class Model : public Component
{
private:
	// �}�l�[�W���AD3D11�֌W
	Manager* manager;
	Shader3D* shader;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// �e�N�X�`���i�V�F�[�_�[���\�[�X�j�֌W
	Texture** textureList;		// �e�N�X�`���i�N���X�̃|�C���^�j�̔z��
	unsigned int textureNum;	// ���̔z��T�C�Y

	// �}�e���A���i�V�F�[�_�[�萔�j�֌W
	MATERIAL** materialList;	// �}�e���A���i�\���̂̃|�C���^�j�̔z��
	unsigned int materialNum;	// ���̔z��T�C�Y
	
	// ���b�V���֌W
	VERTEX_3D* vertexList;		// ���_�o�b�t�@�p�̍��W�f�[�^�̔z��i�|�[�Y���j
	MESH* meshList;				// ���_�o�b�t�@�̃C���f�b�N�X�̃I�t�Z�b�g�A�v���~�e�B�u���A�}�e���A�����Ȃǂ����[
	unsigned short* indexList;	// �C���f�b�N�X�o�b�t�@�p�̃f�[�^
	NODE* rootNode;				// ���[�g�m�[�h�̃|�C���^



	// �A�j���[�V�����֌W
	unsigned int animNum;		// FBX�t�@�C���ɓ��ڂ���Ă���A�j���[�V�����̐�
	unsigned int* channelNum;	// �@�@�@�@�@�@�V�@�@�@�@�@�@ �`�����l���̐��i�A�j���[�V���������̔z��j

	void CalcAnimVertex(const aiMatrix4x4& matrix, const unsigned int& frame, const aiScene* pScene, const aiNodeAnim* pChannel,const unsigned int& channelNum, const aiNode* pNode, VERTEX_3D** ppVertex);

	

	/*
	unsigned int VerNum;
	VERTEX_3D* ver;

	// �A�j���[�V�����֘A
	std::vector<DEFORM_VERTEX>* pDeformVertexs = nullptr;
	unsigned short meshNum;
	std::unordered_map<std::string, BONE> Bones;
	ANIM_DATA* animData = nullptr;
	unsigned int* animChannels;	// FBX�t�@�C���P�ɂ��A�����A�j���[�V�����ɂ���Ƃ��͔z��ɂ���
	bool isAnimated = true;	// �A�j���[�V��������
	unsigned int animNum;
	unsigned int curAnimNum = 0;	// �Đ����̃A�j���[�V�����̔ԍ�
	unsigned int animFrame = 0; // �Đ����̃A�j���[�V�����̃t���[���ԍ�

	bool isTransition = false;				// �A�j���[�V�������J�ڒ����ǂ���
	unsigned int lastAnim = 0;				// �O�̃A�j���[�V�����ԍ��i�J�ڗp�j
	unsigned int lastAnimFrame = 0;			// ���̃A�j���[�V�����̃t���[���ԍ��i�J�ڗp�j
	unsigned short transCount = 0;			// �A�j���[�V�����J�ڊ����܂ł̏��v�t���[��
	unsigned short currentTransCount = 0;	// ���̌��݃t���[��


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


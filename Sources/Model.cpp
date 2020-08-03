

#define NOMINMAX

#include "Scene.h"
#include <Windows.h>
#include <string>
#include <list>
#include "Model.h"


Model::Model()
{
	manager = Manager::Get();
	// DirectXManagerの取得
	if (Manager::Get()->GetDXManager() == nullptr) {
		MessageBox(NULL, "DirectXマネージャの取得に失敗しました", "Modelローダー", MB_OK | MB_ICONHAND);
		exit(1);
	}
	device = Manager::Get()->GetDXManager()->GetDevice();
	context = Manager::Get()->GetDXManager()->GetDeviceContext();
	shader = new Shader3D;
	shader->Init();
}


Model::~Model()
{
	
}

void Model::Update()
{
	

	// 再帰的にボーンデータを更新する
	//UpdateBoneMatrix(pScene->mRootNode, &aiMatrix4x4());

	
}

void Model::Draw()
{
	NODE* curNode = rootNode;
	XMFLOAT4X4 MatLoc = owner->GetTransformMatrix();
	
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->SetViewMatrix(manager->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(manager->GetScene()->GetProjectionMatrix());
	
	manager->GetDXManager()->SetDepthEnable(true);
	
	XMMATRIX mat = XMLoadFloat4x4(&MatLoc);
	mat = XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(180.0f), 0.0f) * mat;
	//mat = XMMatrixTranspose(mat);
	DrawNode(rootNode,mat);
}

void Model::Load(const char* filename)
{
	const aiScene* pScene;

	// aiSceneの読み込み
	pScene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (pScene == nullptr) {
		char filestring[256];
		lstrcpy(filestring, filename);
		char exp[32] = { "が読み込めません" };
		lstrcat(filestring, exp);
		MessageBox(NULL, filestring, "Assimp", MB_OK | MB_ICONHAND);
		exit(1);
	}

	// aiTextureの読み込み->シェーダが読める形に変換
	textureList = new Texture*[pScene->mNumMaterials];
	textureNum = 0;

	for (int i = 0; i < pScene->mNumMaterials; i++) {
		aiString path;	// 画像ファイルのファイルパス用バッファ

		if (pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
			// マテリアルに画像がある
			textureList[i] = new Texture;
			// 画像は内部ファイル？外部ファイル？
			if (path.data[0] == '*') {
				
				// FBX内部に画像ファイルがある（バージョンによって異なるので注意）

				int id = atoi(&path.data[1]);
				textureList[i]->LoadTextureFromMemory((const unsigned char*)pScene->mTextures[id]->pcData, pScene->mTextures[id]->mWidth);
			}
			else {
				std::string texPath = path.data;
				size_t pos = texPath.find_last_of("\\/");
				std::string headerPath = path.data;
				headerPath = headerPath.erase(0, pos + 1);
				char texFilePath[256] = "Assets/Models/Textures/";
				strcat(texFilePath, headerPath.c_str());
				textureList[i]->Load(texFilePath);
			}
		}
		else {
			textureList[i] = 0x00;
		}
		textureNum++;
	}




	// aiMaterialの読み込み->シェーダが読める形に変換
	materialList = new MATERIAL*[pScene->mNumMaterials];
	materialNum = 0;
	for (int i = 0; i < pScene->mNumMaterials; i++) {
		materialList[i] = new MATERIAL;
		aiMaterial* buf = pScene->mMaterials[i];
		aiColor4D color;
		aiGetMaterialColor(buf, AI_MATKEY_COLOR_DIFFUSE, &color);
		materialList[i]->Diffuse.r = color.r;
		materialList[i]->Diffuse.g = color.g;
		materialList[i]->Diffuse.b = color.b;
		materialList[i]->Diffuse.a = color.a;
		aiGetMaterialColor(buf, AI_MATKEY_COLOR_AMBIENT, &color);
		materialList[i]->Ambient.r = color.r;
		materialList[i]->Ambient.g = color.g;
		materialList[i]->Ambient.b = color.b;
		materialList[i]->Ambient.a = color.a;
		aiGetMaterialColor(buf, AI_MATKEY_COLOR_SPECULAR, &color);
		materialList[i]->Specular.r = color.r;
		materialList[i]->Specular.g = color.g;
		materialList[i]->Specular.b = color.b;
		materialList[i]->Specular.a = color.a;
		aiGetMaterialColor(buf, AI_MATKEY_COLOR_EMISSIVE, &color);
		materialList[i]->Emission.r = color.r;
		materialList[i]->Emission.g = color.g;
		materialList[i]->Emission.b = color.b;
		materialList[i]->Emission.a = color.a;

		materialNum++;
	}

	// ノード情報の登録
	aiNode* pNode = pScene->mRootNode;
	rootNode = RegisterNode(pNode);

	// 頂点情報の登録
	std::list<VERTEX_3D> vertexBuf;
	pDeformVertexs = new std::vector<DEFORM_VERTEX>[pScene->mNumMeshes];

	// アニメーションデータの登録
	animNum = pScene->mNumAnimations;
	if (animNum > 0) {
		animChannels = new unsigned int[animNum];
		animData = new ANIM_DATA[animNum];
	}

	for (int a = 0; a < animNum; a++) {
		aiAnimation* pAnimation = pScene->mAnimations[a];
		animChannels[a] = pAnimation->mNumChannels;
		animData[a].channels = new ANIM_CHANNEL[pAnimation->mNumChannels];

		for (auto c = 0; c < pAnimation->mNumChannels; c++) {
			aiNodeAnim* pNodeAnim = pAnimation->mChannels[c];
			animData[a].channels[c].rotDatas = new std::unordered_map<std::string, XMFLOAT4>[pNodeAnim->mNumRotationKeys];
			animData[a].channels[c].posDatas = new std::unordered_map<std::string, XMFLOAT3>[pNodeAnim->mNumPositionKeys];

			for (int frame = 0; frame < pNodeAnim->mNumRotationKeys; frame++) {
				XMFLOAT4 quat;
				quat.w = pNodeAnim->mRotationKeys[frame].mValue.w;
				quat.x = pNodeAnim->mRotationKeys[frame].mValue.x;
				quat.y = pNodeAnim->mRotationKeys[frame].mValue.y;
				quat.z = pNodeAnim->mRotationKeys[frame].mValue.z;
				(animData[a].channels[c].rotDatas[frame])[pNodeAnim->mNodeName.C_Str()] = quat;
			}

			for (int frame = 0; frame < pNodeAnim->mNumPositionKeys; frame++) {
				XMFLOAT3 pos;
				pos.x = pNodeAnim->mPositionKeys[frame].mValue.x;
				pos.y = pNodeAnim->mPositionKeys[frame].mValue.y;
				pos.z = pNodeAnim->mPositionKeys[frame].mValue.z;
				(animData[a].channels[c].posDatas[frame])[pNodeAnim->mNodeName.C_Str()] = pos;
			}

			// 必要なら、サイズについても同様に付け足す

		}


		/*

		// 各頂点の座標変換（本来はシェーダがやるべき）
		for (unsigned int m = 0; m < pScene->mNumMeshes; m++)
		{
			for (auto& vertex : pDeformVertexs[m])
			{
				aiMatrix4x4 matrix[4];
				aiMatrix4x4 outMatrix;
				matrix[0] = Bones[vertex.boneName[0]].matrix;
				matrix[1] = Bones[vertex.boneName[1]].matrix;
				matrix[2] = Bones[vertex.boneName[2]].matrix;
				matrix[3] = Bones[vertex.boneName[3]].matrix;

				//ウェイトを考慮してマトリクス算出
				{
					outMatrix.a1 = matrix[0].a1 * vertex.boneWeight[0]
						+ matrix[1].a1 * vertex.boneWeight[1]
						+ matrix[2].a1 * vertex.boneWeight[2]
						+ matrix[3].a1 * vertex.boneWeight[3];

					outMatrix.a2 = matrix[0].a2 * vertex.boneWeight[0]
						+ matrix[1].a2 * vertex.boneWeight[1]
						+ matrix[2].a2 * vertex.boneWeight[2]
						+ matrix[3].a2 * vertex.boneWeight[3];

					outMatrix.a3 = matrix[0].a3 * vertex.boneWeight[0]
						+ matrix[1].a3 * vertex.boneWeight[1]
						+ matrix[2].a3 * vertex.boneWeight[2]
						+ matrix[3].a3 * vertex.boneWeight[3];

					outMatrix.a4 = matrix[0].a4 * vertex.boneWeight[0]
						+ matrix[1].a4 * vertex.boneWeight[1]
						+ matrix[2].a4 * vertex.boneWeight[2]
						+ matrix[3].a4 * vertex.boneWeight[3];



					outMatrix.b1 = matrix[0].b1 * vertex.boneWeight[0]
						+ matrix[1].b1 * vertex.boneWeight[1]
						+ matrix[2].b1 * vertex.boneWeight[2]
						+ matrix[3].b1 * vertex.boneWeight[3];

					outMatrix.b2 = matrix[0].b2 * vertex.boneWeight[0]
						+ matrix[1].b2 * vertex.boneWeight[1]
						+ matrix[2].b2 * vertex.boneWeight[2]
						+ matrix[3].b2 * vertex.boneWeight[3];

					outMatrix.b3 = matrix[0].b3 * vertex.boneWeight[0]
						+ matrix[1].b3 * vertex.boneWeight[1]
						+ matrix[2].b3 * vertex.boneWeight[2]
						+ matrix[3].b3 * vertex.boneWeight[3];

					outMatrix.b4 = matrix[0].b4 * vertex.boneWeight[0]
						+ matrix[1].b4 * vertex.boneWeight[1]
						+ matrix[2].b4 * vertex.boneWeight[2]
						+ matrix[3].b4 * vertex.boneWeight[3];



					outMatrix.c1 = matrix[0].c1 * vertex.boneWeight[0]
						+ matrix[1].c1 * vertex.boneWeight[1]
						+ matrix[2].c1 * vertex.boneWeight[2]
						+ matrix[3].c1 * vertex.boneWeight[3];

					outMatrix.c2 = matrix[0].c2 * vertex.boneWeight[0]
						+ matrix[1].c2 * vertex.boneWeight[1]
						+ matrix[2].c2 * vertex.boneWeight[2]
						+ matrix[3].c2 * vertex.boneWeight[3];

					outMatrix.c3 = matrix[0].c3 * vertex.boneWeight[0]
						+ matrix[1].c3 * vertex.boneWeight[1]
						+ matrix[2].c3 * vertex.boneWeight[2]
						+ matrix[3].c3 * vertex.boneWeight[3];

					outMatrix.c4 = matrix[0].c4 * vertex.boneWeight[0]
						+ matrix[1].c4 * vertex.boneWeight[1]
						+ matrix[2].c4 * vertex.boneWeight[2]
						+ matrix[3].c4 * vertex.boneWeight[3];



					outMatrix.d1 = matrix[0].d1 * vertex.boneWeight[0]
						+ matrix[1].d1 * vertex.boneWeight[1]
						+ matrix[2].d1 * vertex.boneWeight[2]
						+ matrix[3].d1 * vertex.boneWeight[3];

					outMatrix.d2 = matrix[0].d2 * vertex.boneWeight[0]
						+ matrix[1].d2 * vertex.boneWeight[1]
						+ matrix[2].d2 * vertex.boneWeight[2]
						+ matrix[3].d2 * vertex.boneWeight[3];

					outMatrix.d3 = matrix[0].d3 * vertex.boneWeight[0]
						+ matrix[1].d3 * vertex.boneWeight[1]
						+ matrix[2].d3 * vertex.boneWeight[2]
						+ matrix[3].d3 * vertex.boneWeight[3];

					outMatrix.d4 = matrix[0].d4 * vertex.boneWeight[0]
						+ matrix[1].d4 * vertex.boneWeight[1]
						+ matrix[2].d4 * vertex.boneWeight[2]
						+ matrix[3].d4 * vertex.boneWeight[3];

				}

				vertex.deformPosition = vertex.position;
				vertex.deformPosition *= outMatrix;


				//法線変換用に移動成分を削除
				outMatrix.a4 = 0.0f;
				outMatrix.b4 = 0.0f;
				outMatrix.c4 = 0.0f;

				vertex.deformNormal = vertex.normal;
				vertex.deformNormal *= outMatrix;
			}
		}

		*/

	}


	unsigned int Cnt = 0;

	for (int i = 0; i < pScene->mNumMeshes; i++) {
		for (int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {


			VERTEX_3D v;			
			
			v.Position = XMFLOAT3(pScene->mMeshes[i]->mVertices[j].x , pScene->mMeshes[i]->mVertices[j].y , pScene->mMeshes[i]->mVertices[j].z);
			v.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			v.Normal = XMFLOAT3(pScene->mMeshes[i]->mNormals[j].x, pScene->mMeshes[i]->mNormals[j].y, pScene->mMeshes[i]->mNormals[j].z);
			
			if (pScene->mMeshes[i]->mTextureCoords[0] != nullptr) {
				v.TexCoord = XMFLOAT2(pScene->mMeshes[i]->mTextureCoords[0][j].x, -pScene->mMeshes[i]->mTextureCoords[0][j].y);
			}
			else {
				v.TexCoord = XMFLOAT2(0.0f, 0.0f);
			}
			vertexBuf.push_back(v);


			// アニメーション用の頂点データにメッシュの初期値を代入し初期化
			DEFORM_VERTEX defVertex;
			defVertex.position.x = pScene->mMeshes[i]->mVertices[j].x;
			defVertex.position.y = pScene->mMeshes[i]->mVertices[j].y;
			defVertex.position.z = pScene->mMeshes[i]->mVertices[j].z;
			defVertex.deformPosition.x = pScene->mMeshes[i]->mVertices[j].x;
			defVertex.deformPosition.y = pScene->mMeshes[i]->mVertices[j].y;
			defVertex.deformPosition.z = pScene->mMeshes[i]->mVertices[j].z;
			defVertex.normal.x = pScene->mMeshes[i]->mNormals[j].x;
			defVertex.normal.y = pScene->mMeshes[i]->mNormals[j].y;
			defVertex.normal.z = pScene->mMeshes[i]->mNormals[j].z;
			defVertex.deformNormal.x = pScene->mMeshes[i]->mNormals[j].x;
			defVertex.deformNormal.y = pScene->mMeshes[i]->mNormals[j].y;
			defVertex.deformNormal.z = pScene->mMeshes[i]->mNormals[j].z;
			defVertex.boneNum = 0;
			for (auto b = 0; b < 4; b++) {
				// defVertex.boneIndex = 0;
				defVertex.boneName[b] = {};
				defVertex.boneWeight[b] = 0.0f;
			}
			pDeformVertexs[i].push_back(defVertex);
			Cnt++;
		}
		// アニメーション用ボーンデータをセット
		for (auto b = 0; b < pScene->mMeshes[i]->mNumBones; b++) {
			aiBone* pBone = pScene->mMeshes[i]->mBones[b];

			XMFLOAT4X4 matrixBuf = {
				pBone->mOffsetMatrix.a1,pBone->mOffsetMatrix.a2,pBone->mOffsetMatrix.a3,pBone->mOffsetMatrix.a4,
				pBone->mOffsetMatrix.b1,pBone->mOffsetMatrix.b2,pBone->mOffsetMatrix.b3,pBone->mOffsetMatrix.b4,
				pBone->mOffsetMatrix.c1,pBone->mOffsetMatrix.c2,pBone->mOffsetMatrix.c3,pBone->mOffsetMatrix.c4,
				pBone->mOffsetMatrix.d1,pBone->mOffsetMatrix.d2,pBone->mOffsetMatrix.d3,pBone->mOffsetMatrix.d4
			};
			Bones[pBone->mName.C_Str()].offsetMatrix = matrixBuf;

			for (auto w = 0; w < pBone->mNumWeights; w++) {
				aiVertexWeight weight = pBone->mWeights[w];



				pDeformVertexs[i][weight.mVertexId].boneWeight[pDeformVertexs[i][weight.mVertexId].boneNum] = weight.mWeight;
				pDeformVertexs[i][weight.mVertexId].boneName[pDeformVertexs[i][weight.mVertexId].boneNum] = pBone->mName.C_Str();
				pDeformVertexs[i][weight.mVertexId].boneNum++;
				if (pDeformVertexs[i][weight.mVertexId].boneNum > 4) {
					MessageBox(nullptr, "このモデルデータは正しく表示されない可能性があります", "警告", MB_OK);
				}
			}
		}
	}







	vertexList = new VERTEX_3D[Cnt];
	unsigned int n = 0;
	for (VERTEX_3D vertex : vertexBuf) {
		vertexList[n] = vertex;
		n++;
	}


	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * Cnt;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vertexList;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vbData, &vertexBuffer);

	//delete vertexList;




	// インデックス情報の登録
	
	std::list<unsigned short> indexBuf;
	meshList = new MESH[pScene->mNumMeshes];

	unsigned int StartCnt = 0;
	unsigned int offsetVertex = 0;
	unsigned short indexIns;
	for (int i = 0; i < pScene->mNumMeshes; i++) {
		unsigned int size = 0;
		meshList[i].indexOffset = StartCnt;
		meshList[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;

		for (int j = 0; j < pScene->mMeshes[i]->mNumFaces; j++) {
			for (int k = 0; k < pScene->mMeshes[i]->mFaces[j].mNumIndices; k++) {
				indexIns = pScene->mMeshes[i]->mFaces[j].mIndices[k];
				indexIns += offsetVertex;
				indexBuf.push_back(indexIns);
				size++;
			}
		}
		meshList[i].indexNum = size;
		offsetVertex += pScene->mMeshes[i]->mNumVertices;
		StartCnt += size;
	}

	indexList = new unsigned short[indexBuf.size()];
	unsigned int x = 0;
	for (unsigned short index : indexBuf) {
		indexList[x] = index;
		x++;
	}

	
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * indexBuf.size();
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = indexList;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufferDesc, &ibData, &indexBuffer);

	//delete indexList;


	

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = NULL;
	device->CreateSamplerState(&samplerDesc, &samplerState);

	context->PSSetSamplers(0, 1, &samplerState);
	
	aiReleaseImport(pScene);
}

void Model::Uninit()
{
	DeleteNode(rootNode);
	vertexBuffer->Release();
	indexBuffer->Release();
	if (animNum > 0) {
		for (int i = 0; i < animNum; i++) {
			for (int j = 0; j < animChannels[i]; j++) {
				delete[] animData[i].channels[j].posDatas;
				delete[] animData[i].channels[j].rotDatas;
			}
			delete[] animData[i].channels;
		}
		delete[] animData;
		delete[] animChannels;
	}
	delete[] pDeformVertexs;
	delete[] meshList;
	delete[] vertexList;
	
	for (int i = 0; i < materialNum; i++) {
		if (materialList[i]) {
			delete materialList[i];
		}
	}
	delete[] materialList;


	
	for (int i = 0; i < textureNum; i++) {
		if (textureList[i]) {
			textureList[i]->UnLoad();
			delete textureList[i];
		}
	}
	delete[] textureList;
	

	delete shader;

}

void Model::CreateBone(NODE* node)
{
	BONE bone = {};
	Bones[node->nodeName] = bone;	// ノードの名前をボーン検索名にする = ボーン名
}

void Model::UpdateBoneMatrix(NODE* node, XMFLOAT4X4* defMat)
{
	BONE* pBone = &Bones[node->nodeName];
	
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 resultMatrix;
	XMMATRIX mat = XMLoadFloat4x4(defMat) * XMLoadFloat4x4(&pBone->animationMatrix);
	XMStoreFloat4x4(&resultMatrix, mat);

	XMMATRIX offsetMat = XMLoadFloat4x4(&pBone->offsetMatrix);
	mat *= offsetMat;
	XMStoreFloat4x4(&pBone->matrix, mat);

	for (int n = 0; n < node->childNum; n++) {
		UpdateBoneMatrix(node->childNode[n], &resultMatrix);
	}
}

NODE* Model::RegisterNode(aiNode* pNode)
{
	NODE* node = new NODE;
	node->nodeName = pNode->mName.C_Str();
	node->meshNum = pNode->mNumMeshes;
	if (node->meshNum != 0) {
		node->meshIndex = new unsigned int[node->meshNum];
	}	
	memcpy(node->meshIndex, pNode->mMeshes, sizeof(unsigned int) * node->meshNum);
	node->childNum = pNode->mNumChildren;
	node->offsetMatrix = { 
		pNode->mTransformation.a1, pNode->mTransformation.a2,pNode->mTransformation.a3, pNode->mTransformation.a4,
		pNode->mTransformation.b1, pNode->mTransformation.b2,pNode->mTransformation.b3, pNode->mTransformation.b4,
		pNode->mTransformation.c1, pNode->mTransformation.c2,pNode->mTransformation.c3, pNode->mTransformation.c4,
		pNode->mTransformation.d1, pNode->mTransformation.d2,pNode->mTransformation.d3, pNode->mTransformation.d4
	};
	node->childNode = new NODE*[node->childNum];
	for (int i = 0; i < node->childNum; i++) {
		node->childNode[i] = RegisterNode(pNode->mChildren[i]);
		node->childNode[i]->parentNode = node;
	}
	CreateBone(node);
	return node;
}

void Model::DeleteNode(NODE* node)
{
	if (node->childNum != 0) {
		for (int i = 0; i < node->childNum; i++) {
			DeleteNode(node->childNode[i]);
		}
	}
	if (node->meshNum != 0) {
		delete[] node->meshIndex;
	}
	delete[] node->childNode;
	delete node;
	
}

void Model::DrawNode(NODE* node, XMMATRIX mat)
{
	XMMATRIX localMat = XMLoadFloat4x4(&node->offsetMatrix);
	
	localMat = XMMatrixTranspose(localMat);
	localMat = localMat * mat;
	

	for (int i = 0; i < node->childNum; i++) {
		DrawNode(node->childNode[i], localMat);
	}

	
	XMFLOAT4X4 arg;
	XMStoreFloat4x4(&arg, localMat);
	shader->SetWorldMatrix(&arg);
	
	

	shader->Set();

	

	for (int i = 0; i < node->meshNum; i++) {
		shader->SetMaterial(materialList[meshList[node->meshIndex[i]].materialIndex]);
		if (textureList[meshList[node->meshIndex[i]].materialIndex] != nullptr) {
			shader->SetTexture(textureList[meshList[node->meshIndex[i]].materialIndex]);
			//Manager::Get()->GetDXManager()->SetDepthTexture(0);
		}
		context->DrawIndexed(meshList[node->meshIndex[i]].indexNum, meshList[node->meshIndex[i]].indexOffset, 0);
	}

	
}


#include "ModelNormal.h"

#include "Scene.h"
#include <Windows.h>
#include <string>
#include <list>



ModelNormal::ModelNormal()
{
	manager = Manager::Get();
	// DirectXManagerの取得
	if (Manager::Get()->GetDXManager() == nullptr) {
		MessageBox(NULL, "DirectXマネージャの取得に失敗しました", "ModelNormalローダー", MB_OK | MB_ICONHAND);
		exit(1);
	}
	device = Manager::Get()->GetDXManager()->GetDevice();
	context = Manager::Get()->GetDXManager()->GetDeviceContext();
	shader = new Shader3D_Normal;
	shader->Init();
}


ModelNormal::~ModelNormal()
{

}

void ModelNormal::Draw()
{
	NODE* curNode = rootNode;
	XMFLOAT4X4 MatLoc = owner->GetTransformMatrix();

	UINT stride = sizeof(VERTEX_3D_Normal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->SetViewMatrix(manager->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(manager->GetScene()->GetProjectionMatrix());

	manager->GetDXManager()->SetDepthEnable(true);

	XMMATRIX mat = XMLoadFloat4x4(&MatLoc);
	shader->SetTextureNormal(Normal);
	//mat = XMMatrixTranspose(mat);
	DrawNode(rootNode, mat);

}

void ModelNormal::Load(const char* filename)
{

	VERTEX_3D_Normal v[4];
	for (int i = 0; i < 4; i++) {
		v[i].Position = XMFLOAT3(-5.0f + (i % 2), 0.0f, 5.0f - (i / 2));
		v[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		v[i].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		v[i].binormal = XMFLOAT3(1.0f, 0.0f, 0.0f);
		v[i].tangent = XMFLOAT3(0.0f, 0.0f, 1.0f);
	}
	
	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D_Normal) * 4;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = v;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vbData, &vertexBuffer);







	/*
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

	}

	// aiMaterialの読み込み->シェーダが読める形に変換
	materialList = new MATERIAL*[pScene->mNumMaterials];
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

	}


	// 頂点情報の登録
	std::list<VERTEX_3D_Normal> vertexBuf;

	unsigned int Cnt = 0;

	for (int i = 0; i < pScene->mNumMeshes; i++) {
		for (int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {


			VERTEX_3D_Normal v;


			v.Position = XMFLOAT3(pScene->mMeshes[i]->mVertices[j].x, pScene->mMeshes[i]->mVertices[j].y, pScene->mMeshes[i]->mVertices[j].z);
			v.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			v.Normal = XMFLOAT3(pScene->mMeshes[i]->mNormals[j].x, pScene->mMeshes[i]->mNormals[j].y, pScene->mMeshes[i]->mNormals[j].z);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR vec = XMLoadFloat3(&v.Normal);
			XMVECTOR bufVec = XMVector3Cross(up, vec);
			XMStoreFloat3(&v.tangent, bufVec);
			bufVec = XMVector3Cross(bufVec, vec);
			XMStoreFloat3(&v.binormal, bufVec);

			if (pScene->mMeshes[i]->mTextureCoords[0] != nullptr) {
				v.TexCoord = XMFLOAT2(pScene->mMeshes[i]->mTextureCoords[0][j].x, pScene->mMeshes[i]->mTextureCoords[0][j].y);
			}


			else {
				v.TexCoord = XMFLOAT2(0.0f, 0.0f);
			}
			vertexBuf.push_back(v);
			Cnt++;
		}
	}

	vertexList = new VERTEX_3D_Normal[Cnt];
	unsigned int n = 0;
	for (VERTEX_3D_Normal vertex : vertexBuf) {
		vertexList[n] = vertex;
		n++;
	}


	// 頂点バッファの作製
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D_Normal) * Cnt;
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

	for (int i = 0; i < pScene->mNumMeshes; i++) {
		unsigned int size = 0;
		meshList[i].indexOffset = StartCnt;
		meshList[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;

		for (int j = 0; j < pScene->mMeshes[i]->mNumFaces; j++) {
			for (int k = 0; k < pScene->mMeshes[i]->mFaces[j].mNumIndices; k++) {

				indexBuf.push_back((unsigned short)pScene->mMeshes[i]->mFaces[j].mIndices[k]);
				size++;
			}
		}
		meshList[i].indexNum = size;
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


	// ノード情報の登録
	aiNode* pNode = pScene->mRootNode;
	rootNode = RegisterNode(pNode);

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
	Normal = new Texture;
	Normal->Load("Assets/Textures/t_floorNormal001.png");
	*/
}

void ModelNormal::Uninit()
{
	delete Normal;
	DeleteNode(rootNode);
	vertexBuffer->Release();
	indexBuffer->Release();
	delete[] meshList;
	delete[] vertexList;

	for (int i = 0; i < sizeof(materialList); i++) {
		/*if (materialList[i]) {
			delete materialList[i];
		}*/
	}
	delete[] materialList;


	if (textureList != nullptr) {
		for (int i = 0; i < sizeof(textureList); i++) {
			if (textureList[i]) {
				textureList[i]->UnLoad();
				delete textureList[i];
			}
		}
		delete[] textureList;
	}

	delete shader;

}

NODE* ModelNormal::RegisterNode(aiNode* pNode)
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
	return node;
}

void ModelNormal::DeleteNode(NODE* node)
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

void ModelNormal::DrawNode(NODE* node, XMMATRIX mat)
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
		}
		context->DrawIndexed(meshList[node->meshIndex[i]].indexNum, meshList[node->meshIndex[i]].indexOffset, 0);
	}


}


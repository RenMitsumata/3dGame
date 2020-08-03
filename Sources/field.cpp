#include "Manager.h"
#include "DXManager.h"
#include "GameObject.h"
#include "main.h"
#include "Scene.h"
#include "field.h"
#include "texture.h"




void Field::Init()
{
	shader = new Shader3D_Normal;
	shader->Init();


	for (int z = 0; z < FIELD_Z; z++)
	{
		for (int x = 0; x < FIELD_X; x++)
		{
			m_Vertex[z * FIELD_X + x].Position.x = x - FIELD_X / 2;
			m_Vertex[z * FIELD_X + x].Position.z = -z + FIELD_Z / 2;
			m_Vertex[z * FIELD_X + x].Position.y = 0.0f;
			m_Vertex[z * FIELD_X + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Vertex[z * FIELD_X + x].TexCoord = XMFLOAT2(x, z);
			m_Vertex[z * FIELD_X + x].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_Vertex[z * FIELD_X + x].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			m_Vertex[z * FIELD_X + x].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}

	for (int z = 1; z < FIELD_Z - 1; z++)
	{
		for (int x = 1; x < FIELD_X - 1; x++)
		{
			XMFLOAT3 va, vb, n;
			float len;

			va.x = m_Vertex[(z - 1) * FIELD_X + x].Position.x - m_Vertex[(z + 1) * FIELD_X + x].Position.x;
			va.y = m_Vertex[(z - 1) * FIELD_X + x].Position.y - m_Vertex[(z + 1) * FIELD_X + x].Position.y;
			va.z = m_Vertex[(z - 1) * FIELD_X + x].Position.z - m_Vertex[(z + 1) * FIELD_X + x].Position.z;

			vb.x = m_Vertex[z * FIELD_X + (x + 1)].Position.x - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.x;
			vb.y = m_Vertex[z * FIELD_X + (x + 1)].Position.y - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.y;
			vb.z = m_Vertex[z * FIELD_X + (x + 1)].Position.z - m_Vertex[(z - 1) * FIELD_X + (x - 1)].Position.z;

			n.x = va.y * vb.z - va.z * vb.y;
			n.y = va.z * vb.x - va.x * vb.z;
			n.z = va.x * vb.y - va.y * vb.x;

			len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);

			n.x /= len;
			n.y /= len;
			n.z /= len;

			m_Vertex[z * FIELD_X + x].Normal = n;
		}
	}

	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D_Normal) * FIELD_X * FIELD_Z;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;
		Manager::Get()->GetDXManager()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	
	}


	unsigned short index[(FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2];

	unsigned short i = 0;
	for (int z = 0; z < FIELD_Z - 1; z++)
	{
		for (int x = 0; x < FIELD_X; x++)
		{
			index[i] = (z + 1) * FIELD_X + x;
			i++;
			index[i] = z * FIELD_X + x;
			i++;
		}

		if (z == FIELD_Z - 2)
			break;

		index[i] = z * FIELD_X + FIELD_X - 1;
		i++;
		index[i] = (z + 2) * FIELD_X;
		i++;
	}


	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short) * ((FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Manager::Get()->GetDXManager()->GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}



	
	m_Texture = new Texture();
	m_Texture->Load("Assets/wood.png");
	
	
	m_Normal = new Texture;
	m_Normal->Load("Assets/tile.png");
	

	pos = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	rot = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	siz = XMFLOAT3( 1.0f, 1.0f, 1.0f );

}


void Field::Uninit()
{
	shader->Uninit();
	delete shader;

	m_VertexBuffer->Release();


	m_Normal->UnLoad();
	delete m_Normal;
	m_Texture->UnLoad();
	delete m_Texture;

}


void Field::Update()
{


}


void Field::Draw()
{

	// 頂点バッファ設定
	UINT stride = sizeof( VERTEX_3D_Normal );
	UINT offset = 0;
	Manager::Get()->GetDXManager()->GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );

	// インデックスバッファ設定
	Manager::Get()->GetDXManager()->GetDeviceContext()->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );


	// テクスチャ設定
	shader->SetTexture( m_Texture );

	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling( siz.x, siz.y, siz.z );
	world *= XMMatrixRotationRollPitchYaw( rot.x, rot.y, rot.z );
	world *= XMMatrixTranslation( pos.x, pos.y, pos.z );
	XMFLOAT4X4 arg;
	XMStoreFloat4x4(&arg, world);
	shader->SetWorldMatrix(&arg);

	
	shader->SetViewMatrix(Manager::Get()->GetScene()->GetViewMatrix());
	shader->SetProjMatrix(Manager::Get()->GetScene()->GetProjectionMatrix());

	shader->SetTextureNormal(m_Normal);


	shader->Set();


	// プリミティブトポロジ設定
	Manager::Get()->GetDXManager()->GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );


	// ポリゴン描画
	Manager::Get()->GetDXManager()->GetDeviceContext()->DrawIndexed(((FIELD_X * 2 + 2) * (FIELD_Z - 1) - 2), 0, 0 );

}



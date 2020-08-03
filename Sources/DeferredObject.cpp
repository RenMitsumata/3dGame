#include "DeferredObject.h"
#include "DeferredPolygon.h"
#include "Manager.h"
#include "DXManager.h"
#include "Component.h"

#include "Texture.h"

DeferredObject::DeferredObject()
{
}


DeferredObject::~DeferredObject()
{
}

void DeferredObject::Init()
{
}

void DeferredObject::SetTextureAndSize(const char * filename, XMFLOAT3 pos, float width, float height)
{
	polygon = ComponentFactory::CreateComponent<DeferredPolygon>();
	//if (filename != nullptr) {
	//	polygon->SetTexture(filename);
	//}
	polygon->SetSize(width, height, pos);
	componentsList.push_back(polygon);
}

void DeferredObject::SetDeferredTexture(ID3D11ShaderResourceView * srv)
{
	polygon->SetDeferredTexture(srv);
}


#include "UI.h"
#include "Polygon.h"

#include "Manager.h"
#include "DXManager.h"
#include "Component.h"

#include "Texture.h"


UI::UI()
{
}


UI::~UI()
{
}

void UI::Init()
{
	
}

void UI::SetTextureAndSize(const char * filename, XMFLOAT3 pos, float width, float height)
{
	polygon = ComponentFactory::CreateComponent<class Polygon>();
	if (filename != nullptr) {
		polygon->SetTexture(filename);
	}
	polygon->SetSize(width, height, pos);
	componentsList.push_back(polygon);
}

void UI::ChangeColor()
{
	polygon->ChangeColor();
}

void UI::SetDeferredTexture(ID3D11ShaderResourceView* srv)
{
	polygon->SetDeferredTexture(srv);
}

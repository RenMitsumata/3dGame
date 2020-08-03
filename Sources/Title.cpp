#include "Title.h"
#include "Manager.h"
#include "InputManager.h"
#include "UI.h"
#include "Game.h"


Title::Title()
{
}


Title::~Title()
{
	if (albedoPolygon) {
		albedoPolygon->Uninit();
		delete albedoPolygon;
	}
}

void Title::Init()
{
	UI* ui = AddGameObject<UI>(e_LAYER_UI);
	ui->SetTextureAndSize("Assets/Textures/t_title002.png");
	input = Manager::Get()->GetInput();
	dxManager = Manager::Get()->GetDXManager();
	albedoPolygon = new UI;
	albedoPolygon->SetTextureAndSize(nullptr, XMFLOAT3(0.0f, 0.0f, 0.5f));
}

void Title::Update()
{
	if (input->GetKeyTrigger(VK_SPACE) || input->GetPadTrigger(XINPUT_GAMEPAD_A)) {
		Manager::Get()->ChangeScene(new Game);
	}
}

void Title::DrawDeferred()
{
	ID3D11ShaderResourceView* srv[1];
	srv[0] = dxManager->GetSRV(0);
	if (srv[0] == nullptr) {
		return;
	}
	albedoPolygon->SetDeferredTexture(srv[0]);
	albedoPolygon->Draw();

}

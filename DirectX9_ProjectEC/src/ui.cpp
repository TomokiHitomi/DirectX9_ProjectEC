//=============================================================================
//
// ui処理 [ui.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "ui.h"
#include "scene.h"
#include "titleui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
UiManager::UiManager(void)
{
	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::UI, Priority::Lowest, Priority::Ui);

	pUi = NULL;

	switch (SceneManager::GetScene())
	{
	case SceneManager::TITLE:
		pUi = new TitleUi();
		break;
	case SceneManager::GAME:
		break;
	case SceneManager::RESULT:
		break;

	}
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
UiManager::~UiManager(void)
{
	SAFE_RELEASE(pUi);
	SAFE_DELETE(pUi);
}

//=============================================================================
// 更新処理
//=============================================================================
void UiManager::Update(void)
{
#ifdef _DEBUG
	// imguiの更新開始
	ImGui::Begin("Ui");
	//ImGui::Text("test\n");
	//if (ImGui::Button("WaveAdd"));
#endif

	SAFE_UPDATE(pUi);

#ifdef _DEBUG
	// imguiの更新終了
	ImGui::End();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void UiManager::Draw(void)
{
	SAFE_DRAW(pUi);
}
//=============================================================================
//
// タイトルUI処理 [titleui.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
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
TitleUi::TitleUi(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pLogo = NULL;
	pLogo = new TitleLogo();
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
TitleUi::~TitleUi(void)
{
	Release();
}

//=============================================================================
// 解放処理
//=============================================================================
void TitleUi::Release(void)
{
	SAFE_RELEASE(pLogo);
	SAFE_DELETE(pLogo);
}


//=============================================================================
// 更新処理
//=============================================================================
void TitleUi::Update(void)
{
	SAFE_UPDATE(pLogo);
}

//=============================================================================
// 描画処理
//=============================================================================
void TitleUi::Draw(void)
{
	SAFE_DRAW(pLogo);
}
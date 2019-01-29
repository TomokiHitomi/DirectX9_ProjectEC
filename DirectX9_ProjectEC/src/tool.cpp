//=============================================================================
//
// ツール処理 [tool.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "tool.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
D3DXVECTOR4 Color::xColor[COLOR_PALLET_MAX];

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Color g_cColor;

//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPallet(int nColor)
{
	D3DXCOLOR temp;
	//temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	switch (nColor)
	{
	case COLOR_PALLET_WHITE:		// ホワイト
		temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_RED:			// レッド
		temp = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_YELLOW:		// イエロー
		temp = D3DXCOLOR(0.9f, 0.9f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_GREEN:		// グリーン
		temp = D3DXCOLOR(0.0f, 1.0f, 0.05f, 1.0f);
		break;
	case COLOR_PALLET_CYAN:			// シアン
		temp = D3DXCOLOR(0.0f, 0.9f, 0.9f, 1.0f);
		break;
	case COLOR_PALLET_BLUE:			// ブルー
		temp = D3DXCOLOR(0.1f, 0.1f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_MAGENTA:		// マゼンダ
		temp = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_BROWN:		// ブラウン
		temp = D3DXCOLOR(0.6f, 0.4f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTGREEN:	// ライトグリーン
		temp = D3DXCOLOR(0.3f, 0.9f, 0.3f, 1.0f);
		break;
	case COLOR_PALLET_ORANGE:		// オレンジ
		temp = D3DXCOLOR(1.0f, 0.3f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_BLACK:		// ブラック
		temp = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTBLUE:	// ブルー
		temp = D3DXCOLOR(0.4f, 0.7f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_GRAY:			// グレイ
		temp = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		break;
	}
	return temp;
}

//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPalletRandom(void)
{
	return SetColorPallet(rand() % COLOR_PALLET_MAX);
}

//=============================================================================
// コンストラクタ
//=============================================================================
Color::Color(void)
{
	for (UINT i = 0; i < COLOR_PALLET_MAX; i++)
	{
		xColor[i] = (D3DXVECTOR4)SetColorPallet(i);
	}
}

//=============================================================================
// 関数名：
// 引数  ：
// 戻り値：
// 説明  ：
//=============================================================================

//=============================================================================
//
// ツール処理 [tool.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _TOOL_H_
#define _TOOL_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 列挙型定義
//*****************************************************************************
enum
{
	COLOR_PALLET_WHITE,
	COLOR_PALLET_RED,
	COLOR_PALLET_YELLOW,
	COLOR_PALLET_GREEN,
	COLOR_PALLET_CYAN,
	COLOR_PALLET_BLUE,
	COLOR_PALLET_MAGENTA,
	COLOR_PALLET_BROWN,
	COLOR_PALLET_LIGHTGREEN,
	COLOR_PALLET_ORANGE,
	COLOR_PALLET_BLACK,
	COLOR_PALLET_LIGHTBLUE,
	COLOR_PALLET_GRAY,
	COLOR_PALLET_MAX
};

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Color
{
public:
	static D3DXVECTOR4 xColor[COLOR_PALLET_MAX];
	Color();
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
D3DXCOLOR SetColorPallet(int nColor);
D3DXCOLOR SetColorPalletRandom(void);

#endif

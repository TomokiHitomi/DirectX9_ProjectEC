//=============================================================================
//
// タイトルUI処理 [titleui.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _TITLEUI_H_
#define _TITLEUI_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"
#include "ui.h"
#include "titlelogo.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class TitleUi : public BaseUi
{
public:
	TitleLogo* pLogo;

	TitleUi();
	~TitleUi();

	void Update(void);
	void Draw(void);
	void Release(void);
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

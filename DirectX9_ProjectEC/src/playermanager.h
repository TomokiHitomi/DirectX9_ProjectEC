//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _PLAYERMANAGER_H_
#define _PLAYERMANAGER_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "object.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class PlayerManager : public ObjectManager
{
public:
	enum PLAYER
	{	// プレイヤー管理
		PLAYER_1P,
		PLAYER_MAX
	};
	static Player		*m_pPlayer[PLAYER_MAX];

	// コンストラクタ（初期化処理）
	PlayerManager(void);
	//デストラクタ（終了処理）
	~PlayerManager(void);

	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);

	static void Reset(void);
	static Player *GetPlayer(PLAYER player) { return m_pPlayer[player]; }
	static D3DXVECTOR3 GetPos(PLAYER player) { return m_pPlayer[player]->GetPos(); }
	static D3DXMATRIX GetMtx(PLAYER player) { return m_pPlayer[player]->GetMtx(); }
	static Player::MODE GetMode(PLAYER player) { return m_pPlayer[player]->GetMode(); }
	static D3DXVECTOR3 GetVecX(PLAYER player) { return m_pPlayer[player]->GetVecX(); }
	static D3DXVECTOR3 GetVecY(PLAYER player) { return m_pPlayer[player]->GetVecY(); }
	static D3DXVECTOR3 GetVecZ(PLAYER player) { return m_pPlayer[player]->GetVecZ(); }
	static D3DXVECTOR3 GetPosTaget(PLAYER player) { return m_pPlayer[player]->GetPosTaget(); }
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

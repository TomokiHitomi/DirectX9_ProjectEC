//=============================================================================
//
// ウィング処理 [wing.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _WING_H_
#define _WING_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "SkinMeshX.h"
#include "wing_feather.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデル
#define WING_MODEL				("data/model/wing/wing.X")

#define WING_POS				(D3DXVECTOR3(0.0f, 0.0f, 2.0f))
#define WING_ROT				(D3DXVECTOR3(2.1f, 0.0f, 0.0f))
#define WING_SCL				(20.0f)

// アニメーションスピード
#define WING_ANIM_SPEED_DEF		(60.0f / 4800.0f)

// アニメーションのブレンドウェイト
#define WING_ANIM_WEIGHT_DEF	(0.1f)

#define WING_BONE_MAX			(12)

// アニメーションフラグ
#define WING_ANIM_FRY			(0x00000001)
#define WING_ANIM_FLOAT			(0x00000002)
#define WING_ANIM_BACK			(0x00000004)
#define WING_ANIM_CLOSE			(0x00000008)
//#define WING_ANIM_	0x00000010
//#define WING_ANIM_	0x00000020
//#define WING_ANIM_	0x00000040
//#define WING_ANIM_	0x00000080
//#define WING_ANIM_	0x00000100
//#define WING_ANIM_	0x00000200
//#define WING_ANIM_	0x00001000
//#define WING_ANIM_	0x00002000

// SE
#define WING_SE_FLAG_MARGIN		(0.1f)
#define WING_SE_FLOAT			(0.5f)
#define WING_SE_FLY1			(0.4f)
#define WING_SE_FLY2			(1.2f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Wing
{
private:
	CSkinMesh*		m_CSkinMesh;					// スキンメッシュ格納用
	D3DXMATRIX		m_mtxWorld;						// ワールドマトリクス
	D3DXMATRIX*		m_pSetMtx;						// 設置先マトリクス
	D3DXMATRIX*		m_pBoneMtxArray[WING_BONE_MAX];	// ボーンマトリクス

	bool			m_bUse;							// 使用フラグ
	DWORD			m_dwAnim;

	WingFeather		m_cWingFeader;
	int				m_nWingFeaderCount;
	int				m_nWingFeaderTime;

	bool			m_bSe;

public:
	// コンストラクタ（初期化処理）
	Wing(void);
	//デストラクタ（解放処理）
	~Wing(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);

	// アニメーション設定処理
	void SetAnim(DWORD dwFlag) { m_dwAnim |= dwFlag; }
	void SetMtx(D3DXMATRIX* pSetMtx) { m_pSetMtx = pSetMtx; }
private:
	enum WingAnim
	{	// アニメーションセットは追加の逆順
		WING_CLOSE,
		WING_BACK,
		WING_FLOAT,
		WING_FLY,
		WING_ANIM_MAX
	};
	void ChangeAnim(DWORD dwAnime, FLOAT fShift)	{ if (m_CSkinMesh != NULL) m_CSkinMesh->ChangeAnim(dwAnime, fShift); }
	void ChangeAnimSpeed(FLOAT AnimSpeed)			{ if (m_CSkinMesh != NULL) m_CSkinMesh->SetAnimSpeed(AnimSpeed); }
	void CheckAnim(void);

	// Se設定処理
	void Se(void);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

//=============================================================================
//
// ソード処理 [sword.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _SWORD_H_
#define _SWORD_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "object.h"
#include "XModel.h"
#include "debugobject.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデル
//#define SWORD_MODEL					("data/model/sword/Espada.x")
#define SWORD_MODEL					("data/model/sword/Anelace.x")
#define SWORD_MODEL_TEX				("Espada_AlbedoTransparency.png")

#define SWORD_MAX					(50)
#define SWORD_MAX_INIT				(8)

#define SWORD_SCL					(3.0f)
#define SWORD_POS_Y					(20.0f)
#define SWORD_POS_Y_FLY				(8.0f)
#define SWORD_POS_Y_GUARD			(35.0f)

#define SWORD_SPEED_MIN				(8.0f)
#define SWORD_SPEED_MAX				(20.0f)
#define SWORD_SPEED_RETRUN			(20.0f)
#define SWORD_SPEED_ADD				(5)


//#define SWORD_ROT_SPEED			(0.02f)
#define SWORD_ROT_SPEED_MIN			(-0.1f)
#define SWORD_ROT_SPEED_COUNT		(30)
#define SWORD_ROT_SPEED_MAX			(0.02f)
#define SWORD_ROT_SPEED_ADD			(20)
#define SWORD_ROT_SPEED_SUB			(3)

#define SWORD_ROT_LENGTH			(20.0f)
#define SWORD_ROT_LENGTH_FLY		(30.0f)
#define SWORD_ROT_ADJUST_TIME		(15)

#define SWORD_TRUN					(0.1f)
#define SWORD_LENGTH_RETRUN			(200.0f)
#define SWORD_LENGTH_RETRUN_SUB		(3.5f)

// 剣の向きと展開半径の変化スピード
#define SWORD_MORPH_SPEED			(0.05f)

#define SWORD_HAND_ROT				(D3DXVECTOR3(1.5f,0.9f,0.0f))

#define SWORD_COLLISION_SIZE		(10.0f)
#define SWORD_COLLISION_POS			(10)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Sword
{
public:

	// コンストラクタ（初期化処理）
	Sword(void);
	//デストラクタ（終了処理）
	~Sword(void);

	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);
	void SetMtx(D3DXMATRIX* pSetMtx) { m_pSetMtx = pSetMtx; }
private:
	CXModel			*m_CXModel;			// モデル格納用
	class SwordData
	{
	public:
		enum Mode
		{
			STAY,
			SHOT,
			RETURN
		};
		D3DXVECTOR3			vPos;			// 座標情報

		// ローカルベクトル
		D3DXVECTOR3			vX;
		D3DXVECTOR3			vY;
		D3DXVECTOR3			vZ;
		SwordData::Mode		eMode;
		float				fSpeed;
		float				fLength;
		float				fAngle;
		bool				bUse;
#ifdef _DEBUG
		DebugObjectData m_cDebug;
#endif
	};

	enum Mode
	{
		SHOT,
		ROT,
		RETURN
	};
	Sword::Mode		m_eMode;

	// 剣の情報：[SWORD_MAX]は持ち剣用
	SwordData		m_cSword[SWORD_MAX + 1];
	D3DXMATRIX		m_mtxSword;
	D3DXMATRIX		*m_pSetMtx;			// 設置先マトリクス
	D3DXVECTOR3		m_vScl;				// 拡縮情報
	int				m_nMax;


	int				m_nCnt;
	int				m_nCntMax;
	int				m_nCntRot;

	float			m_fAngle;
	float			m_fRot;
	float			m_fRotSpeed;

	// モーフ
	float			m_fMorph;
	float			m_fMorphGuard;

	bool			m_bUse;

	void	Stay(void);
	void	StayAll(void);
	void	Rot(void);
public:
	void	Shot(void);
	void	RetrunAll(void);
	void	Add(void);
	void	Sub(void);
};

	//Sword	m_cSword;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

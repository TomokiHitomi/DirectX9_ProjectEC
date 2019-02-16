//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "SkinMeshX.h"
#include "sword.h"
#include "wing.h"
#include "effect.h"

// デバッグ用
#ifdef _DEBUG
#include "debugobject.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデル
#define PLAYER_MODEL						("data/model/haku/haku.X")

#define PLAYER_POS							(D3DXVECTOR3(0.0f,250.0f,500.0f))
#define PLAYER_ROT							(D3DXVECTOR3(D3DX_PI / 2, 0.0f, 0.0f))
#define PLAYER_SCL							(20.0f)

#define PLAYER_HEIGHT						(30.0f)

#define PLAYER_COLLISION_Y					(20.0f)
#define PLAYER_COLLISION_POS				(D3DXVECTOR3(0.0f, PLAYER_COLLISION_Y, 0.0f))
#define PLAYER_COLLISION_SIZE				(30.0f)

// モデルのボーン名
#define PLAYER_MODEL_BONE_WING				("No_12_joint_Torso2")
#define PLAYER_MODEL_BONE_RIGHT_HAND		("No_59_i_joint_RightDummy")

// 移動スピード
#define PLAYER_MOVE_SPEED					(6.0f)
#define PLAYER_MOVE_SPEED_CHANGE			(0.05f)
#define PLAYER_MOVE_SPEED_MAX				(10.0f)
#define PLAYER_MOVE_SPEED_MIN				(3.0f)

#define PLAYER_RISE_SPEED					(1.2f)
#define PLAYER_RISE_SPEED_AUTO				(0.2f)
#define PLAYER_RISE_SPEED_MAX				(15.0f)

#define PLAYER_ROT_SPEED_X					(0.0005f)
#define PLAYER_ROT_SPEED_Z					(0.0015f)

#define PLAYER_ROT_SPEED_MAX_X				(0.02f)
#define PLAYER_ROT_SPEED_MAX_Z				(0.03f)


#define PLAYER_MOVE_INERTIA					(0.03f)
#define PLAYER_ROT_INERTIA					(0.3f)

#define PLAYER_ALPHA_TEST					(150)

// 移動制限
#define PLAYER_HEIGHT_MAX					(4500.0f)
#define PLAYER_HEIGHT_MIN					(0.0f)

/***** FloatMode *****/
#define PLAYER_FLOAT_ROT_X					(D3DX_PI * 0.5f)
#define PLAYER_FLOAT_ROT_Y					(D3DX_PI)

/***** FlyMode *****/

/***** ChangeMode *****/
#define PLAYER_CHANGE_FRAME					(10)

// テスト用
#define PLAYER_GYRO_MARGIN					(300.0f)

// アニメーションフラグ
#define PLAYER_ANIM_FRY						(0x00000001)
#define PLAYER_ANIM_FLOAT					(0x00000002)
#define PLAYER_ANIM_ATK_HORIZON				(0x00000004)
#define PLAYER_ANIM_ATK_THRUST				(0x00000008)
#define PLAYER_ANIM_ATK_ROUNDUP				(0x00000010)
#define PLAYER_ANIM_ATK_RICARD				(0x00000020)
#define PLAYER_ANIM_GUARD					(0x00000040)
#define PLAYER_ANIM_STEP_FLONT				(0x00000080)
#define PLAYER_ANIM_STEP_BACK				(0x00000100)
#define PLAYER_ANIM_STEP_LEFT				(0x00000200)
#define PLAYER_ANIM_STEP_RIGHT				(0x00001000)
//#define PLAYER_ANIM_	(0x00002000)
#define PLAYER_ANIM_DUMMY					(0x80000000)

#define PLAYER_ANIM_SPEED_DEF				(60.0f / 4800.0f)	// アニメーションスピード
#define PLAYER_ANIM_WEIGHT_DEF				(0.1f)				// アニメーションのブレンドウェイト
#define PLAYER_ANIM_WEIGHT_ATK				(0.3f)				// アニメーションのブレンドウェイト

// アタック
#define PLAYER_ATK_COMBOTIME_HORIZON		(12 * 2)
#define PLAYER_ATK_COMBOTIME_THRUST			(9 * 2)
#define PLAYER_ATK_COMBOTIME_ROUNDUP		(25 * 2)
#define PLAYER_ATK_COMBO_MAX				(3)

#define PLAYER_ATK_VOICE_COMBOTIME_HORIZON	(1)
#define PLAYER_ATK_VOICE_COMBOTIME_THRUST	(30)
#define PLAYER_ATK_VOICE_COMBOTIME_ROUNDUP	(50)

#define PLAYER_ATK_CHARGE_TIME				(60 * 2)
#define PLAYER_ATK_CHARGE					(60)


#define PLAYER_STEP_TIME					(22 * 2)
#define PLAYER_STEP_WAIT_TIME				(14)
#define PLAYER_STEP_SPEED					(1.5f)

#define PLAYER_SLOPE_SPEED_MAX				(0.2f)
#define PLAYER_SLOPE_SPEED					(0.02f)
#define PLAYER_SLOPE_INERTIA				(0.1f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************

class Player
{
public:
	enum MODE
	{	// プレイヤーのモード管理
		MODE_FLOAT,
		MODE_FLY,
		MODE_LOCKON,
		MODE_CHANGE,
		MODE_MAX
	};
	enum MODEL_ANIM
	{
		CHARACTER,
		WING,
		MODEL_ANIM_MAX
	};
	enum MODEL_NORMAL
	{
		SWORD,
		MODEL_NORMAL_MAX
	};
	CSkinMesh		*m_CSkinMesh;	// スキンメッシュ格納用
	//enum PLAYER
	//{	// プレイヤー管理
	//	PLAYER_1P,
	//	PLAYER_MAX
	//};

	// コンストラクタ（初期化処理）
	Player(void);
	//デストラクタ（終了処理）
	~Player(void);

	// 初期化処理
	void	Init(void);
	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);

	// ファイル確認処理
	void	CheckFile(void);
	// ファイル読み込み処理
	bool	LoadFile(void);
	// ファイル書き込み処理
	void	WriteFile(void);

private:
	D3DXVECTOR3		m_vPos;				// 座標情報
	D3DXVECTOR3		m_vRot;				// 回転情報
	D3DXVECTOR3		m_vSlope;			// 傾き情報
	D3DXVECTOR3		m_vSlopeIner;		// 傾き情報
	D3DXVECTOR3		m_vScl;				// 拡縮情報
	D3DXVECTOR3		m_vMove;			// 移動量情報
	D3DXVECTOR3		m_vRotIner;			// 回転情報
	D3DXVECTOR3		m_vRotChange;		// 回転情報
	D3DXVECTOR3		m_vUp;

	D3DXVECTOR3		m_vTarget;			// ターゲット座標

	// ローカルベクトル
	D3DXVECTOR3		m_vX;
	D3DXVECTOR3		m_vY;
	D3DXVECTOR3		m_vZ;

	MODE			m_eMode;
	MODE			m_eModeNext;
	MODE			m_eModeOld;

	float			m_fMoveSpeed;

	int				m_nCount;			// 汎用カウンター
	float			m_fRiseSpeed;
	bool			m_bUse;				// 使用フラグ

	D3DXMATRIX		m_mtxWorld;				// ワールドマトリクス
	D3DXMATRIX*		m_pMtxTorso;			// ワールドマトリクス


	// エフェクト関係
	EffectManager* pEffetMgr;
	::Effekseer::Handle					m_handle;


#ifdef _DEBUG
	DebugObjectData m_cDebug;
#endif

	// アニメーション
	enum PlayerAnim
	{	// アニメーションセットは追加の逆順
		PLAYER_STEP_RIGHT,
		PLAYER_STEP_LEFT,
		PLAYER_STEP_BACK,
		PLAYER_STEP_FRONT,
		PLAYER_GUARD,
		PLAYER_ATK_RICARD,
		PLAYER_ATK_ROUNDUP,
		PLAYER_ATK_THRUST,
		PLAYER_ATK_HORIZON,
		PLAYER_FLOAT,
		PLAYER_FLY,
		PLAYER_ANIM_MAX
	};
	DWORD			m_dwAnim;
	// アニメーション設定処理
	void SetAnim(DWORD dwFlag) { m_dwAnim |= dwFlag; }
	void ChangeAnim(DWORD dwAnime, FLOAT fShift) { if (m_CSkinMesh != NULL) m_CSkinMesh->ChangeAnim(dwAnime, fShift); }
	void ChangeAnimSpeed(FLOAT AnimSpeed) { if (m_CSkinMesh != NULL) m_CSkinMesh->SetAnimSpeed(AnimSpeed); }
	void CheckAnim(void);


	// アタック
	void Attack(void);
	int			m_nComboTime;
	int			m_nComboCount;
	bool		m_bAttack;
	int			m_nCharge;
	bool		m_bCharge;
	bool		m_bChargeAttack;


	// ステップ
	enum PlayerStep
	{
		STEP_NON,
		STEP_RIGHT,
		STEP_LEFT,
		STEP_BACK,
		STEP_FRONT
	};
	PlayerStep	m_eStep;
	void		Step(void);
	void		SetStep(PlayerStep eStep);
	void		UpdateStep(void);
	bool		m_bStep;
	int			m_nStepCount;

	// ガード
	void Guard(void);
	bool m_bGuard;

	// 魔法陣
	float		m_fMagicScl;

	// モード
	void Float(void);
	void Fly(void);
	void Lockon(void);

	void Action(void);
	bool m_bAction;


	void Change(void);
	void ModeChange(void);
	void UpdateMove(void);
	void MoveNormal(void);
	void SlopeNormal(void);
	void SlopeRestore(void);
	void SetCamera(void);


	Wing	*m_cWing;
	Sword	*m_cSword;
public:
	bool GetGuard(void) { return m_bGuard; }
	D3DXMATRIX GetMtx(void) { return m_mtxWorld; }
	D3DXVECTOR3 GetVecX(void) { return m_vX; }
	D3DXVECTOR3 GetVecY(void) { return m_vY; }
	D3DXVECTOR3 GetVecZ(void) { return m_vZ; }
	D3DXVECTOR3 GetPos(void) { return m_vPos; }
	D3DXVECTOR3 GetPosTaget(void) { return m_vTarget; }
	MODE GetMode(void) { return m_eMode; }
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

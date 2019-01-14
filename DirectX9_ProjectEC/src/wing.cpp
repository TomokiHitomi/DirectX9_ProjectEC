//=============================================================================
//
// ウィング処理 [wing.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "wing.h"
#include "calculate.h"
#include "sound.h"
#include "input.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPSTR g_lpWingBone[WING_BONE_MAX] = {
	(LPSTR)"No_5_joint_migiwabone1",
	(LPSTR)"No_6_joint_migiwabone2",
	(LPSTR)"No_7_joint_migiwabone3",
	(LPSTR)"No_8_joint_migiwabone4",
	(LPSTR)"No_9_joint_migiwabone5",
	(LPSTR)"No_10_joint_migiwabone6",
	(LPSTR)"No_11_joint_hidariwabone1",
	(LPSTR)"No_12_joint_hidariwabone2",
	(LPSTR)"No_13_joint_hidariwabone3",
	(LPSTR)"No_14_joint_hidariwabone4",
	(LPSTR)"No_15_joint_hidariwabone5",
	(LPSTR)"No_16_joint_hidariwabone6"
};

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Wing::Wing(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// スキンメッシュの初期化
	m_CSkinMesh = new CSkinMesh;
	// スキンメッシュにモデルを設定
	m_CSkinMesh->Init(pDevice, (LPSTR)WING_MODEL);
	ChangeAnimSpeed(WING_ANIM_SPEED_DEF);
	// ウィング設置用のマトリクスを初期化
	m_pSetMtx = NULL;


	// 羽根設置カウントの初期化
	m_nWingFeaderCount = 0;
	// 羽根設置タイムの初期化
	m_nWingFeaderTime = 0;

	for (UINT i = 0; i < WING_BONE_MAX; i++)
	{
		// ボーンのマトリクスを初期化
		m_pBoneMtxArray[i] = NULL;
		// ボーンマトリクスのアドレスを取得
		m_pBoneMtxArray[i] = m_CSkinMesh->GetBoneMatrixAdr(g_lpWingBone[i]);
	}

	// ローカル情報を作成
	D3DXVECTOR3 pos, rot, scl;
	pos = WING_POS;
	rot = WING_ROT;
	scl = D3DXVECTOR3(WING_SCL, WING_SCL, WING_SCL);

	// ワールド行列を作成
	WorldConvert(&m_mtxWorld, pos, rot, scl);
}

//=============================================================================
// デストラクタ（解放処理）
//=============================================================================
Wing::~Wing(void)
{
	SAFE_RELEASE(m_CSkinMesh);
	SAFE_DELETE(m_CSkinMesh);
}

//=============================================================================
// 更新処理
//=============================================================================
void Wing::Update(void)
{
	// アニメーションの変更を確認
	CheckAnim();
	// スキンメッシュのアップデート
	m_CSkinMesh->Update();

	// 一定フレームごとにフェザーを設置
	if (m_nWingFeaderTime % WING_FEATHER_SET_TIME == 0)
	{
		D3DXVECTOR3 temp;
		// ウィングのボーンマトリクスから座標を取得
		temp.x = (*m_pBoneMtxArray[m_nWingFeaderCount])._41;
		temp.y = (*m_pBoneMtxArray[m_nWingFeaderCount])._42;
		temp.z = (*m_pBoneMtxArray[m_nWingFeaderCount])._43;
		// フェザーを設置
		m_cWingFeader.Set(temp);
		// ボーンの参照カウンタを加算
		m_nWingFeaderCount++;
		if (m_nWingFeaderCount >= WING_BONE_MAX) m_nWingFeaderCount = 0;
	}
	// フェザータイムを加算
	m_nWingFeaderTime++;
	// フェザーを更新
	m_cWingFeader.Update();
}

//=============================================================================
// 描画処理
//=============================================================================
void Wing::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxTemp;		// アニメーション更新処理

	if (m_pSetMtx != NULL)
	{
		// 設置先マトリクスをコピー
		mtxTemp = *m_pSetMtx;

		// マトリクスを正規化
		D3DXMatrixNormalize(&mtxTemp, &mtxTemp);
	}
	else D3DXMatrixIdentity(&mtxTemp);

	// モデル行列にセット行列をかける
	D3DXMatrixMultiply(&mtxTemp, &m_mtxWorld, &mtxTemp);

	// セット行列を渡して描画
	m_CSkinMesh->Draw(pDevice, mtxTemp);

	m_cWingFeader.Draw();
}

//=============================================================================
// アニメーション確認
//=============================================================================
void Wing::CheckAnim(void)
{
	// 優先順位ごとに if 分岐
	if		(WING_ANIM_FRY & m_dwAnim)		ChangeAnim(WING_FLY, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_FLOAT & m_dwAnim)	ChangeAnim(WING_FLOAT, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_BACK & m_dwAnim)		ChangeAnim(WING_BACK, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_CLOSE & m_dwAnim)	ChangeAnim(WING_CLOSE, WING_ANIM_WEIGHT_DEF);

	// アニメーションビットパターンを初期化
	m_dwAnim = 0x00000000l;
}
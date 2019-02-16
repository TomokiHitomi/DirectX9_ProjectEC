//=============================================================================
//
// スカイドーム処理 [skydome.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "skydome.h"
#include "calculate.h"
#include "playermanager.h"

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

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Skydome::Skydome(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::SKYDOME, Priority::Middle, Priority::Highest);

	// 各プロパティの初期化
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScl = D3DXVECTOR3(MODEL_SKYDOME_SCL, MODEL_SKYDOME_SCL, MODEL_SKYDOME_SCL);
	m_bUse = true;

	// モデルの初期化
	//m_CSkinMesh->Init(pDevice, "data/model/Yuko.x");
	m_CXModel = new CXModel;
	m_CXModel->Init(pDevice, (LPSTR)SKYDOME_MODEL, (LPSTR)SKYDOME_TEXTURE);
	m_CXModel->SetLight(false);
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Skydome::~Skydome(void)
{
	if (m_CXModel != NULL)
	{
		m_CXModel->Release();
		delete m_CXModel;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Skydome::Update(void)
{
	if (m_bUse)
	{
		// アニメーション更新処理
		m_CXModel->Update();
		m_vPos = PlayerManager::GetPos(PlayerManager::PLAYER_1P);
		m_vPos.y = 0.0f;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Skydome::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// ワールド変換
		WorldConvert(&m_mtxWorld, m_vPos, m_vRot, m_vScl);
		m_CXModel->Draw(m_mtxWorld);
	}
}
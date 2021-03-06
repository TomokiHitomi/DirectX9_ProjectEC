//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "playermanager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player* PlayerManager::m_pPlayer[PLAYER_MAX] = {};

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
PlayerManager::PlayerManager(void)
{
	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::PLAYERMANAGER, Priority::Middle, Priority::High);

	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーを初期化
		m_pPlayer[i] = NULL;
		m_pPlayer[i] = new Player;
	}
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
PlayerManager::~PlayerManager(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		delete m_pPlayer[i];
		m_pPlayer[i] = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void PlayerManager::Update(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Update();
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void PlayerManager::Draw(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Draw();
		}
	}
}
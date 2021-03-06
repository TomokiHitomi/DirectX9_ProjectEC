//=============================================================================
//
// モデル処理 [model.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "model.h"

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
bool ModelManager::m_bUse = true;

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
ModelManager::ModelManager(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (unsigned int i = 0; i < MODEL_MAX; i++)
	{
		m_CSkinMesh[i] = NULL;
		if (ModelManager::m_bUse)
		{
			m_CSkinMesh[i] = new CSkinMesh;
		}
	}

	if (ModelManager::m_bUse)
	{
		m_CSkinMesh[PLAYER]->Init(pDevice, (LPSTR)MODEL_PLAYER);
		m_CSkinMesh[WING]->Init(pDevice, (LPSTR)MODEL_WING);
	}
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
ModelManager::~ModelManager(void)
{
	for (unsigned int i = 0; i < MODEL_MAX; i++)
	{
		SAFE_DELETE(m_CSkinMesh[i]);
		//if(!m_CSkinMesh[i])
		//delete m_CSkinMesh[i];
		//m_CSkinMesh[i] = NULL;
	}
}

//=============================================================================
// キャラクターデータの取得処理
//=============================================================================
CSkinMesh *ModelManager::GetCharData(ModelManager::MODEL eModel)
{
	if (ModelManager::m_bUse)
	{
		return m_CSkinMesh[eModel];
	}
	else
	{
		return NULL;
	}
}
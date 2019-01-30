//=============================================================================
//
// バレットマネージャー処理 [bulletmanager.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "bulletmanager.h"
#include "calculate.h"

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
//#ifdef _DEBUG
//bool BulletManager::s_bDebug;
//#endif

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
BulletManager::BulletManager(void)
{
	//// オブジェクトIDとプライオリティの設定処理
	//SetIdAndPriority(ObjectID::BULLETMANAGER, Priority::Low, Priority::Low);

	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		tProp[i].vPos = ZERO_D3DXVECTOR3;
		tProp[i].vMove = ZERO_D3DXVECTOR3;
		tProp[i].fCol = 0.0f;
		tProp[i].fMoveSpeed = 0.0f;
		tProp[i].fSize = 0.0f;
		tProp[i].nCount = 0;
		tProp[i].bUse = false;
	}

	m_pBullet = NULL;
	m_pBullet = new Bullet;

	m_pBulletEffect = NULL;
	m_pBulletEffect = new BulletEffect;
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
BulletManager::~BulletManager(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
#ifdef _DEBUG
		DebugObject::pSphere->Release(tProp[i].cDebug.nIdx);
#endif
	}
	SAFE_DELETE(m_pBullet);
	SAFE_DELETE(m_pBulletEffect);
}

//=============================================================================
// 開始処理
//=============================================================================
void BulletManager::Set(D3DXVECTOR3 vPos, D3DXVECTOR3 vMove, float fCol, float fMoveSpeed, float fSize)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (!tProp[i].bUse)
		{
			tProp[i].vPos = vPos;
			//tProp[i].vMove = vMove;
			D3DXVec3Normalize(&tProp[i].vMove, &vMove);
			tProp[i].fCol = fCol;
			tProp[i].fMoveSpeed = fMoveSpeed;
			tProp[i].fSize = fSize;
			tProp[i].nCount = 0;
			tProp[i].bUse = true;

			tProp[i].cBullet.bUse = true;
			tProp[i].cBullet.fSize = tProp[i].fSize;
			tProp[i].cBullet.nIdx = m_pBullet->Set(tProp[i].cBullet.fSize);
			m_pBullet->SetColor(tProp[i].cBullet.nIdx, tProp[i].fCol);
			
#ifdef _DEBUG
			tProp[i].cDebug.bUse = true;
			tProp[i].cDebug.fSize = tProp[i].fSize;
			tProp[i].cDebug.nIdx = DebugObject::pSphere->Set(tProp[i].cDebug.fSize);
#endif
			return;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void BulletManager::Update(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (tProp[i].bUse)
		{
			tProp[i].vPos += tProp[i].vMove * tProp[i].fMoveSpeed;

			tProp[i].nCount++;

			if (tProp[i].nCount > BULLET_COUNT_MAX) Delete(i);
			m_pBullet->SetPos(tProp[i].cBullet.nIdx, tProp[i].vPos);

			m_pBulletEffect->Set(tProp[i].vPos, tProp[i].fSize, tProp[i].fCol, 15);
#ifdef _DEBUG
			DebugObject::pSphere->SetPos(tProp[i].cDebug.nIdx, tProp[i].vPos);
#endif
		}
	}
	SAFE_UPDATE(m_pBullet);
	SAFE_UPDATE(m_pBulletEffect);
}

//=============================================================================
// 描画処理
//=============================================================================
void BulletManager::Draw(void)
{

	SAFE_DRAW(m_pBulletEffect);
	SAFE_DRAW(m_pBullet);
}

//=============================================================================
// 削除処理
//=============================================================================
void BulletManager::Delete(int nNum)
{
	tProp[nNum].bUse = false;
	m_pBullet->Release(tProp[nNum].cBullet.nIdx);
#ifdef _DEBUG
	DebugObject::pSphere->Release(tProp[nNum].cDebug.nIdx);
#endif
}

//=============================================================================
// 解放処理
//=============================================================================
void BulletManager::Release(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (tProp[i].bUse)
		{
			Delete(i);
		}
	}
}

////=============================================================================
//// 更新処理
////=============================================================================
//void EnemyNormal::Update(void)
//{
//
//}
//
////=============================================================================
//// 更新処理
////=============================================================================
//void EnemyNormal::Draw(void)
//{
//
//}


////=============================================================================
//// コンストラクタ（初期化処理）
////=============================================================================
//BulletManager::BulletManager(void)
//{
//	// ポインタを初期化
//	m_pBullet = NULL;
//	m_pBullet = new Bullet;
//}
//
////=============================================================================
//// デストラクタ（終了処理）
////=============================================================================
//BulletManager::~BulletManager(void)
//{
//	Release();
//}
//
////=============================================================================
//// 初期化処理
////=============================================================================
//void BulletManager::Init(void)
//{
//
//}
//
////=============================================================================
//// 終了処理
////=============================================================================
//void BulletManager::Release(void)
//{
//	SAFE_RELEASE(m_pBullet);
//	SAFE_DELETE(m_pBullet);
//}
//
////=============================================================================
//// 描画処理
////=============================================================================
//void BulletManager::Draw(void)
//{
//	////Enemy* pList = m_pRoot;
//
//	////while (pList != NULL)
//	////{
//	////	pList->Draw();
//	////	pList = pList->m_pNext;
//	////}
//	//SAFE_DRAW(pOcta);
//}
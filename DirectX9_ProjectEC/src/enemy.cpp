//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "enemy.h"
#include "game.h"
#include "calculate.h"
#include "tool.h"

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
#ifdef _DEBUG
bool EnemyManager::s_bDebug;
#endif

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Enemy::Enemy(void)
{
	m_pPrev = NULL;
	m_pNext = NULL;
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Enemy::~Enemy(void)
{
#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif
}

//=============================================================================
// 開始処理
//=============================================================================
void Enemy::Start(void)
{
	m_cProp.bUse = true;

	m_cOctaData.bUse = true;
	m_cOctaData.fSize = m_cProp.fSize;
	m_cOctaData.nIdx = pOcta->Set(m_cOctaData.fSize);
	pOcta->SetColor(m_cOctaData.nIdx, m_cProp.fCol);

#ifdef _DEBUG
	m_cDebug.bUse = true;
	m_cDebug.fSize = m_cProp.fSize;
	m_cDebug.nIdx = DebugObject::pSphere->Set(m_cDebug.fSize);
#endif
}

//=============================================================================
// 更新処理
//=============================================================================
void Enemy::Update(void)
{
	pOcta->SetPos(m_cOctaData.nIdx, m_cProp.vPos);
	pOcta->AddRot(m_cOctaData.nIdx, 0.02f);
#ifdef _DEBUG
	if (EnemyManager::s_bDebug)
	{
		ImGui::Text("Pos [%6.0f.%6.0f,%6.0f]\n",
			m_cProp.vPos.x, m_cProp.vPos.y, m_cProp.vPos.z);
	}
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, m_cProp.vPos);
#endif

	m_cProp.nCount++;
	if (m_cProp.nCount > ENEMY_BULLET_START)
	{
		if (m_cProp.nCount % ENEMY_BULLET_RELOAD == 0)
		{
			float fRot = pOcta->GetRot(m_cOctaData.nIdx);
			D3DXVECTOR3 vMove;
			vMove = D3DXVECTOR3(sinf(fRot), 0.0f, cosf(fRot));
			m_pBullet->Set(
				m_cProp.vPos + vMove * m_cProp.fSize,
				vMove,
				m_cProp.fCol,
				3.0f,
				10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI), 0.0f, cosf(fRot + D3DX_PI));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI * 0.5f), 0.0f, cosf(fRot + D3DX_PI * 0.5f));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI * 1.5f), 0.0f, cosf(fRot + D3DX_PI * 1.5f));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Enemy::Draw(void)
{

}

//=============================================================================
// 解放処理
//=============================================================================
void Enemy::Release(void)
{
	pOcta->Release(m_cOctaData.nIdx);
#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif

	// 前ポインタがNULLではない
	if ((m_pPrev) != NULL)
	{
		// 前ポインタが指す次ポインタを
		// 自らが格納している次ポインタに差し替える
		(m_pPrev)->m_pNext = m_pNext;
	}

	// 次ポインタがNULLではない
	if (m_pNext != NULL)
	{
		// 次ポインタが指す前ポインタを
		// 自らが格納している前ポインタに差し替える
		m_pNext->m_pPrev = m_pPrev;
	}

	delete this;
}

//=============================================================================
// 更新処理
//=============================================================================
void EnemyNormal::Update(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void EnemyNormal::Draw(void)
{

}


//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
EnemyManager::EnemyManager(void)
{
	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::ENEMYMANAGER, Priority::Middle, Priority::Middle);

	// ルートポインタを初期化
	m_pRoot = NULL;

	pOcta = NULL;
	pOcta = new Octa;

	m_pBullet = NULL;
	m_pBullet = new BulletManager;
	//m_pBullet = ObjectManager::CreateObject<BulletManager>();

	// 読込処理
	Load();
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
EnemyManager::~EnemyManager(void)
{
	Uninit();
	SAFE_DELETE(pOcta);
	SAFE_DELETE(m_pBullet);
}

//=============================================================================
// 初期化処理
//=============================================================================
void EnemyManager::Init(void)
{

}

//=============================================================================
// 終了処理
//=============================================================================
void EnemyManager::Uninit(void)
{
	Enemy* pList = m_pRoot;
	Enemy* pTemp = NULL;

	while (pList != NULL)
	{
		pTemp = pList->m_pNext;
		pList->Release();
		pList = pTemp;
	}
}

//=============================================================================
// 
//=============================================================================
bool EnemyManager::Create(Enemy** ppEnemy, int* pData)
{
	if (pData[DATA_WAVE] < 0)
		return false;

	//EnemyNormal* pTest = new EnemyNormal;
	//delete pTest;

	// 仮に[1]がタイプだったとしたら
	switch (pData[DATA_TYPE])
	{
	case ENEMY_NORMAL:
		*ppEnemy = new Enemy;
		break;
	case 1:
		*ppEnemy = new EnemyNormal;
		break;
	}

	(*ppEnemy)->m_cProp.nWave = pData[DATA_WAVE];
	(*ppEnemy)->m_cProp.vPos = D3DXVECTOR3(pData[DATA_POSX], pData[DATA_POSY], pData[DATA_POSZ]);
	(*ppEnemy)->m_cProp.fSize = pData[DATA_SIZE];
	(*ppEnemy)->m_cProp.fCol = pData[DATA_COL];

	(*ppEnemy)->pOcta = pOcta;
	(*ppEnemy)->m_pBullet = m_pBullet;
	return true;
}

//=============================================================================
// ファイル読込処理
//=============================================================================
void EnemyManager::Load(void)
{
	FILE *fp;

	// ファイルを開く
	fopen_s(&fp,ENEMY_FILE, "r");

	// ファイルオープンエラー処理
	if (fp == NULL)
		return;

	// データ取得
	Read(fp);

	// ファイルを閉じる
	fclose(fp);
}

//=============================================================================
// ファイル読込処理
//=============================================================================
void EnemyManager::Read(FILE* fp)
{
	int nData[8];

	Enemy** ppList = &m_pRoot;
	Enemy** ppPrev = NULL;

	if (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d",
		&nData[0], &nData[1], &nData[2], &nData[3], &nData[4], &nData[5], &nData[6], &nData[7]) != EOF)
	{
		if (!Create(ppList, &nData[0]))
			return;
		ppPrev = ppList;
		ppList = &(*ppList)->m_pNext;
	}
	else
		return;

	// エネミーステータス設定
	while (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d",
		&nData[0], &nData[1], &nData[2], &nData[3], &nData[4], &nData[5], &nData[6], &nData[7]) != EOF)
	{
		if (!Create(ppList, &nData[0]))
			return;
		(*ppList)->m_pPrev = *ppPrev;
		ppPrev = ppList;
		ppList = &(*ppList)->m_pNext;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void EnemyManager::Update(void)
{
	Enemy* pList = m_pRoot;
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	s_bDebug = ImGui::TreeNode("Enemy");
#endif

	while (pList != NULL)
	{
		if (pList->m_cProp.nWave > GameScene::GetWave())
			break;

		if (pList->m_cProp.bUse)
			pList->Update();
		else
			pList->Start();

		pList = pList->m_pNext;
	}

#ifdef _DEBUG
	if (s_bDebug)
	{
		ImGui::TreePop();
	}
#endif

	SAFE_UPDATE(pOcta);
	SAFE_UPDATE(m_pBullet);
}

//=============================================================================
// 描画処理
//=============================================================================
void EnemyManager::Draw(void)
{
	//Enemy* pList = m_pRoot;

	//while (pList != NULL)
	//{
	//	pList->Draw();
	//	pList = pList->m_pNext;
	//}
	SAFE_DRAW(pOcta);
	SAFE_DRAW(m_pBullet);
}
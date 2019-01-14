//=============================================================================
//
// オブジェクト処理 [object.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "object.h"

/* 全体で必要なインクルード */

/* オブジェクトで必要なインクルード */

/* Debug */
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
ObjectManager		*ObjectManager::s_pRoot[ObjectRootMax] = { NULL, NULL };
//Object	*Object::s_pRootUpdate = NULL;
//Object	*Object::s_pRootDraw = NULL;
int			ObjectManager::nObjectCount = 0;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 更新処理
//=============================================================================


//=============================================================================
// 描画処理
//=============================================================================

//=============================================================================
// 解放処理
//=============================================================================
void ObjectManager::Release(void)
{
	// オブジェクトのリスト離脱処理
	RemoveList();

	// 自身の解放
	delete this;

	// オブジェクト数が0の場合
	if (nObjectCount == 0)
	{
		for (unsigned int i = 0; i < ObjectRootMax; i++)
		{
			// ルートポインタをNULL
			s_pRoot[i] = NULL;
		}
	}
}

//=============================================================================
// コンストラクタ処理（初期化）
//=============================================================================
ObjectManager::ObjectManager(void)
{
	// オブジェクトカウンタをインクリメント
	nObjectCount++;

	// プロパティの初期化
	for (unsigned int i = 0; i < ObjectRootMax; i++)
	{
		m_pPrev[i] = NULL;			// 前ポインタの初期化
		m_pNext[i] = NULL;			// 後ポインタの初期化
		m_ePriority[i] = Middle;	// 優先順位を Middle で初期化
	}
}


//=============================================================================
// デストラクタ処理（終了）
//=============================================================================
ObjectManager::~ObjectManager(void)
{
	// オブジェクトカウンタをデクリメント
	nObjectCount--;
}

//=============================================================================
// オブジェクトIDとプライオリティの設定処理
// 引数：ID, 更新プライオリティ, 描画プライオリティ
//=============================================================================
void ObjectManager::SetIdAndPriority(ObjectID eObjId, Priority eUpdateP, Priority eDrawP)
{
	//// プライオリティテスト用
	//eObjId = ObjectID(nObjectCount);

	SetObjectId(eObjId);				// オブジェクトIDを設定
	SetPriority(UpdateRoot, eUpdateP);	// 更新プライオリティを設定
	SetPriority(DrawRoot, eDrawP);		// 描画プライオリティを設定

	// オブジェクトをリスト追加処理
	InsertList();
}

//=============================================================================
// オブジェクトのリスト追加処理
//=============================================================================
void ObjectManager::InsertList(void)
{
	ObjectManager **pList = NULL;
	ObjectManager **pPrevTemp = NULL;

	for (unsigned int i = 0; i < ObjectRootMax; i++)
	{
		// ルートポインタをpListに格納
		pList = GetObjectRootAdr(ObjectRoot(i));

		// リスト構造形成ループ
		while (true)
		{
			// pListポインタが設定済みならば
			if (*pList != NULL)
			{
				// プライオリティが高い
				if ((*pList)->m_ePriority[i] > m_ePriority[i])
				{
					// 割り込み処理
					// pListの前ポインタに自らを格納
					(*pList)->m_pPrev[i] = this;
					// pListがルート以外
					if (*pList != s_pRoot[i])
					{
						// 前ポインタにpPrevTempを設定
						m_pPrev[i] = *pPrevTemp;
					}
					m_pNext[i] = *pList;
					// pListに自らを格納
					*pList = this;
					break;
				}

				// 前ポインタ候補としてpListを保管
				pPrevTemp = pList;

				// 次ポインタをpListに格納しなおす
				pList = &(*pList)->m_pNext[i];
			}
			// 次ポインタが未設定ならば
			else if (*pList == NULL)
			{
				// pListがルート以外
				if (*pList != s_pRoot[i])
				{
					// 前ポインタにpPrevTempを設定
					m_pPrev[i] = *pPrevTemp;
				}

				// pListの指すRootまたはNextに自らを格納
				*pList = this;
				break;
			}
		}
	}
}

//=============================================================================
// オブジェクトのリスト離脱処理
//=============================================================================
void ObjectManager::RemoveList(void)
{
	// ルート格納用ポインタ
	ObjectManager	**pRoot = NULL;

	// リスト構造の再結合処理
	for (unsigned int i = 0; i < ObjectRootMax; i++)
	{
		// ルートポインタを取得
		pRoot = GetObjectRootAdr(ObjectRoot(i));

		// 前ポインタがNULLではない
		if ((m_pPrev[i]) != NULL)
		{
			// 前ポインタが指す次ポインタを
			// 自らが格納している次ポインタに差し替える
			(m_pPrev[i])->m_pNext[i] = m_pNext[i];
		}
		// 前ポインタがNULL
		else
		{
			// ルートポインタを次ポインタに差し替える
			*pRoot = m_pNext[i];
		}

		// 次ポインタがNULLではない
		if (m_pNext[i] != NULL)
		{
			// 次ポインタが指す前ポインタを
			// 自らが格納している前ポインタに差し替える
			m_pNext[i]->m_pPrev[i] = m_pPrev[i];
		}
	}
}

//=============================================================================
// オブジェクトのポインタ取得処理
//=============================================================================
//template <typename Type> Type* Object::GetObjectPointer(ObjectID eObjId)
//{
//	Object *pList = GetObjectRoot(UpdateRoot);
//
//	while (pList != NULL)
//	{
//		if (eObjId == pList->GetObjectId())
//		{	
//			return dynamic_cast<Type*>(pList);
//		}
//		pList = pList->GetObjectNext(UpdateRoot);
//	}
//	return NULL;
//}

//=============================================================================
// 全オブジェクトの更新処理
//=============================================================================
void ObjectManager::UpdateAll(void)
{
	ObjectManager *pList = ObjectManager::GetObjectRoot(UpdateRoot);

#ifdef _DEBUG
	PrintDebugProc("【 Object 】\n");
	PrintDebugProc("ObjectCount [%d]\n", nObjectCount);
	PrintDebugProc("\n");


	while (pList != NULL)
	{
		PrintDebugProc("Update[ ID : %d  Pri : %d] \n",
			pList->GetObjectId(), pList->GetPriority(UpdateRoot));
		pList = pList->GetObjectNext(UpdateRoot);
	}
	PrintDebugProc("\n");
	pList = ObjectManager::GetObjectRoot(UpdateRoot);
#endif

	while (pList != NULL)
	{
		pList->Update();
		pList = pList->GetObjectNext(UpdateRoot);
	}
}

//=============================================================================
// 全オブジェクトの描画処理
//=============================================================================
void ObjectManager::DrawAll(void)
{
	ObjectManager *pList = ObjectManager::GetObjectRoot(DrawRoot);

	
	while (pList != NULL)
	{
		pList->Draw();
#ifdef _DEBUG
		PrintDebugProc(" Draw [ ID : %d  Pri : %d] \n",
			pList->GetObjectId(), pList->GetPriority(DrawRoot));
#endif
		pList = pList->GetObjectNext(DrawRoot);
	}
#ifdef _DEBUG
	PrintDebugProc("\n");
#endif
}

//=============================================================================
// 全オブジェクトの解放処理
//=============================================================================
void ObjectManager::ReleaseAll(void)
{
	ObjectManager *pList = NULL;
	ObjectManager *pNext = NULL;
	for (unsigned int i = 0; i < ObjectRootMax; i++)
	{
		// ルートポインタをpListに格納
		pList = GetObjectRoot(ObjectRoot(i));

		// NULLがくるまでpListをたどる
		while (pList != NULL)
		{
			pNext = pList->GetObjectNext(ObjectRoot(i));
			pList->Release();
			pList = pNext;
		}
	}
}

//=============================================================================
// ルートポインタのアドレス取得処理
//=============================================================================
ObjectManager **ObjectManager::GetObjectRootAdr(ObjectRoot eObjRoot)
{
	return &s_pRoot[eObjRoot];
}

//=============================================================================
// ネクストポインタの取得処理
//=============================================================================
ObjectManager *ObjectManager::GetObjectNext(ObjectRoot eObjRoot)
{ 
	return m_pNext[eObjRoot];
}
//=============================================================================
//
// オブジェクト処理 [object.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _OBJECT_H_
#define _OBJECT_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 列挙型定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************

// オブジェクト用
class ObjectManager
{
public:
	// リスト内優先順位
	enum Priority
	{
		Highest,
		High,
		Middle,
		Low,
		Lowest,
		Ui
	};
	// オブジェクトID
	enum ObjectID
	{
		NON,
		COPYRIGHT,
		FRAME,
		PLAYER,
		SKYDOME,
		CUBE,
		EFFECT,
		STENCIL,
		STAGE,
		CLOUD,
		PLAYERMANAGER,
		ENEMYMANAGER,
		DEBUGOBJECT,
		//BULLETMANAGER,
		UI
	};

private:
	// ルートポインタ
	enum ObjectRoot
	{
		UpdateRoot,
		DrawRoot,
		ObjectRootMax
	};
	static int		nObjectCount;					// 全オブジェクト数のカウンタ
	static ObjectManager	*s_pRoot[ObjectRootMax];		// リストの更新ルートポインタ
	ObjectManager			*m_pPrev[ObjectRootMax];		// リストの前ポインタ
	ObjectManager			*m_pNext[ObjectRootMax];		// リストの次ポインタ
	Priority		m_ePriority[ObjectRootMax];		// 優先順位（サブ）
	ObjectID		eObjectId;						// オブジェクト識別用ID
public:
	// コンストラクタ（初期化処理）
	ObjectManager();
	// デストラクタ（終了処理）
	virtual ~ObjectManager();

	// 更新処理
	virtual void Update(void) { ; }
	// 描画処理
	virtual void Draw(void) { ; }
	// 解放処理
	void Release(void);

	// オブジェクトIDとプライオリティの設定処理
	void SetIdAndPriority(ObjectID, Priority, Priority);
	// オブジェクトのリスト追加処理
	void InsertList(void);
	// オブジェクトのリスト離脱処理
	void RemoveList(void);

	// 全オブジェクトの更新処理
	static void UpdateAll(void);
	// 全オブジェクトの描画処理
	static void DrawAll(void);
	// 全オブジェクトの解放処理
	static void ReleaseAll(void);

	// ルートポインタの取得処理
	static ObjectManager *GetObjectRoot(ObjectRoot eObjRoot) { return s_pRoot[eObjRoot]; }
	// ルートポインタのアドレス取得処理
	static ObjectManager **GetObjectRootAdr(ObjectRoot);

	// ネクストポインタの取得処理
	ObjectManager *GetObjectNext(ObjectRoot eObjRoot);


	// オブジェクトID設定処理
	void SetObjectId(ObjectID eObjId) { eObjectId = eObjId; }
	// オブジェクトID取得処理
	ObjectID GetObjectId(void) { return (eObjectId); }

	// プライオリティ設定処理
	void SetPriority(ObjectRoot eObjRoot, Priority ePriority) { m_ePriority[eObjRoot] = ePriority; }
	// プライオリティ取得処理
	Priority GetPriority(ObjectRoot eObjRoot) { return (m_ePriority[eObjRoot]); }

	// オブジェクトのポインタ取得処理
	template <typename Type>
	static Type *GetObjectPointer(ObjectID eObjId)
	{
		ObjectManager *pList = GetObjectRoot(UpdateRoot);

		while (pList != NULL)
		{
			if (eObjId == pList->GetObjectId())
			{
				// 対象ポインタを指定した型でダウンキャストして返す
				return dynamic_cast<Type*>(pList);
			}
			pList = pList->GetObjectNext(UpdateRoot);
		}
		return NULL;
	}

	// オブジェクトの作成処理
	template <typename Type>
	static Type* CreateObject(void)
	{
		Type* pType = new Type;
		return pType;
	}
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

//=============================================================================
//
// バレットマネージャー処理 [bulletmanager.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "debugobject.h"
#include "bullet.h"
#include "bulleteffect.h"
//#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define BULLET_MAX			(8192)
#define BULLET_COUNT_MAX	(800)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class BulletManager/* : public ObjectManager*/
{
public:
	// コンストラクタ（初期化処理）
	BulletManager(void);
	//デストラクタ（終了処理）
	~BulletManager(void);

	// 更新処理
	virtual void Update(void);
	// 描画処理
	virtual void Draw(void);
	// 設置処理
	virtual void Set(D3DXVECTOR3 vPos, D3DXVECTOR3 vMove, float fCol, float fMoveSpeed, float fSize);
	// 削除処理
	virtual void Delete(int nNum);
	// 削除処理
	virtual void Release(void);


	typedef struct BulletProp
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vMove;
		float		fCol;
		float		fMoveSpeed;
		float		fSize;
		int			nCount;
		bool		bUse;
		BulletData	cBullet;
#ifdef _DEBUG
		DebugObjectData cDebug;
#endif
	};

	BulletProp tProp[BULLET_MAX];

	Bullet*			m_pBullet;
	BulletEffect*	m_pBulletEffect;

	//typedef struct BulletData
	//{
	//	float fMoveSpeed;
	//};
	//
	//BulletData tData;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

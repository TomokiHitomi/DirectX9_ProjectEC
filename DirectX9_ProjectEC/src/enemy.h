//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "object.h"
#include "debugobject.h"
#include "octa.h"
#include "bulletmanager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define MODEL_PLAYER			("data/model/haku/haku.X")
//#define MODEL_WING				("data/model/wing/wing.X")
#define ENEMY_FILE				("data/file/enemy.dat")
#define ENEMY_BULLET_START		(100)
#define ENEMY_BULLET_RELOAD		(7)

//*****************************************************************************
// 列挙型定義
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Enemy
{
public:
	Enemy*	m_pPrev;		// リストの前ポインタ
	Enemy*	m_pNext;		// リストの次ポインタ

	// コンストラクタ（初期化処理）
	Enemy(void);
	//デストラクタ（終了処理）
	~Enemy(void);

	void Start(void);
	// 更新処理
	virtual void Update(void);
	// 描画処理
	virtual void Draw(void);

	void Release(void);

	class Prop
	{
	public:
		int				nWave;	// 出現ウェーブ
		D3DXVECTOR3		vPos;	// 座標
		float			fSize;	// サイズ
		float			fCol;	// カラー
		int				nCount;	// 生存カウント
		bool			bUse;	// 使用フラグ
		Prop()
		{
			nWave = -1;
			vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			fSize = 0.0f;
			fCol = 0.0f;
			nCount = 0;
			bUse = false;
		}
	};

	Prop		m_cProp;
	Octa*		pOcta;
	OctaData	m_cOctaData;

	BulletManager* m_pBullet;
#ifdef _DEBUG
	DebugObjectData m_cDebug;
#endif
};

class EnemyNormal : public Enemy
{
public:
	int nTest;
	// コンストラクタ（初期化処理）
	EnemyNormal(void) { ; }
	//デストラクタ（終了処理）
	~EnemyNormal(void) { ; }

	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
private:


};



class EnemyManager : public ObjectManager
{
public:
	enum ENEMY
	{	
		ENEMY_NORMAL,
		ENEMY_MAX
	};

	Enemy* m_pRoot;		// リストの更新ルートポインタ

	// コンストラクタ（初期化処理）
	EnemyManager(void);
	//デストラクタ（終了処理）
	~EnemyManager(void);

	// 初期化処理
	void Init(void);
	// 終了処理
	void Uninit(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// ファイル読込処理
	void Load(void);
	// ファイル読込処理
	void Read(FILE* fp);
 
	bool Create(Enemy** ppEnemy, int* pData);
	Octa* pOcta;
	BulletManager* m_pBullet;

#ifdef _DEBUG
	static bool s_bDebug;
#endif
private:
	enum Data
	{
		DATA_WAVE,
		DATA_TYPE,
		DATA_POSX,
		DATA_POSY,
		DATA_POSZ,
		DATA_SIZE,
		DATA_COL,
		DATA_MAX
	};
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

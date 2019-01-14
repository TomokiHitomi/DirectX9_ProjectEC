//=============================================================================
//
// クラウド処理 [cloud.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _CLOUD_H_
#define _CLOUD_H_

#include "main.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 読み込むテクスチャファイル名
#define	CLOUD_TEXTURE			("data/TEXTURE/cloud2.png")

// テクスチャ内分割数
#define CLOUD_TEXTURE_DIVIDE_X	(2)
#define CLOUD_TEXTURE_DIVIDE_Y	(2)

// テクスチャ分割パターン
#define CLOUD_TEXTURE_NUM		(CLOUD_TEXTURE_DIVIDE_X * CLOUD_TEXTURE_DIVIDE_Y)

// サイズ
#define	CLOUD_SIZE					(150.0f)
#define	CLOUD_SIZE_X				(CLOUD_SIZE)
#define	CLOUD_SIZE_Y				(CLOUD_SIZE)

// 最大数
#define CLOUD_MAX_X				(100)
#define CLOUD_MAX_Y				(3)
#define CLOUD_MAX_Z				(100)
#define CLOUD_MAX				(CLOUD_MAX_X * CLOUD_MAX_Y * CLOUD_MAX_Z)

#define CLOUD_POS_MARGIN		(100.0f)
#define CLOUD_POS_MARGIN_Y		(30.0f)

#define CLOUD_POS_RANDOM_VECTOR	(10.0f)

// モーフの変化スピード
#define CLOUD_MORPH_SPEED		(0.01f)

// 最大移動量（落下）
#define CLOUD_MOVE				(-50.0f)

// 設置間隔（フレーム）
#define CLOUD_SET_TIME			(10)

// 設置距離（1～指定値のランダム）
#define CLOUD_SET_LENGTH			(20)


//*****************************************************************************
// クラス定義
//*****************************************************************************
class Cloud : public ObjectManager
{
private:
	LPDIRECT3DTEXTURE9				m_pTexture;	// テクスチャ
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;	// 頂点バッファ
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// インスタンスバッファ
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;	// インデックスバッファ
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// 頂点宣言

	// シェーダ関連
	LPD3DXEFFECT					pEffect;
	enum EH
	{
		EH_TEX,
		EH_PROJ,
		EH_VIEW,
		EH_WORLD,
		EH_DATA,
		EH_MORPH,
		EH_POS,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// シェーダハンドル

	int								m_nCount;
	float							m_fMorph;
	float							m_fMove;
	D3DXVECTOR4						m_vPos;
	D3DXVECTOR4						m_vPosSet;
	D3DXVECTOR4						m_vMove;	// (move.x, move.y, 0.0f, size)

	typedef struct
	{
		D3DXVECTOR4 vtx;
		D3DXVECTOR2 uv;
		D3DCOLOR	diffuse;
	}VERTEX;

	typedef struct
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 vec;
		float		rot;
		D3DXVECTOR2 shiftuv;
	}INSTANCE;

	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
public:
	// コンストラクタ
	Cloud(void);
	//デストラクタ
	~Cloud(void);
	// 初期化処理
	HRESULT	Init(void);
	// 解放処理
	void	Release(void);
	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);
	// 設置処理
	//void	Set(D3DXVECTOR3 pos);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

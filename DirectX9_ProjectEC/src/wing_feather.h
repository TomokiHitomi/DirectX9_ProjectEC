//=============================================================================
//
// ウィングフェザー処理 [wing_feather.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _WING_FEATHER_H_
#define _WING_FEATHER_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 読み込むテクスチャファイル名
#define	WING_FEATHER_TEXTURE			("data/texture/Feather.png")

// テクスチャ内分割数
#define WING_FEATHER_TEXTURE_DIVIDE_X	(4)
#define WING_FEATHER_TEXTURE_DIVIDE_Y	(4)
// テクスチャ分割パターン
#define WING_FEATHER_TEXTURE_NUM		(WING_FEATHER_TEXTURE_DIVIDE_X * WING_FEATHER_TEXTURE_DIVIDE_Y)

// サイズ
#define	WING_FEATHER_SIZE_X				(1.5f)
#define	WING_FEATHER_SIZE_Y				(1.5f)

// 最大数
#define WING_FEATHER_MAX				(100)

// モーフの変化スピード
#define WING_FEATHER_MORPH_SPEED		(0.01f)

// 最大移動量（落下）
#define WING_FEATHER_MOVE				(-50.0f)

// 設置間隔（フレーム）
#define WING_FEATHER_SET_TIME			(10)

// 設置距離（1～指定値のランダム）
#define WING_FEATHER_SET_LENGTH			(20)


//*****************************************************************************
// クラス定義
//*****************************************************************************
class WingFeather
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
		EH_MOVE,
		EH_MORPH,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// シェーダハンドル

	int								m_nCount;
	float							m_fMorph;

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
		float		morph;
		D3DXVECTOR2 shiftuv;
	}INSTANCE;

	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
public:
	// コンストラクタ
	WingFeather(void);
	//デストラクタ
	~WingFeather(void);
	// 初期化処理
	HRESULT	Init(void);
	// 解放処理
	void	Release(void);
	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);
	// 設置処理
	void	Set(D3DXVECTOR3 pos);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

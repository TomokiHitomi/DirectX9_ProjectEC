//=============================================================================
//
// バレットエフェクト処理 [bulleteffect.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _BULLETEFFECT_H_
#define _BULLETEFFECT_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLETEFFECT_MAX					(32768)
#define BULLETEFFECT_TEX					("data/texture/effect.png")
#define BULLETEFFECT_TEXTURE_DIVIDE_X		(1)
#define BULLETEFFECT_TEXTURE_DIVIDE_Y		(1)

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class BulletEffect
{
public:
	// コンストラクタ（初期化処理）
	BulletEffect(void);
	//デストラクタ（終了処理）
	~BulletEffect(void);

	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);

	// 使用状態にする
	int Set(D3DXVECTOR3 vPos, float fSize, float fCol, int nCount);
	//// カラーをセット
	//void SetColor(int nIdx, float fCol);
	//// 座標をセット
	//void SetPos(int nIdx, D3DXVECTOR3 vPos);


	// 解放
	void Release(int nIdx);
private:


	LPD3DXEFFECT		pEffect;		// シェーダー関連
	enum EH
	{
		EH_TEX,
		EH_PROJ,
		EH_VIEW,
		EH_WORLD,
		EH_COLORPALLET,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// シェーダハンドル

	LPDIRECT3DTEXTURE9				m_pTexture;	// テクスチャ

	D3DXMATRIX			m_mtxWorld;		// ワールドマトリクス
	UINT				m_nCount;
	bool				m_bUse;			// 使用フラグ

	// インスタンシング用
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// インスタンスバッファ
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;		// インデックスバッファ
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// 頂点宣言

	typedef struct
	{
		D3DXVECTOR4 vtx;
		D3DXVECTOR2 uv;
	}VERTEX;

	// VRAM用
	typedef struct
	{
		D3DXVECTOR3 vPos;		// ワールド座標
		float		fSize;		// サイズ
		float		fUse;		// 使用フラグ（シェーダ用）
		float		fCol;		// カラーインデックス
	}INSTANCE;

	// RAM用
	typedef struct : public INSTANCE
	{
		int		nCountMax;				// 寿命
		int		nCount;				// 寿命
		bool	bUse;				// 使用フラグ
	}DATA;

	DATA	m_tData[BULLETEFFECT_MAX];


	// マトリクスの初期化
	HRESULT InitMatrix(void);

	// メッシュの生成
	HRESULT CreateMesh(LPDIRECT3DDEVICE9 pDevice);
	// メッシュ情報の取得
	HRESULT GetMeshData(LPD3DXMESH pMesh);

	// インスタンシング用データの初期化
	HRESULT InitInst(void);
	// インスタンシングバッファ生成 + データの代入
	HRESULT CreateInst(LPDIRECT3DDEVICE9 pDevice);
	// インスタンシングバッファにデータの代入
	HRESULT SetInst(void);
	// 頂点シェーダ宣言の作成
	HRESULT CreateDecl(LPDIRECT3DDEVICE9 pDevice);
};

//class BulletEffectData
//{
//public:
//	int		nIdx;
//	float	fSize;
//	bool	bUse;
//	BulletEffectData()
//	{
//		nIdx = -1;
//		fSize = 0.0f;
//		bUse = false;
//	}
//};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

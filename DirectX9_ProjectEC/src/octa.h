//=============================================================================
//
// 八面体処理 [octa.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _OCTA_H_
#define _OCTA_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define OCTA_MAX			(50)
#define	OCTA_MODEL			"data/model/octahedral.x"	// 読み込むモデル名
#define OCTA_ROT_SPEED		(0.01f)

//#define DEBUGSPHERE_COLOR	(D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f))

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Octa
{
public:
	// コンストラクタ（初期化処理）
	Octa(void);
	//デストラクタ（終了処理）
	~Octa(void);

	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);

	// 使用状態にする
	int Set(float fSize);
	// カラーをセット
	void SetColor(int nIdx, float fCol);
	// 座標をセット
	void SetPos(int nIdx, D3DXVECTOR3 vPos);
	// 回転を加算
	void AddRot(int nIdx, float fRot);
	// 回転を取得
	float GetRot(int nIdx);


	// 解放
	void Release(int nIdx);
private:
	LPD3DXEFFECT		pEffect;		// シェーダー関連
	D3DXMATRIX			m_mtxWorld;		// ワールドマトリクス
	UINT				m_nCount;
	bool				m_bUse;			// 使用フラグ

	// インスタンシング用
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;		// 頂点バッファ
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// インスタンスバッファ
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;		// インデックスバッファ
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// 頂点宣言
	DWORD							m_dwVtxSize;	// 頂点サイズ
	DWORD							m_dwNumVtx;		// 頂点数
	DWORD							m_dwNumFace;	// 面数
	//D3DXATTRIBUTERANGE*			m_pAttrTable;	// 属性テーブル
	//DWORD							m_dwNumAttr;	// 属性テーブル数

	// VRAM用
	typedef struct
	{
		D3DXVECTOR3 vPos;		// ワールド座標
		float		fSize;		// サイズ
		float		fUse;		// 使用フラグ（シェーダ用）
		float		fRot;		// 回転
		float		fCol;		// 回転
	}INSTANCE;

	// RAM用
	typedef struct : public INSTANCE
	{
		bool bUse;				// 使用フラグ
	}DATA;

	DATA	m_tData[OCTA_MAX];


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
	HRESULT CreateDecl(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh);
};

class OctaData
{
public:
	int		nIdx;
	float	fSize;
	bool	bUse;
	OctaData()
	{
		nIdx = -1;
		fSize = 0.0f;
		bUse = false;
	}
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

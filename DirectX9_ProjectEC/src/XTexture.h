//=============================================================================
//
// 2Dテクスチャ処理 [XTexture.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _XTEXTURE_H_
#define _XTEXTURE_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define XTEXTURE_INIT_SIZE		(100.0f)


//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CXTexture
{
private:
	LPDIRECT3DTEXTURE9			m_pTexture;		// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9		m_pVertex;		// 頂点バッファへのポインタ

#ifdef _DEBUG
	LPSTR						m_pDataPath;		// テクスチャパス
	bool						m_bEdit;
#endif

	// シェーダ関連
	LPD3DXEFFECT				m_pEffect;

	class Divide
	{
	public:
		int x, y;
		Divide()
		{
			x = 1;
			y = 1;
		}
	};

	Divide						m_cDivide;		// テクスチャの分割数
	int							m_nNum;			// インスタンス数
	bool						m_bUse;			// 全体の使用フラグ

	typedef struct XTextureData
	{
	public:
		D3DXVECTOR3		vPos;					// スクリーン座標
		D3DXVECTOR2		vSize;					// サイズ
		D3DXCOLOR		xColor;					// カラー
		int				nTexNum;				// テクスチャナンバー（UV分割）
		float			fBaseAngle;				// 標準角度（回転用）
		float			fRadius;				// 半径（回転用）
		float			fScl;					// スケール
		float			fRot;					// ロット
		bool			bUse;					// 単体の使用フラグ
		//XTextureData()
		//{
		//	// 各データの初期化
		//	vPos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
		//	vSize = D3DXVECTOR2(XTEXTURE_INIT_SIZE, XTEXTURE_INIT_SIZE);
		//	xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//	nTexNum = 0;
		//	fBaseAngle = 0.0f;
		//	fRadius = 0.0f;
		//	fScl = 1.0f;
		//	fRot = 0.0f;
		//	bUse = true;
		//}
	};

	// 頂点の更新
	HRESULT UpdateVertex(void);

	// 頂点の作成
	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
	

public:
	// データのポインタ
	XTextureData* m_pData;

#ifdef _DEBUG
	XTextureData*				m_pDataInit;
#endif

	// コンストラクタ
	CXTexture();
	// デストラクタ
	~CXTexture();

	// 初期化処理
	HRESULT Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPath, LPSTR pDataPath, int nNum, int nDivideX, int nDivideY);
	// 解放処理
	void Release(void);
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
};
#endif
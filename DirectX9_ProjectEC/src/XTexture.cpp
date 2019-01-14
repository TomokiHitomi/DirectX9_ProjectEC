//=============================================================================
//
// 2Dテクスチャ処理 [XTexture.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "XTexture.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
CXTexture::CXTexture(void)
{
	// テクスチャバッファの初期化
	m_pD3DTexture = NULL;
	m_pD3DTexture2 = NULL;

	data.vPos = D3DXVECTOR2(1920 / 2, 1080 / 2);
	data.vSize = D3DXVECTOR2(1920 / 2, 1080 / 2);
	data.nDivide.x = 1;
	data.nDivide.y = 1;
	data.xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	data.nTexNum = 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CXTexture::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPass)
{
	// テクスチャの読み込み
	//if (FAILED(D3DXCreateTextureFromFile(
	//	pDevice,				// デバイス
	//	pTexPass,				// ファイル名
	//	&m_pD3DTexture)))		// 読み込むメモリ（複数なら配列に）
	//{
	//	return E_FAIL;
	//}

	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,				// デバイス
		"data/TEXTURE/カービー.bmp",				// ファイル名
		&m_pD3DTexture)))		// 読み込むメモリ（複数なら配列に）
	{
		return E_FAIL;
	}


	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,				// デバイス
		"data/TEXTURE/すたふぃー.bmp",				// ファイル名
		&m_pD3DTexture2)))		// 読み込むメモリ（複数なら配列に）
	{
		return E_FAIL;
	}

	// 頂点の作成
	MakeVertex();
}

//=============================================================================
// 解放処理
//=============================================================================
void CXTexture::Release(void)
{
	if (m_pD3DTexture != NULL)
	{// テクスチャの開放
		m_pD3DTexture->Release();
		m_pD3DTexture = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CXTexture::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CXTexture::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetTextureStageState(0,
		D3DTSS_TEXCOORDINDEX,		// ブレンド時に0番目のUV座標を参照する
		1
	);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pD3DTexture);
	pDevice->SetTexture(1, m_pD3DTexture2);

	// テクスチャステージ0の設定
	pDevice->SetTextureStageState(0,
		D3DTSS_COLOROP,				// RGBのOPを設定
		D3DTOP_SELECTARG2			// ARG2の入力をそのまま出力
	);

	pDevice->SetTextureStageState(0,
		D3DTSS_COLORARG1,			// ARG1の設定
		D3DTA_CURRENT				// ポリゴンの色を適用
	);

	pDevice->SetTextureStageState(0,
		D3DTSS_COLORARG2,			// ARG2の設定
		D3DTA_TEXTURE				// テクスチャ0番を入力
	);

	pDevice->SetTextureStageState(0,
		D3DTSS_ALPHAOP,				// αのOP設定
		D3DTOP_SELECTARG2			// ARG2のデータを出力
	);


	pDevice->SetTextureStageState(0,
		D3DTSS_ALPHAARG2,			// αのARG2の設定
		D3DTA_CURRENT				// ポリゴンの色を適用
	);

	// テクスチャステージ1の設定
	pDevice->SetTextureStageState(1,
		D3DTSS_COLOROP,				// RGBのOPを設定
		D3DTOP_BLENDTEXTUREALPHA	// テクスチャ1のαを使ってブレンド
	);

	pDevice->SetTextureStageState(1,
		D3DTSS_COLORARG1,			// RGBのOPを設定
		D3DTA_TEXTURE				// テクスチャ1のテクスチャを入力
	);

	pDevice->SetTextureStageState(1,
		D3DTSS_COLORARG2,			// ARG2の設定
		D3DTA_CURRENT				// 前のステージを出力
	);

	pDevice->SetTextureStageState(1,
		D3DTSS_ALPHAOP,				// αのOP設定
		D3DTOP_SELECTARG2			// ARG2のデータを出力
	);

	pDevice->SetTextureStageState(1,
		D3DTSS_ALPHAARG1,			// αのARG1の設定
		D3DTA_CURRENT				// 前のステージを出力
	);

	pDevice->SetTextureStageState(1,
		D3DTSS_ALPHAARG2,			// αのARG2の設定
		D3DTA_TEXTURE				// テクスチャ1番を入力
	);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, NUM_POLYGON, m_VertexWk, sizeof(VERTEX_2D));
	

	//pDevice->SetTextureStageState(1,
	//	D3DTSS_COLOROP,				// RGBのOPを設定
	//	D3DTOP_DISABLE				// 指定ステージ以降は無効
	//);

	//pDevice->SetTextureStageState(1,
	//	D3DTSS_ALPHAOP,				// αのOP設定
	//	D3DTOP_DISABLE				// 指定ステージ以降は無効
	//);

	//pDevice->SetTextureStageState(0,
	//	D3DTSS_COLORARG1,			// ARG1の設定
	//	D3DTA_TEXTURE				// テクスチャ0番を入力
	//);

	//pDevice->SetTextureStageState(0,
	//	D3DTSS_COLORARG2,			// ARG2の設定
	//	D3DTA_CURRENT				// ポリゴンの色を適用
	//);

	//pDevice->SetTextureStageState(0,
	//	D3DTSS_TEXCOORDINDEX,		// ブレンド時に0番目のUV座標を参照する
	//	0
	//);


	//pDevice->SetTextureStageState(1,
	//	D3DTSS_TEXCOORDINDEX,		// ブレンド時に0番目のUV座標を参照する
	//	1
	//);
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT CXTexture::MakeVertex(void)
{
	// 頂点座標の設定
	m_VertexWk[0].vtx = D3DXVECTOR3(data.vPos.x - data.vSize.x, data.vPos.y - data.vSize.y, 0.0f);
	m_VertexWk[1].vtx = D3DXVECTOR3(data.vPos.x + data.vSize.x, data.vPos.y - data.vSize.y, 0.0f);
	m_VertexWk[2].vtx = D3DXVECTOR3(data.vPos.x - data.vSize.x, data.vPos.y + data.vSize.y, 0.0f);
	m_VertexWk[3].vtx = D3DXVECTOR3(data.vPos.x + data.vSize.x, data.vPos.y + data.vSize.y, 0.0f);

	// rhwの設定
	m_VertexWk[0].rhw =
	m_VertexWk[1].rhw =
	m_VertexWk[2].rhw =
	m_VertexWk[3].rhw = 1.0f;

	// 反射光の設定
	m_VertexWk[0].diffuse =
	m_VertexWk[1].diffuse =
	m_VertexWk[2].diffuse =
	m_VertexWk[3].diffuse = data.xColor;

	// テクスチャ座標の設定
	int x = data.nTexNum % data.nDivide.x;
	int y = data.nTexNum / data.nDivide.x;
	float sizeX = 1.0f / data.nDivide.x;
	float sizeY = 1.0f / data.nDivide.y;
	m_VertexWk[0].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
	m_VertexWk[1].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
	m_VertexWk[2].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
	m_VertexWk[3].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);
	return S_OK;
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT CXTexture::SetVertex(void)
{
	// 頂点座標の設定
	m_VertexWk[0].vtx = D3DXVECTOR3(data.vPos.x - data.vSize.x, data.vPos.y - data.vSize.y, 0.0f);
	m_VertexWk[1].vtx = D3DXVECTOR3(data.vPos.x + data.vSize.x, data.vPos.y - data.vSize.y, 0.0f);
	m_VertexWk[2].vtx = D3DXVECTOR3(data.vPos.x - data.vSize.x, data.vPos.y + data.vSize.y, 0.0f);
	m_VertexWk[3].vtx = D3DXVECTOR3(data.vPos.x + data.vSize.x, data.vPos.y + data.vSize.y, 0.0f);
	return S_OK;
}

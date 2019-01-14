//=============================================================================
//
// ステンシル処理 [stencil.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "stencil.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Stencil::Stencil(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::STENCIL, Priority::Middle, Priority::Lowest);

	// 各プロパティの初期化
	m_bUse = true;
	m_CXTexture.data.vPos = D3DXVECTOR2(1920 / 2, 1080 / 2);
	m_CXTexture.data.nTexNum = 0;
	m_CXTexture.data.vSize = D3DXVECTOR2(200.0f, 200.0f);
	m_CXTexture.data.xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_CXTexture.data.nDivide.x = 1;
	m_CXTexture.data.nDivide.y = 1;

	// テクスチャの初期化
	m_CXTexture.Init(pDevice, (LPSTR)STENCIL_TEXTURE);

	//m_CXTexture2.data.vPos = D3DXVECTOR2(1920 / 2, 1080 / 2);
	//m_CXTexture2.data.vSize = D3DXVECTOR2(1920 / 2, 1080 / 2);
	//m_CXTexture2.data.nDivide.x = 1;
	//m_CXTexture2.data.nDivide.y = 1;
	//m_CXTexture2.data.xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//m_CXTexture2.data.nTexNum = 0;

	// テクスチャの初期化
	//m_CXTexture2.Init(pDevice, STENCIL_TEXTURE2);
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Stencil::~Stencil(void)
{
	m_CXTexture.Release();
	//m_CXTexture2.Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void Stencil::Update(void)
{
	//m_CXTexture.data.vPos.x++;
	//m_CXTexture.SetVertex();
}

//=============================================================================
// 描画処理
//=============================================================================
void Stencil::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	if (m_bUse)
	{
		//// Zバッファへの書き込み禁止
		//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		//// ステンシルテスト許可
		//pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		//// ステンシルマスクの設定 データはそのまま使う
		//pDevice->SetRenderState(D3DRS_STENCILMASK, 0xff);
		//// ステンシルテストに強制的に合格（必ず書き込む）
		//pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		//// バックバッファへRGB値を書き込まない
		//pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00);
		//// ステンシルテストに不合格の場合はステンシル値に何もしない
		//pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILCAPS_KEEP);
		//// Zバッファに不合格の場合はステンシル値になにもしない
		//pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILCAPS_KEEP);
		//// ステンシルテストに合格の場合ステンシル値を1増やす
		//pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILCAPS_INCRSAT);

		// ステンシル描画
		m_CXTexture.Draw();

		//// ステンシルバッファの値と比較する参照値
		//pDevice->SetRenderState(D3DRS_STENCILREF, 0x09);
		//// 比較関数、条件が真の時ステンシルテスト合格
		//pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
		//// ステンシルテストに合格した場合ステンシル値に何もしない
		//pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILCAPS_KEEP);
		//// フレームバッファにRGBを書き込めるようにする
		//pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
		//	D3DCOLORWRITEENABLE_RED |
		//	D3DCOLORWRITEENABLE_GREEN |
		//	D3DCOLORWRITEENABLE_BLUE |
		//	D3DCOLORWRITEENABLE_ALPHA);
		//// Zバッファへの書き込みを許可する
		//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		//m_CXTexture2.Draw();
	}
}
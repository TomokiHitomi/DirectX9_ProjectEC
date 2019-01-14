//=============================================================================
//
// ライト処理 [light.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DLIGHT9		g_aLight[LIGHT_MAX];		// ライト情報
Light			LightManager::cLight[LightManager::Max];


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT LightManager::Init(void)
{
	// ライトのタイプ設定
	cLight[Main].type = Light::DIRECTIONAL;

	// 拡散光の設定
	cLight[Main].value.Diffuse = LIGHT_DIFFUSE;
	// 環境光の設定
	cLight[Main].value.Ambient = LIGHT_AMBIENT;
	// 反射光の設定
	cLight[Main].value.Specular = LIGHT_SPECULAR;

	// 座標の設定（DIRECTIONALライトは使わない）
	cLight[Main].value.Position = LIGHT_POSITION;

	// 平行光源のベクトルを設定（POINTライトは使わない）
	cLight[Main].value.Direction = LIGHT_DIRECTION;
	// 平行光源のベクトルを正規化
	D3DXVec3Normalize(
		(D3DXVECTOR3*)&cLight[Main].value.Direction,
		(D3DXVECTOR3*)&cLight[Main].value.Direction);
	return S_OK;
}


//=============================================================================
// デストラクタ
//=============================================================================



//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;

	/********** ライト0 **********/

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));

	// ライト0のタイプの設定
	g_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	// ライト0の拡散光の設定
	g_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト0の鏡面反射光の設定
	g_aLight[0].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト0の環境光の設定
	g_aLight[0].Ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);

	// ライト0の方向の設定
	vecDir = LIGHT_DIRECTION;
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ライト0をレンダリングパイプラインに設定
	pDevice->SetLight(0, &g_aLight[0]);

	// ライト0を使用状態に
	pDevice->LightEnable(0, TRUE);


	///********** ライト1 **********/

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));

	// ライト1のタイプの設定
	g_aLight[1].Type = D3DLIGHT_DIRECTIONAL;

	// ライト1の拡散光の設定
	g_aLight[1].Diffuse = D3DXCOLOR(LIGHT_DIFFUSE_1, LIGHT_DIFFUSE_1, LIGHT_DIFFUSE_1, 1.0f);

	// ライト1の鏡面反射光の設定
	//	g_aLight[1].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト1の環境光の設定
	//	g_aLight[1].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト1の方向の設定
	vecDir = D3DXVECTOR3(0.0f, 0.5f, 0.5f);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[1].Direction, &vecDir);

	// ライト1をレンダリングパイプラインに設定
	pDevice->SetLight(1, &g_aLight[1]);

	// ライト1を使用状態に
	pDevice->LightEnable(1, FALSE);


	///********** ライト2 **********/

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ライト2のタイプの設定
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライト2の拡散光の設定
	g_aLight[2].Diffuse = D3DXCOLOR(LIGHT_DIFFUSE_2, LIGHT_DIFFUSE_2, LIGHT_DIFFUSE_2, 1.0f);

	// ライト2の鏡面反射光の設定
	//	g_aLight[2].Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト2の環境光の設定
	//	g_aLight[2].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト2の方向の設定
	vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[2].Direction, &vecDir);

	// ライト2をレンダリングパイプラインに設定
	pDevice->SetLight(2, &g_aLight[2]);

	// ライト2を使用状態に
	pDevice->LightEnable(2, FALSE);

	// ライティングモードをON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================================================================
// ライト設定処理
//=============================================================================
void SetLight(int nLight, bool bFlag)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->LightEnable(nLight, bFlag);
}

//=============================================================================
// ライト取得処理
//=============================================================================
D3DLIGHT9 GetLight(int nLight)
{
	return g_aLight[nLight];
}
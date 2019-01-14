//=============================================================================
//
// モデル処理 [XModel.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "XModel.h"
#include "shader.h"
#include "light.h"
#include "camera.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
CXModel::CXModel(void)
{
	// モデル関係の初期化
	ppTexture = NULL;
	//ppMat = NULL;
	pMesh = NULL;
	pBuffMat = NULL;
	dwNumMat = 0;
	bLight = true;

	// シェーダポインタの初期化
	pEffect = NULL;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CXModel::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pMeshPass, LPSTR pTexPass)
{
	// モデルデータ読み込み
	if (FAILED(D3DXLoadMeshFromX(
		pMeshPass,				// モデルデータ
		D3DXMESH_SYSTEMMEM,		// 使用するメモリのオプション
		pDevice,				// デバイス
		NULL,					// 未使用
		&pBuffMat,				// マテリアルデータ
		NULL,					// 未使用
		&dwNumMat,				// D3DXMATERIAL構造体の数
		&pMesh)))				// メッシュデータへのポインタ
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました", pMeshPass, MB_OK);
		return E_FAIL;
	}

	if (pTexPass == NULL)
	{
		// マテリアル情報を取り出す
		D3DXMATERIAL*	d3Mat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();
		//ppMat = new D3DMATERIAL9[dwNumMat];				// メッシュ情報を確保
		ppTexture = new LPDIRECT3DTEXTURE9[dwNumMat];	// テクスチャを確保
		for (int i = 0; i < dwNumMat; i++)
		{
			//ppMat[i] = d3Mat[i].MatD3D;			// マテリアル情報セット
			//ppMat[i].Ambient = ppMat[i].Diffuse;// 環境光初期化
			ppTexture[i] = NULL;	// テクスチャ初期化

			// 使用しているテクスチャがあれば読み込む
			if (d3Mat[i].pTextureFilename != NULL &&
				lstrlen(d3Mat[i].pTextureFilename) > 0)
			{
				// テクスチャ読み込み
				if (FAILED(D3DXCreateTextureFromFile(
					pDevice,
					d3Mat[i].pTextureFilename,
					&ppTexture[i])))
				{
					MessageBox(NULL, "Xファイルのテクスチャ読み込みに失敗しました", pTexPass, MB_OK);
					return E_FAIL;
				}
			}
		}
	}
	else
	{
		ppTexture = new LPDIRECT3DTEXTURE9[1];	// テクスチャを確保

		// テクスチャ読み込み
		if (FAILED(D3DXCreateTextureFromFile(
			pDevice,
			pTexPass,
			&ppTexture[0])))
		{
			MessageBox(NULL, "Xファイルのテクスチャ読み込みに失敗しました", pTexPass, MB_OK);
			return E_FAIL;
		}
	}

	// シェーダのアドレスを取得
	if (pEffect == NULL) pEffect = ShaderManager::GetEffect(ShaderManager::XMODEL);

	// マテリアル情報に対するポインタの取得
	D3DXMATERIAL *pMat;
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (unsigned int i = 0; i < (int)dwNumMat; i++)
	{
		// 環境光初期化
		//pMat[i].MatD3D.Ambient = pMat[i].MatD3D.Diffuse;
		pMat[i].MatD3D.Ambient.r = 0.5f;
		pMat[i].MatD3D.Ambient.g = 0.5f;
		pMat[i].MatD3D.Ambient.b = 0.5f;
		pMat[i].MatD3D.Ambient.a = 1.0f;
	}
}

//=============================================================================
// マテリアル設定処理（スペキュラ）
//=============================================================================
void CXModel::SetMaterialSpecular(float speclar, float power)
{
	// マテリアル情報に対するポインタの取得
	D3DXMATERIAL *pMat;
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (unsigned int i = 0; i < (int)dwNumMat; i++)
	{
		// 環境光初期化
		//pMat[i].MatD3D.Ambient = pMat[i].MatD3D.Diffuse;
		pMat[i].MatD3D.Specular.r = speclar;
		pMat[i].MatD3D.Specular.g = speclar;
		pMat[i].MatD3D.Specular.b = speclar;
		pMat[i].MatD3D.Specular.a = 1.0f;
		pMat[i].MatD3D.Power = power;
	}
}

//=============================================================================
// 解放処理
//=============================================================================
void CXModel::Release(void)
{
	for (unsigned int i = 0; i < dwNumMat; i++)
	{
		SAFE_RELEASE(ppTexture[i]);
	}
	SAFE_DELETE_ARRAY(ppTexture);
	//SAFE_DELETE_ARRAY(ppMat);

	SAFE_RELEASE(pMesh);
	SAFE_RELEASE(pBuffMat);
}

//=============================================================================
// 更新処理
//=============================================================================
void CXModel::Update(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void CXModel::Draw(D3DXMATRIX mtxWorld)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATERIAL *pMat;
	D3DMATERIAL9 pMatDef;
	HRESULT hr;

	// ビュー・プロジェクション行列を取得
	D3DXMATRIX mtxView, mtxProjection;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	// マテリアル情報に対するポインタの取得
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	bool bSetEffect = false;

	//メッシュの描画
	for (int i = 0; i < (int)dwNumMat; i++)
	{
		// 使用するテクニックを定義
		if (bLight)
		{	// ライトON
			if (ppTexture[i]) hr = pEffect->SetTechnique("LIGHT_ON_TEX");
			else hr = pEffect->SetTechnique("LIGHT_ON");
		}
		else
		{	// ライトOFF
			if (ppTexture[i]) hr = pEffect->SetTechnique("LIGHT_OFF_TEX");
			else hr = pEffect->SetTechnique("LIGHT_OFF");
		}
		// エラー
		if (FAILED(hr)) MessageBox(NULL, "テクニックの定義に失敗しました。", "XMODEL", MB_OK);

		if (!bSetEffect)
		{
			// 必要な行列情報をセット
			pEffect->SetMatrix("proj", &mtxProjection);
			pEffect->SetMatrix("view", &mtxView);
			pEffect->SetMatrix("world", &mtxWorld);

			if (bLight)
			{	// ライトON
				Camera* pCamera = CameraManager::GetCameraNow();
				D3DXVECTOR4 eyeTmp = D3DXVECTOR4(pCamera->GetEye(), 0.0f);
				if (FAILED(pEffect->SetVector("eye", &eyeTmp)))
				{
					// エラー
					MessageBox(NULL, "カメラEye情報のセットに失敗しました。", "eye", MB_OK);
				}
				// ライト情報を取得
				Light* pLight = LightManager::GetLightAdr(LightManager::Main);
				// ライト情報をセット
				if (FAILED(pEffect->SetValue("lt", &pLight->value, sizeof(Light::LIGHTVALUE))))
				{
					// エラー
					MessageBox(NULL, "ライト情報のセットに失敗しました。", "lt", MB_OK);
				}
				//SetShaderLight(pEffect, GetLight(0));
			}
		}

		// マテリアルをセット
		pEffect->SetValue("mat", &pMat[i].MatD3D, sizeof(D3DMATERIAL9));

		// テクスチャをセット
		pEffect->SetTexture("tex", ppTexture[i]);

		// 結果を確定させる
		pEffect->CommitChanges();

		// シェーダーの開始、numPassには指定してあるテクニックに定義してあるpassの数が変える
		UINT numPass = 0;
		pEffect->Begin(&numPass, 0);
		for (unsigned int j = 0; j < numPass; j++)
		{
			// パスを指定して開始
			pEffect->BeginPass(0);

			// 描画
			pMesh->DrawSubset(i);

			// シェーダーパスを終了
			pEffect->EndPass();
		}
		// シェーダーを終了
		pEffect->End();
	}

	// 固定機能に戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
//=============================================================================
//
// バレットエフェクト処理 [bulleteffect.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "bulleteffect.h"
#include "shader.h"
#include "light.h"
#include "camera.h"
#include "calculate.h"
#include "tool.h"

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
LPCTSTR g_lpBulletEffectEH[] = {
	"tex",
	"proj",
	"view",
	"world",
	"colorpallet"
};


//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
BulletEffect::BulletEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// マトリクスの初期化
	InitMatrix();

	// インスタンシング用データの初期化
	InitInst();

	// 管理用データの初期化
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		m_tData[i].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tData[i].fSize = 0.0f;
		m_tData[i].fUse = 0.0f;
		m_tData[i].fCol = 0.0f;

		m_tData[i].nCountMax = 0;
		m_tData[i].nCount = 0;
		m_tData[i].bUse = false;
	}

	// 使用フラグの初期化
	m_bUse = true;
	m_nCount = 0;

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		BULLETEFFECT_TEX,
		&m_pTexture)))
	{
		// エラー
		MessageBox(NULL, "テクスチャの読み込みに失敗しました", BULLETEFFECT_TEX, MB_OK);
		//return S_FALSE;
	}

	// シェーダポインタの初期化・取得
	pEffect = NULL;
	pEffect = ShaderManager::GetEffect(ShaderManager::BULLET);

	for (UINT i = 0; i < EH_MAX; i++)
	{
		// シェーダのハンドルを初期化
		hEffectHandle[i] = NULL;
		// シェーダのハンドルを取得
		hEffectHandle[i] = pEffect->GetParameterByName(NULL, g_lpBulletEffectEH[i]);
		// シェーダのハンドル取得のエラーチェック
		if (hEffectHandle[i] == NULL) MessageBox(NULL, "シェーダハンドルの取得失敗", g_lpBulletEffectEH[i], MB_OK);
	}

	// メッシュの生成
	CreateMesh(pDevice);
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
BulletEffect::~BulletEffect(void)
{
	SAFE_RELEASE(m_pTexture);		// テクスチャ
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pInstBuff);
	SAFE_RELEASE(m_pIdxBuff);
	SAFE_RELEASE(m_pDecl);

	//SAFE_DELETE_ARRAY(m_pAttrTable);
}

//=============================================================================
// 更新処理
//=============================================================================
void BulletEffect::Update(void)
{
	SetInst();
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("BulletEffect");
	if (bGui)
	{
		ImGui::Text("Count [%d]\n", m_nCount);

		//// 管理用データの初期化
		//for (UINT i = 0; i <= m_nCount; i++)
		//{
		//	ImGui::Text("No[%2d] Pos [%6.0f.%6.0f,%6.0f] Size[%3.0f] Rot[%3.2f] Use[%d]\n",
		//		i,m_tData[i].vPos.x, m_tData[i].vPos.y, m_tData[i].vPos.z,
		//		m_tData[i].fSize, m_tData[i].fRot, m_tData[i].bUse
		//	);
		//}
		ImGui::TreePop();
	}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void BulletEffect::Draw(void)
{
	if (m_nCount > 0)
	{
		D3DXMATRIX mtxWorld, mtxView, mtxProjection;

		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// UpがYのビュー行列を取得
		mtxView = CameraManager::GetCameraNow()->GetMtxViewUpY();
		// プロジェクション行列を取得
		pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

		// ワールドマトリクスの初期化
		D3DXMatrixIdentity(&mtxWorld);

		// ビュー行列の逆行列を格納（ビルボード化）
		mtxWorld._11 = mtxView._11;
		mtxWorld._12 = mtxView._21;
		mtxWorld._13 = mtxView._31;
		mtxWorld._21 = mtxView._12;
		mtxWorld._22 = mtxView._22;
		mtxWorld._23 = mtxView._32;
		mtxWorld._31 = mtxView._13;
		mtxWorld._32 = mtxView._23;
		mtxWorld._33 = mtxView._33;

		// ビュー行列を再取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// インスタンス宣言
		pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nCount);
		pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

		// 頂点とインデックスを設定
		pDevice->SetVertexDeclaration(m_pDecl);
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX));			// 頂点バッファ
		pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// インスタンスバッファ
		pDevice->SetIndices(m_pIdxBuff);								// インデックスバッファ

			// 使用するテクニックを定義
		if (FAILED(pEffect->SetTechnique("Tec01")))
		{
			// エラー
			MessageBox(NULL, "テクニックの定義に失敗しました", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// テクスチャをセット
		SHR(pEffect->SetTexture(hEffectHandle[EH_TEX], m_pTexture), g_lpBulletEffectEH[EH_TEX]);

		// テクスチャの分割数をセット
		//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_X], (FLOAT)CLOUD_TEXTURE_DIVIDE_X);
		//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_Y], (FLOAT)CLOUD_TEXTURE_DIVIDE_Y);

		// 必要な行列情報をセット
		SHR(pEffect->SetMatrix(hEffectHandle[EH_PROJ], &mtxProjection), g_lpBulletEffectEH[EH_PROJ]);
		SHR(pEffect->SetMatrix(hEffectHandle[EH_VIEW], &mtxView), g_lpBulletEffectEH[EH_VIEW]);
		SHR(pEffect->SetMatrix(hEffectHandle[EH_WORLD], &mtxWorld), g_lpBulletEffectEH[EH_WORLD]);

		// カラーパレットをセット
		SHR(pEffect->SetVectorArray(
			hEffectHandle[EH_COLORPALLET],
			Color::xColor,
			COLOR_PALLET_MAX),
			g_lpBulletEffectEH[EH_COLORPALLET]);

		// 結果を確定させる
		pEffect->CommitChanges();

		// シェーダーの開始、numPassには指定してあるテクニックに定義してあるpassの数が変える
		UINT numPass = 0;
		pEffect->Begin(&numPass, 0);
		for (UINT i = 0; i < numPass; i++)
		{
			// パスを指定して開始
			pEffect->BeginPass(i);

			// 描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			// シェーダーパスを終了
			pEffect->EndPass();
		}

		// シェーダーを終了
		pEffect->End();

		// インスタンス宣言を標準に戻す
		pDevice->SetStreamSourceFreq(0, 1);
		pDevice->SetStreamSourceFreq(1, 1);

		// 固定機能に戻す
		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);
	}
}

//=============================================================================
// 使用状態にする
//=============================================================================
int BulletEffect::Set(D3DXVECTOR3 vPos, float fSize, float fCol, int nCount)
{
	// 管理用データの初期化
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		if (!m_tData[i].bUse)
		{
			m_tData[i].fSize = fSize;
			m_tData[i].fUse = 1.0f;
			m_tData[i].vPos = vPos;
			m_tData[i].fCol = fCol;

			m_tData[i].nCountMax = nCount;
			m_tData[i].nCount = nCount;
			m_tData[i].bUse = true;
			return i;
		}
	}
	return -1;
}

////=============================================================================
//// カラーをセット
////=============================================================================
//void BulletEffect::SetColor(int nIdx, float fCol)
//{
//	if (nIdx < 0)return;
//	m_tData[nIdx].fCol = fCol;
//	return;
//}
//
////=============================================================================
//// 座標をセット
////=============================================================================
//void BulletEffect::SetPos(int nIdx, D3DXVECTOR3 vPos)
//{
//	if (nIdx < 0)return;
//	m_tData[nIdx].vPos = vPos;
//	return;
//}
//
//=============================================================================
// 解放
//=============================================================================
void BulletEffect::Release(int nIdx)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_tData[nIdx].fSize = 0.0f;
	m_tData[nIdx].fCol = 0.0f;

	m_tData[nIdx].fUse = 0.0f;
	m_tData[nIdx].bUse = false;
	m_tData[nIdx].nCountMax = 0;
	m_tData[nIdx].nCount = 0;
	return;
}

//=============================================================================
// インスタンシング用データの初期化
//=============================================================================
HRESULT BulletEffect::InitInst(void)
{
	m_pVtxBuff = NULL;	// 頂点バッファ
	m_pInstBuff = NULL;	// インスタンシングバッファ
	m_pIdxBuff = NULL;	// インデックスバッファ
	m_pDecl = NULL;		// 頂点宣言
	return S_OK;
}

//=============================================================================
// マトリクスの初期化
//=============================================================================
HRESULT BulletEffect::InitMatrix(void)
{
	// マトリクスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	return S_OK;
}

//=============================================================================
// メッシュの生成
//=============================================================================
HRESULT BulletEffect::CreateMesh(LPDIRECT3DDEVICE9 pDevice)
{
	/***** 頂点バッファ設定 *****/

	// 頂点バッファ生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		0,									// 頂点バッファの使用法　
		0,									// 使用する頂点フォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&m_pVtxBuff,							// 頂点バッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR4(-1.0f, 1.0f, 0.0f, 1.0f);
		pVtx[1].vtx = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
		pVtx[2].vtx = D3DXVECTOR4(-1.0f, -1.0f, 0.0f, 1.0f);
		pVtx[3].vtx = D3DXVECTOR4(1.0f, -1.0f, 0.0f, 1.0f);

		// テクスチャ座標の設定
		int x = 0 % BULLETEFFECT_TEXTURE_DIVIDE_X;
		int y = 0 / BULLETEFFECT_TEXTURE_DIVIDE_X;
		float sizeX = 1.0f / BULLETEFFECT_TEXTURE_DIVIDE_X;
		float sizeY = 1.0f / BULLETEFFECT_TEXTURE_DIVIDE_Y;

		pVtx[0].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
		pVtx[1].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
		pVtx[2].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
		pVtx[3].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

		//// 頂点カラーの設定
		//pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// 頂点データをアンロックする
		m_pVtxBuff->Unlock();
	}


	/***** インデックスバッファ設定 *****/

	WORD wIndex[6] = { 0, 1, 2, 2, 1, 3 };

	// 座標バッファ生成
	if (FAILED(pDevice->CreateIndexBuffer(
		sizeof(wIndex),						// 頂点データ用に確保するバッファサイズ(バイト単位)
		0,									// 頂点バッファの使用法　
		D3DFMT_INDEX16,						// 使用する頂点フォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&m_pIdxBuff,							// 頂点バッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	{// 座標バッファの中身を埋める
		WORD *pIdx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		// 頂点座標の設定
		for (unsigned int i = 0; i < 6; i++, pIdx++)
		{
			*pIdx = wIndex[i];
		}

		// 頂点データをアンロックする
		m_pIdxBuff->Unlock();
	}


	// インスタンシングバッファ生成 + データの代入
	if (FAILED(CreateInst(pDevice)))
	{
		MessageBox(NULL, "インスタンシング用バッファの生成に失敗", "Bullet", MB_OK);
		//return E_FAIL;
	}

	// 頂点宣言を作成
	if (FAILED(CreateDecl(pDevice)))
	{
		MessageBox(NULL, "頂点宣言の作成に失敗", "Bullet", MB_OK);
		//return E_FAIL;
	}

	
	return S_OK;
}

//=============================================================================
// インスタンシングバッファ生成 + データの代入
//=============================================================================
HRESULT BulletEffect::CreateInst(LPDIRECT3DDEVICE9 pDevice)
{
	// インスタンシングバッファ生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * BULLETEFFECT_MAX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		0,									// 頂点バッファの使用法　
		0,									// 使用する頂点フォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&m_pInstBuff,						// 頂点バッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	// インスタンシングバッファにデータを代入
	SetInst();
	return S_OK;
}

//=============================================================================
// インスタンシングバッファにデータの代入
//=============================================================================
HRESULT BulletEffect::SetInst(void)
{
	INSTANCE *pInst;

	// 座標データの範囲をロックし、頂点バッファへのポインタを取得
	m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

	// 座標の設定
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		pInst->vPos = m_tData[i].vPos;
		pInst->fSize = m_tData[i].fSize;
		pInst->fUse = m_tData[i].fUse;
		pInst->fCol = m_tData[i].fCol;

		if (m_tData[i].bUse)
		{
			m_tData[i].nCount--;
			if (m_tData[i].nCount > 0)
			{
				pInst->fSize = m_tData[i].fSize * ((float)m_tData[i].nCount / (float)m_tData[i].nCountMax);
				m_nCount = i + 1;
			}
			else
			{
				pInst->fSize = m_tData[i].fSize * ((float)m_tData[i].nCount / (float)m_tData[i].nCountMax);
				Release(i);
			}
		}
		pInst++;
	}

	// 座標データをアンロックする
	m_pInstBuff->Unlock();
	return S_OK;
}

//=============================================================================
// 頂点シェーダ宣言の作成
//=============================================================================
HRESULT BulletEffect::CreateDecl(LPDIRECT3DDEVICE9 pDevice)
{
	// インスタンシングデータと合わせて再定義
	D3DVERTEXELEMENT9 declElems[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },		// VTX
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// UV

		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// ワールド位置
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// サイズ
		{ 1, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// 使用フラグ
		{ 1, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },		// カラーインデックス
		D3DDECL_END()
	};

	// 頂点要素から頂点シェーダ宣言を作成
	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);
	return S_OK;
}
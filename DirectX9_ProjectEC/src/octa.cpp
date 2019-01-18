//=============================================================================
//
// 八面体処理 [debugsphere.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "octa.h"
#include "shader.h"
#include "light.h"
#include "camera.h"
#include "calculate.h"

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
Octa::Octa(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// マトリクスの初期化
	InitMatrix();

	// インスタンシング用データの初期化
	InitInst();

	// 管理用データの初期化
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		m_tData[i].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tData[i].fSize = 0.0f;
		m_tData[i].fUse = 0.0f;
		m_tData[i].fRot = ((rand() % 618) * 0.01f);
		m_tData[i].vColor = SetColorPalletRandom();

		m_tData[i].bUse = false;
	}

	// 使用フラグの初期化
	m_bUse = true;
	m_nCount = 0;

	// シェーダポインタの初期化・取得
	pEffect = NULL;
	pEffect = ShaderManager::GetEffect(ShaderManager::OCTA);

	// メッシュの生成
	CreateMesh(pDevice);
}

//=============================================================================
// 更新処理
//=============================================================================
void Octa::Update(void)
{
	SetInst();
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Octa");
	if (bGui)
	{
		ImGui::Text("Count [%d]\n", m_nCount);

		// 管理用データの初期化
		for (UINT i = 0; i <= m_nCount; i++)
		{
			ImGui::Text("No[%2d] Pos [%6.0f.%6.0f,%6.0f] Size[%3.0f] Rot[%3.2] Use[%d]\n",
				i,m_tData[i].vPos.x, m_tData[i].vPos.y, m_tData[i].vPos.z,
				m_tData[i].fSize, m_tData[i].fRot, m_tData[i].bUse
			);
		}
		ImGui::TreePop();
	}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void Octa::Draw(void)
{
	if (m_nCount > 0)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// ビュー・プロジェクション行列を取得
		D3DXMATRIX mtxView, mtxProjection;
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);
		pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

		// インスタンス宣言
		pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nCount);
		pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

		// 頂点とインデックスを設定
		pDevice->SetVertexDeclaration(m_pDecl);
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, (UINT)m_dwVtxSize);			// 頂点バッファ
		pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// インスタンスバッファ
		pDevice->SetIndices(m_pIdxBuff);								// インデックスバッファ

			// 使用するテクニックを定義
		if (FAILED(pEffect->SetTechnique("Tec01")))
		{
			// エラー
			MessageBox(NULL, "テクニックの定義に失敗しました", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// 必要な行列情報をセット
		pEffect->SetMatrix("proj", &mtxProjection);
		pEffect->SetMatrix("view", &mtxView);
		pEffect->SetMatrix("world", &m_mtxWorld);

		// カメラ情報を取得
		Camera* pCamera = CameraManager::GetCameraNow();
		D3DXVECTOR4 eyeTmp = D3DXVECTOR4(pCamera->GetEye(), 0.0f);
		// カメラ視点をセット
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
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVtx, 0, m_dwNumFace);

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
int Octa::Set(float fSize)
{
	// 管理用データの初期化
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		if (!m_tData[i].bUse)
		{
			m_tData[i].fSize = fSize;
			m_tData[i].fUse = 1.0f;
			m_tData[i].bUse = true;
			return i;
		}
	}
	return -1;
}

//=============================================================================
// カラーをセット
//=============================================================================
void Octa::SetColor(int nIdx, D3DCOLOR xColor)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vColor = xColor;
	return;
}

//=============================================================================
// 座標をセット
//=============================================================================
void Octa::SetPos(int nIdx, D3DXVECTOR3 vPos)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vPos = vPos;
	return;
}

//=============================================================================
// 回転を加算
//=============================================================================
void Octa::AddRot(int nIdx, float fRot)
{
	if (nIdx < 0)return;
	m_tData[nIdx].fRot += fRot;
	return;
}

//=============================================================================
// 解放
//=============================================================================
void Octa::Release(int nIdx)
{
	if (nIdx < 0)return;
	m_tData[nIdx].fUse = 0.0f;
	m_tData[nIdx].bUse = false;
	return;
}

//=============================================================================
// インスタンシング用データの初期化
//=============================================================================
HRESULT Octa::InitInst(void)
{
	m_pVtxBuff = NULL;	// 頂点バッファ
	m_pInstBuff = NULL;	// インスタンシングバッファ
	m_pIdxBuff = NULL;	// インデックスバッファ
	m_pDecl = NULL;		// 頂点宣言

	m_dwVtxSize = 0;
	m_dwNumVtx = 0;
	m_dwNumFace = 0;

	//m_pAttrTable = NULL;
	//m_dwNumAttr = 0;
	return S_OK;
}

//=============================================================================
// マトリクスの初期化
//=============================================================================
HRESULT Octa::InitMatrix(void)
{
	// マトリクスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	return S_OK;
}

//=============================================================================
// メッシュの生成
//=============================================================================
HRESULT Octa::CreateMesh(LPDIRECT3DDEVICE9 pDevice)
{
	// メッシュ情報へのポインタ
	LPD3DXMESH pMesh = NULL;
	LPD3DXMESH pNewMesh = NULL;

	// モデルデータ読み込み
	if (FAILED(D3DXLoadMeshFromX(
		OCTA_MODEL,				// モデルデータ
		D3DXMESH_SYSTEMMEM,		// 使用するメモリのオプション
		pDevice,				// デバイス
		NULL,					// 未使用
		NULL,				// マテリアルデータ
		NULL,					// 未使用
		NULL,				// D3DXMATERIAL構造体の数
		&pMesh)))				// メッシュデータへのポインタ
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました", OCTA_MODEL, MB_OK);
		return E_FAIL;
	}

	// 頂点座標のみのメッシュをコピー（法線が不要）
	pMesh->CloneMeshFVF(pMesh->GetOptions(), D3DFVF_XYZ | D3DFVF_NORMAL, pDevice, &pNewMesh);
	// メッシュを解放
	pMesh->Release();

	// メッシュ情報の取得
	GetMeshData(pNewMesh);

	// 頂点シェーダ宣言の作成
	CreateDecl(pDevice, pNewMesh);

	// インスタンシングバッファ生成 + データの代入
	if (FAILED(CreateInst(pDevice)))
	{
		MessageBox(NULL, "インスタンシング用バッファの生成に失敗", "Octa", MB_OK);
		//return E_FAIL;
	}
	return S_OK;
}

//=============================================================================
// メッシュ情報の取得
//=============================================================================
HRESULT Octa::GetMeshData(LPD3DXMESH pMesh)
{
	// メッシュの頂点バッファ・インデックスバッファを取得
	pMesh->GetVertexBuffer(&m_pVtxBuff);
	pMesh->GetIndexBuffer(&m_pIdxBuff);

	// 頂点サイズ・頂点数・面数を取得
	m_dwVtxSize = pMesh->GetNumBytesPerVertex();
	m_dwNumVtx = pMesh->GetNumVertices();
	m_dwNumFace = pMesh->GetNumFaces();

	///// メッシュの属性テーブルを取得
	//// メッシュの属性テーブル数を取得
	//pNewMesh->GetAttributeTable(NULL, &m_dwNumAttr);
	//// 属性テーブルの生成
	//m_pAttrTable = new D3DXATTRIBUTERANGE[m_dwNumAttr];
	//// メッシュの属性テーブルを取得
	//pNewMesh->GetAttributeTable(m_pAttrTable, &m_dwNumAttr);
	return S_OK;
}

//=============================================================================
// インスタンシングバッファ生成 + データの代入
//=============================================================================
HRESULT Octa::CreateInst(LPDIRECT3DDEVICE9 pDevice)
{
	// インスタンシングバッファ生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * OCTA_MAX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
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
HRESULT Octa::SetInst(void)
{
	INSTANCE *pInst;

	// 座標データの範囲をロックし、頂点バッファへのポインタを取得
	m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

	// 座標の設定
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		pInst->vPos = m_tData[i].vPos;
		pInst->fSize = m_tData[i].fSize;
		pInst->fUse = m_tData[i].fUse;
		pInst->fRot = m_tData[i].fRot;
		pInst->vColor = m_tData[i].vColor;
		pInst++;
		if (m_tData[i].bUse) m_nCount = i + 1;
	}

	// 座標データをアンロックする
	m_pInstBuff->Unlock();
	return S_OK;
}

//=============================================================================
// 頂点シェーダ宣言の作成
//=============================================================================
HRESULT Octa::CreateDecl(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh)
{
	// メッシュの入力頂点データの定義を取得
	D3DVERTEXELEMENT9 declMesh[8];
	pMesh->GetDeclaration(declMesh);

	// インスタンシングデータと合わせて再定義
	D3DVERTEXELEMENT9 declElems[] = {
		declMesh[0],		// POSITION
		declMesh[1],		// NORMAL
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// ワールド位置
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// サイズ
		{ 1, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// 使用フラグ
		{ 1, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// 頂点カラー
		{ 1, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },		// 頂点カラー
		D3DDECL_END()
	};

	// 頂点要素から頂点シェーダ宣言を作成
	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);
	return S_OK;
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Octa::~Octa(void)
{
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pInstBuff);
	SAFE_RELEASE(m_pIdxBuff);
	SAFE_RELEASE(m_pDecl);

	//SAFE_DELETE_ARRAY(m_pAttrTable);
}
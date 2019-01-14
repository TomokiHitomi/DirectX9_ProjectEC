//=============================================================================
//
// クラウド処理 [cloud.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "cloud.h"
#include "input.h"
#include "calculate.h"
#include "camera.h"
#include "shader.h"
#include "player.h"

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
LPCTSTR g_lpCloudEH[] = {
	"tex",
	"proj",
	"view",
	"world",
	"g_vMove",
	"g_fMorph",
	"g_vPos"
};

//=============================================================================
// コンストラクタ
//=============================================================================
Cloud::Cloud()
{
	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::CLOUD, Priority::Middle, Priority::Low);

	// 各ポインタの初期化
	m_pTexture = NULL;	// テクスチャ
	m_pVtxBuff = NULL;	// 頂点バッファ
	m_pInstBuff = NULL;	// インスタンシングバッファ
	m_pIdxBuff = NULL;	// インデックスバッファ
	m_pDecl = NULL;		// 頂点宣言

	// シェーダ関連の初期化
	pEffect = NULL;		// シェーダ

	// カウンタの初期化
	m_nCount = 0;

	// モーフを初期化
	m_fMorph = 1.0f;

	m_fMove = 0.0f;

	m_vPos = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	m_vMove = D3DXVECTOR4(0.0f, 0.0f, 0.0f, CLOUD_MAX_X * CLOUD_POS_MARGIN);

	// 初期化処理
	Init();
}

//=============================================================================
// デストラクタ
//=============================================================================
Cloud::~Cloud()
{
	// 解放処理
	Release();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Cloud::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// シェーダのアドレスを取得
	pEffect = ShaderManager::GetEffect(ShaderManager::CLOUD);

	for (UINT i = 0; i < EH_MAX; i++)
	{
		// シェーダのハンドルを初期化
		hEffectHandle[i] = NULL;
		// シェーダのハンドルを取得
		hEffectHandle[i] = pEffect->GetParameterByName(NULL, g_lpCloudEH[i]);
		// シェーダのハンドル取得のエラーチェック
		if (hEffectHandle[i] == NULL) MessageBox(NULL, "シェーダハンドルの取得失敗", g_lpCloudEH[i], MB_OK);
	}

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		CLOUD_TEXTURE,
		&m_pTexture)))
	{
		// エラー
		MessageBox(NULL, "テクスチャの読み込みに失敗しました", CLOUD_TEXTURE, MB_OK);
		return S_FALSE;
	}

	// 頂点情報の作成（ローカル座標の設定）
	MakeVertex(pDevice);
	return S_OK;
}

//=============================================================================
// 解放処理
//=============================================================================
void Cloud::Release(void)
{
	// 解放処理
	SAFE_RELEASE(m_pTexture);		// テクスチャ
	SAFE_RELEASE(m_pVtxBuff);		// 頂点バッファ
	SAFE_RELEASE(m_pInstBuff);		// インスタンシングバッファ
	SAFE_RELEASE(m_pIdxBuff);		// インデックスバッファ
	SAFE_RELEASE(m_pDecl);			// 頂点宣言
}

//=============================================================================
// 更新処理
//=============================================================================
void Cloud::Update(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//m_fMorph += CLOUD_MORPH_SPEED;
	//m_fMove += 0.2f;

	D3DXVECTOR4 vTemp = m_vPos;
	m_vPos = D3DXVECTOR4(PlayerManager::GetPos(PlayerManager::PLAYER_1P),0.0f);

	m_vMove.x += vTemp.x - m_vPos.x;
	if (m_vMove.x < 0.0f) m_vMove.x += m_vMove.w;
	m_vMove.z += vTemp.z - m_vPos.z;
	if (m_vMove.z < 0.0f) m_vMove.z += m_vMove.w;

	m_vPosSet = m_vPos;
	m_vPosSet.x -= m_vMove.w / 2;
	m_vPosSet.z -= m_vMove.w / 2;

#ifdef _DEBUG
	PrintDebugProc("【 CLOUD 】\n");
	PrintDebugProc("Pos [%f,%f,%f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
	PrintDebugProc("Move[%f,%f,%f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void Cloud::Draw(void)
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
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | CLOUD_MAX);
	pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	// 頂点とインデックスを設定
	pDevice->SetVertexDeclaration(m_pDecl);
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX));		// 頂点バッファ
	pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// インスタンスバッファ
	pDevice->SetIndices(m_pIdxBuff);										// インデックスバッファ

	// 使用するテクニックを定義
	if (FAILED(pEffect->SetTechnique("Tec01")))
	{
		// エラー
		MessageBox(NULL, "テクニックの定義に失敗しました", "Tec01", MB_OK);
		//return S_FALSE;
	}

	// テクスチャをセット
	SHR(pEffect->SetTexture(hEffectHandle[EH_TEX], m_pTexture), g_lpCloudEH[EH_TEX]);

	// テクスチャの分割数をセット
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_X], (FLOAT)CLOUD_TEXTURE_DIVIDE_X);
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_Y], (FLOAT)CLOUD_TEXTURE_DIVIDE_Y);

	// 必要な行列情報をセット
	SHR(pEffect->SetMatrix(hEffectHandle[EH_PROJ], &mtxProjection), g_lpCloudEH[EH_PROJ]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_VIEW], &mtxView), g_lpCloudEH[EH_VIEW]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_WORLD], &mtxWorld), g_lpCloudEH[EH_WORLD]);

	// モーフ値を設定
	SHR(pEffect->SetFloat(hEffectHandle[EH_MORPH], m_fMorph), g_lpCloudEH[EH_MORPH]);

	// 移動量をセット
	SHR(pEffect->SetVector(hEffectHandle[EH_DATA], &m_vMove), g_lpCloudEH[EH_DATA]);

	// 設置座標をセット
	SHR(pEffect->SetVector(hEffectHandle[EH_POS], &m_vPosSet), g_lpCloudEH[EH_POS]);

	// 結果を確定させる
	pEffect->CommitChanges();

	// シェーダーの開始、numPassには指定してあるテクニックに定義してあるpassの数が変える
	UINT numPass = 0;
	pEffect->Begin(&numPass, D3DXFX_DONOTSAVESTATE);

	for (UINT iPass = 0; iPass < numPass; iPass++)
	{
		// パスを指定して開始
		pEffect->BeginPass(iPass);

		// ポリゴンの描画
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


//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT Cloud::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
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
		pVtx[0].vtx = D3DXVECTOR4(-CLOUD_SIZE_X, CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[1].vtx = D3DXVECTOR4(CLOUD_SIZE_X, CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[2].vtx = D3DXVECTOR4(-CLOUD_SIZE_X, -CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[3].vtx = D3DXVECTOR4(CLOUD_SIZE_X, -CLOUD_SIZE_Y, 0.0f, 1.0f);

		// テクスチャ座標の設定
		int x = 0 % CLOUD_TEXTURE_DIVIDE_X;
		int y = 0 / CLOUD_TEXTURE_DIVIDE_X;
		float sizeX = 1.0f / CLOUD_TEXTURE_DIVIDE_X;
		float sizeY = 1.0f / CLOUD_TEXTURE_DIVIDE_Y;

		pVtx[0].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
		pVtx[1].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
		pVtx[2].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
		pVtx[3].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

		// 頂点カラーの設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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


	/***** インスタンシング用の座標バッファ設定 *****/

	// 座標バッファ生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * CLOUD_MAX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		0,									// 頂点バッファの使用法　
		0,									// 使用する頂点フォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&m_pInstBuff,				// 頂点バッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	{// インスタンシングバッファの中身を埋める
		INSTANCE *pInst;


		// 座標データの範囲をロックし、頂点バッファへのポインタを取得
		m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

		// 座標の設定
		for (unsigned int i = 0; i < CLOUD_MAX_X; i++)
		{
			for (unsigned int j = 0; j < CLOUD_MAX_Y; j++)
			{
				for (unsigned int k = 0; k < CLOUD_MAX_Z; k++)
				{
					pInst->pos = D3DXVECTOR3(
						i * CLOUD_POS_MARGIN,
						j * CLOUD_POS_MARGIN_Y,
						k * CLOUD_POS_MARGIN);
					pInst->pos += RandVector() * CLOUD_POS_RANDOM_VECTOR;
					pInst->vec = RandVector();
					pInst->rot = (float)(rand() % 618) * 0.01f;
					pInst->shiftuv.x = 1.0f / CLOUD_TEXTURE_DIVIDE_X * (int)(rand() % CLOUD_TEXTURE_DIVIDE_X);
					pInst->shiftuv.y = 1.0f / CLOUD_TEXTURE_DIVIDE_Y * (int)(rand() % CLOUD_TEXTURE_DIVIDE_Y);
					pInst++;
				}
			}
		}

		// 座標データをアンロックする
		m_pInstBuff->Unlock();
	}

	D3DVERTEXELEMENT9 declElems[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },		// VTX
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// UV
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },		// カラー

		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// ワールド位置
		{ 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// 移動ベクトル
		{ 1, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// モーフ
		{ 1, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },		// 追加UV
		D3DDECL_END()
	};

	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);

	return S_OK;
}

////=============================================================================
//// 設置処理
////=============================================================================
//void Cloud::Set(D3DXVECTOR3 pos)
//{
//	{// 座標バッファの中身を埋める
//		INSTANCE *pInst;
//
//		// 座標データの範囲をロックし、頂点バッファへのポインタを取得
//		m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);
//
//		for (unsigned int i = 0; i < CLOUD_MAX; i++, pInst++)
//		{
//			if (m_fMorph - pInst->rot > 1.0f)
//			{
//				// 座標の設定
//				pInst->pos = pos + RandVector() * (rand() % CLOUD_SET_LENGTH);
//				pInst->rot = m_fMorph;
//				if (i >= m_nCount)
//				{
//					m_nCount = i + 1;
//				}
//				break;
//			}
//		}
//
//		// 座標データをアンロックする
//		m_pInstBuff->Unlock();
//	}
//}
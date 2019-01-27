//=============================================================================
//
// 2Dテクスチャ処理 [XTexture.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
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
	// ポインタを NULL で初期化
	m_pTexture = NULL;	// テクスチャバッファ
	m_pVertex = NULL;	// 頂点バッファ
	m_pData = NULL;		// データ

	// 使用フラグを false で初期化
	m_bUse = false;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CXTexture::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPass, int nNum)
{
	// 使用数が0以下の場合は初期化失敗
	if (nNum <= 0) return E_FAIL;

	// 使用数を保管
	m_nNum = nNum;

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,					// デバイス
		pTexPass,					// ファイル名
		&m_pTexture)))				// 読み込むメモリ（複数なら配列に）
	{
		return E_FAIL;
	}

	// 各データを実体化＋初期化
	m_pData = new XTextureData[m_nNum];

	// 頂点の作成
	if (FAILED(MakeVertex(pDevice)))
	{
		return E_FAIL;
	}

	// 使用フラグを true にする
	m_bUse = true;
}

//=============================================================================
// 解放処理
//=============================================================================
void CXTexture::Release(void)
{
	SAFE_RELEASE(m_pTexture);	// テクスチャの開放
	SAFE_RELEASE(m_pVertex);	// 頂点バッファの開放
}

//=============================================================================
// 更新処理
//=============================================================================
void CXTexture::Update(void)
{
	if (m_bUse)
	{
		UpdateVertex();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CXTexture::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// 頂点バッファをデバイスのデータストリームにバインド
		pDevice->SetStreamSource(0, m_pVertex, 0, sizeof(VERTEX_2D));

		// テクスチャの設定
		pDevice->SetTexture(0, m_pTexture);

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		for (UINT i = 0; i < m_nNum; i++)
		{
			if (m_pData[i].bUse)
			{
				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i * NUM_VERTEX), NUM_POLYGON);
			}
		}
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT CXTexture::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * (NUM_VERTEX * m_nNum),		// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,								// 頂点バッファの使用法　
		FVF_VERTEX_2D,									// 使用する頂点フォーマット
		D3DPOOL_MANAGED,								// リソースのバッファを保持するメモリクラスを指定
		&m_pVertex,										// 頂点バッファインターフェースへのポインタ
		NULL)))											// NULLに設定
	{
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
// 頂点の更新
//=============================================================================
HRESULT CXTexture::UpdateVertex(void)
{
	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		m_pVertex->Lock(0, 0, (void**)&pVtx, 0);

		for (UINT i = 0; i < m_nNum; i++, pVtx += 4)
		{
			if (m_pData[i].bUse)
			{
				// 角度を計算
				m_pData[i].fBaseAngle = atan2f(m_pData[i].vSize.y, m_pData[i].vSize.x);
				// 半径を計算
				D3DXVECTOR2 temp = D3DXVECTOR2(m_pData[i].vSize.x, m_pData[i].vSize.y);
				m_pData[i].fRadius = D3DXVec2Length(&temp);
				// 半径にスケール値を乗算
				m_pData[i].fRadius *= m_pData[i].fScl;

				// 頂点座標の設定（回転対応）
				pVtx[0].vtx.x = m_pData[i].vPos.x - cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.y = m_pData[i].vPos.y - sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.z = m_pData[i].vPos.z;
				pVtx[1].vtx.x = m_pData[i].vPos.x + cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.y = m_pData[i].vPos.y - sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.z = m_pData[i].vPos.z;
				pVtx[2].vtx.x = m_pData[i].vPos.x - cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.y = m_pData[i].vPos.y + sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.z = m_pData[i].vPos.z;
				pVtx[3].vtx.x = m_pData[i].vPos.x + cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.y = m_pData[i].vPos.y + sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.z = m_pData[i].vPos.z;

				//pVtx[0].vtx = D3DXVECTOR3(m_pData[i].vPos.x - m_pData[i].vSize.x / 2, m_pData[i].vPos.y - m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[1].vtx = D3DXVECTOR3(m_pData[i].vPos.x + m_pData[i].vSize.x / 2, m_pData[i].vPos.y - m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[2].vtx = D3DXVECTOR3(m_pData[i].vPos.x - m_pData[i].vSize.x / 2, m_pData[i].vPos.y + m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[3].vtx = D3DXVECTOR3(m_pData[i].vPos.x + m_pData[i].vSize.x / 2, m_pData[i].vPos.y + m_pData[i].vSize.y / 2, 0.0f);

				// rhwの設定
				pVtx[0].rhw =
				pVtx[1].rhw =
				pVtx[2].rhw =
				pVtx[3].rhw = 1.0f;

				// 反射光の設定
				pVtx[0].diffuse =
				pVtx[1].diffuse =
				pVtx[2].diffuse =
				pVtx[3].diffuse = m_pData[i].xColor;

				// テクスチャ座標の設定
				int x = m_pData[i].nTexNum % m_pData[i].tDivide.x;
				int y = m_pData[i].nTexNum / m_pData[i].tDivide.x;
				float sizeX = 1.0f / m_pData[i].tDivide.x;
				float sizeY = 1.0f / m_pData[i].tDivide.y;
				pVtx[0].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
				pVtx[1].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
				pVtx[2].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
				pVtx[3].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

				//pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
				//pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
				//pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
				//pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);
			}
		}

		// 頂点データをアンロックする
		m_pVertex->Unlock();
	}
	return S_OK;
}
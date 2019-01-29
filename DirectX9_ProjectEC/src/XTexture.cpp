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
#include "shader.h"
#include "file.h"

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

	// シェーダのアドレスを取得
	m_pEffect = ShaderManager::GetEffect(ShaderManager::XTEXTURE);


	// 使用フラグを false で初期化
	m_bUse = false;

#ifdef _DEBUG
	m_bEdit = false;
#endif
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
CXTexture::~CXTexture(void)
{
	Release();
}

//=============================================================================
// 解放処理
//=============================================================================
void CXTexture::Release(void)
{
	SAFE_RELEASE(m_pTexture);	// テクスチャの開放
	SAFE_RELEASE(m_pVertex);	// 頂点バッファの開放

	// 各種データを解放
	SAFE_DELETE_ARRAY(m_pData);
#ifdef _DEBUG
	SAFE_DELETE_ARRAY(m_pDataInit);
#endif
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CXTexture::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPath, LPSTR pDataPath, int nNum, int nDivideX, int nDivideY)
{
	// 使用数が0以下の場合は初期化失敗
	if (nNum <= 0) return E_FAIL;

	// 使用数を保管
	m_nNum = nNum;

	// テクスチャの分割数を保管
	m_cDivide.x = nDivideX;
	m_cDivide.y = nDivideY;

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,					// デバイス
		pTexPath,					// ファイル名
		&m_pTexture)))				// 読み込むメモリ（複数なら配列に）
	{
		return E_FAIL;
	}

	// 各データを実体化＋初期化
	m_pData = new XTextureData[m_nNum];

#ifdef _DEBUG
	m_pDataPath = pDataPath;
	m_pDataInit = new XTextureData[m_nNum];
#endif

	bool bResult = File::Load(m_pData, pDataPath, m_nNum);

	if (!bResult)
	{
#ifdef _RELEASE
		// エラー
		MessageBox(NULL, "初期化データがありません。", pDataPath, MB_OK);
		return E_FAIL;
#endif

#ifdef _DEBUG
		for (UINT i = 0; i < m_nNum; i++)
		{
			// 各データの初期化
			m_pData[i].vPos = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
			m_pData[i].vSize = D3DXVECTOR2(XTEXTURE_INIT_SIZE, XTEXTURE_INIT_SIZE);
			m_pData[i].xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_pData[i].nTexNum = 0;
			m_pData[i].fBaseAngle = 0.0f;
			m_pData[i].fRadius = 0.0f;
			m_pData[i].fScl = 1.0f;
			m_pData[i].fRot = 0.0f;
			m_pData[i].bUse = true; 
		}

		File::Save(m_pData, pDataPath, m_nNum);
#endif
	}

#ifdef _DEBUG
	for (UINT i = 0; i < m_nNum; i++)
	{
		m_pDataInit[i] = m_pData[i];
	}
#endif

	// 頂点の作成
	if (FAILED(MakeVertex(pDevice)))
	{
		return E_FAIL;
	}

	// 使用フラグを true にする
	m_bUse = true;
}

//=============================================================================
// 更新処理
//=============================================================================
void CXTexture::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode(m_pDataPath);
	if (bGui)
	{
		if (ImGui::Button("Save"))
		{
			File::Save(m_pDataInit, m_pDataPath, m_nNum);
		}
		if (ImGui::Button("Load"))
		{
			File::Load(m_pDataInit, m_pDataPath, m_nNum);
		}

		ImGui::Checkbox("Edit", &m_bEdit);

		if (m_bEdit)
		{
			for (UINT i = 0; i < m_nNum; i++)
			{
				m_pData[i] = m_pDataInit[i];
			}
		}

		for (UINT i = 0; i < m_nNum; i++)
		{
			CHAR cNum[256];
			cNum[0] = 'N';
			cNum[1] = 'o';
			cNum[2] = '.';
			cNum[3] = (int)'0' + i;
			cNum[4] = ' ';
			cNum[5] = NULL;

			strcat_s(cNum, sizeof(cNum) - strlen(cNum) - strlen(m_pDataPath) - 1, m_pDataPath);

			ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
			bool bGui = ImGui::TreeNode(cNum);
			if (bGui)
			{
				ImGui::Text("Slider");
				ImGui::SliderFloat2("PosS", &m_pDataInit[i].vPos.x, 0.0f, 1.0f);
				ImGui::SliderFloat4("ColorS", &m_pDataInit[i].xColor.r, 0.0f, 1.0f);
				ImGui::SliderFloat("SclS", &m_pDataInit[i].fScl, 0.0f, 2.0f);
				ImGui::SliderFloat("RotS", &m_pDataInit[i].fRot, -D3DX_PI, D3DX_PI);

				ImGui::Text("Input");
				ImGui::InputFloat2("Pos", &m_pDataInit[i].vPos.x, 3);
				ImGui::InputFloat2("Size", &m_pDataInit[i].vSize.x, 3);
				ImGui::InputFloat4("Color", &m_pDataInit[i].xColor.r, 3);
				ImGui::InputInt("TexNum", &m_pDataInit[i].nTexNum);
				ImGui::InputFloat("Scl", &m_pDataInit[i].fScl, 3);
				ImGui::InputFloat("Rot", &m_pDataInit[i].fRot, 3);

				ImGui::Text("Flag");
				ImGui::Checkbox("Use", &m_pDataInit[i].bUse);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
#endif

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

		// ビュー・プロジェクション行列を取得
		D3DXMATRIX mtxProjection;
		pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

		// 頂点バッファをデバイスのデータストリームにバインド
		pDevice->SetStreamSource(0, m_pVertex, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// 使用するテクニックを定義
		if (FAILED(m_pEffect->SetTechnique("Tec01")))
		{
			// エラー
			MessageBox(NULL, "テクニックの定義に失敗しました", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// 必要な行列情報をセット
		m_pEffect->SetMatrix("proj", &mtxProjection);

		// テクスチャの設定
		//pDevice->SetTexture(0, m_pTexture);
		m_pEffect->SetTexture("tex", m_pTexture);

		// 結果を確定させる
		m_pEffect->CommitChanges();

		// シェーダーの開始、numPassには指定してあるテクニックに定義してあるpassの数が変える
		UINT numPass = 0;
		m_pEffect->Begin(&numPass, 0);
		for (UINT j = 0; j < numPass; j++)
		{
			// パスを指定して開始
			m_pEffect->BeginPass(0);

			for (UINT i = 0; i < m_nNum; i++)
			{
				if (m_pData[i].bUse)
				{
					// ポリゴンの描画
					pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i * NUM_VERTEX), NUM_POLYGON);
				}
			}

			// シェーダーパスを終了
			m_pEffect->EndPass();
		}
		// シェーダーを終了
		m_pEffect->End();

		// 固定機能に戻す
		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);
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
				m_pData[i].fBaseAngle = atan2f((m_pData[i].vSize.y * SCREEN_SCALE) / 2, (m_pData[i].vSize.x * SCREEN_SCALE) / 2);
				// 半径を計算
				D3DXVECTOR2 temp = D3DXVECTOR2((m_pData[i].vSize.x * SCREEN_SCALE) / 2, (m_pData[i].vSize.y * SCREEN_SCALE) / 2);
				m_pData[i].fRadius = D3DXVec2Length(&temp);
				// 半径にスケール値を乗算
				m_pData[i].fRadius *= m_pData[i].fScl;

				// 頂点座標の設定（回転対応）
				pVtx[0].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) - cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) - sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.z = m_pData[i].vPos.z;
				pVtx[1].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) + cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) - sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.z = m_pData[i].vPos.z;
				pVtx[2].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) - cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) + sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.z = m_pData[i].vPos.z;
				pVtx[3].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) + cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) + sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
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
				int x = m_pData[i].nTexNum % m_cDivide.x;
				int y = m_pData[i].nTexNum / m_cDivide.x;
				float sizeX = 1.0f / m_cDivide.x;
				float sizeY = 1.0f / m_cDivide.y;
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
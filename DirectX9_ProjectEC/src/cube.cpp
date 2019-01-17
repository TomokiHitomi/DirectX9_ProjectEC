//=============================================================================
//
// スカイドーム処理 [cube.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "cube.h"
#include "calculate.h"
#include "joycon.h"

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
Cube::Prop		Cube::m_tagProp;


//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Cube::Cube(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::CUBE, Priority::Low, Priority::Middle);

	// 各プロパティの初期化
	m_tagProp.vPos = D3DXVECTOR3(0.0f, 350.0f, 0.0f);
	m_tagProp.vRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_tagProp.vScl = D3DXVECTOR3(MODEL_CUBE_SCL, MODEL_CUBE_SCL, MODEL_CUBE_SCL);
	m_tagProp.bUse = true;

	// モデルの初期化
	//m_CSkinMesh->Init(pDevice, "data/model/Yuko.x");
	m_tagProp.CXModel = new CXModel;
	m_tagProp.CXModel->Init(pDevice, (LPSTR)CUBE_MODEL, NULL);

}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Cube::~Cube(void)
{
	if (m_tagProp.CXModel != NULL)
	{
		m_tagProp.CXModel->Release();
		delete m_tagProp.CXModel;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Cube::Update(void)
{
	if (m_tagProp.bUse)
	{
		// アニメーション更新処理
		m_tagProp.CXModel->Update();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Cube::Draw(void)
{
	if (m_tagProp.bUse)
	{
		//// joyconテスト
		//D3DXVECTOR3 vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		//D3DXVECTOR3 vecTemp = GetJoyconAccel(0);
		//D3DXVec3Normalize(&vecTemp, &vecTemp);

		////WorldConvertAxis(&m_tagProp.mtxWorld, m_tagProp.vPos, vecTemp,
		////	vecUp, m_tagProp.vScl);

		//PrintDebugProc("Cube [X:%f Y:%f Z:%f]\n",
		//	vecTemp.x, vecTemp.y, vecTemp.z);

		//m_tagProp.vPos += vecTemp;
		// ワールド変換
		WorldConvert(&m_tagProp.mtxWorld,
			m_tagProp.vPos, m_tagProp.vRot, m_tagProp.vScl);

		m_tagProp.CXModel->Draw(m_tagProp.mtxWorld);
	}
}
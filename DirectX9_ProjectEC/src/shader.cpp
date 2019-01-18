//=============================================================================
//
// シェーダ処理 [shader.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "shader.h"
#include <tchar.h>

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
D3DXVECTOR4 ShaderColorToVec(D3DCOLORVALUE color);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Shader* ShaderManager::pShader[FILE_MAX];
LPD3DXBUFFER ShaderManager::pErrorBuff;

// シェーダファイルのパス（FILEの通しナンバーと順番を合わせること）
//const TCHAR* cShaderFilename[] = {
//	// BGM
//	_T("data/SHADER/billboard.fx"),
//	_T("data/SHADER/xmodel.fx"),
//	_T("data/SHADER/skinassimp.fx"),
//	_T("data/SHADER/skinmesh.fx"),
//	_T("data/SHADER/plane.fx"),
//	_T("data/SHADER/box.fx")
//};

//=============================================================================
// コンストラクタ
//=============================================================================
ShaderManager::ShaderManager()
{
	for (unsigned int i = 0; i < FILE_MAX; i++)
	{
		pShader[i] = NULL;
	}

	// 初期化処理
	Load();
}

//=============================================================================
// デストラクタ
//=============================================================================
ShaderManager::~ShaderManager()
{
	// 削除処理
	Delete();
}

//=============================================================================
// 読み込み処理
//=============================================================================
HRESULT ShaderManager::Load(void)
{
	pShader[XMODEL] = new Shader(SHADER_FILE_XMODEL);
	pShader[SKINMESH] = new Shader(SHADER_FILE_SKINMESH);
	pShader[WING_FEATHER] = new Shader(SHADER_FILE_WING_FEATHER);
	pShader[CLOUD] = new Shader(SHADER_FILE_CLOUD);
	pShader[OCTA] = new Shader(SHADER_FILE_OCTA);
	pShader[DEBUGSPHERE] = new Shader(SHADER_FILE_DEBUGSPHERE);
	return S_OK;
}

//=============================================================================
// 削除処理
//=============================================================================
void ShaderManager::Delete(void)
{
	for (unsigned int i = 0; i < FILE_MAX; i++)
	{
		SAFE_DELETE(pShader[i]);
	}
}

//=============================================================================
// 作成処理
//=============================================================================
HRESULT ShaderManager::CreateEffect(LPD3DXEFFECT* pEffect, FILE eFile)
{
	//// デバイスを取得
	//LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//// ファイル( const char* path )からシェーダーファイルを読み込み読み込み
	//if (FAILED(D3DXCreateEffectFromFile(
	//	pDevice, cShaderFilename[eFile], 0, 0, 0, 0, &*pEffect, &pErrorBuff)))
	//{
	//	// エラー
	//	MessageBox(NULL, (LPCTSTR)pErrorBuff->GetBufferPointer(), cShaderFilename[eFile], MB_OK);
	//	pEffect = NULL;
	//	return S_FALSE;
	//}
	return S_OK;
}


////=============================================================================
//// 更新処理
////=============================================================================
//void ShaderManager::Update(void)
//{
//	//SAFE_UPDATE();
//}
//
////=============================================================================
//// 描画処理
////=============================================================================
//void ShaderManager::Draw(void)
//{
//	//SAFE_DRAW();
//}



//=============================================================================
// コンストラクタ
//=============================================================================
Shader::Shader(const char* path)
{
	// シェーダ関連の初期化
	pErrorBuff = NULL;	// シェーダ用コンパイルエラー
	pEffect = NULL;		// シェーダ

	// 初期化処理
	Init(path);
}

//=============================================================================
// デストラクタ
//=============================================================================
Shader::~Shader()
{
	// 解放処理
	Release();
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Shader::Init(const char* path)
{
	// デバイスを取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ファイル( const char* path )からシェーダーファイルを読み込み読み込み
	if (FAILED(D3DXCreateEffectFromFile(
		pDevice, path, 0, 0, 0, 0, &pEffect, &pErrorBuff)))
	{
		// エラー
		MessageBox(NULL, (LPCTSTR)pErrorBuff->GetBufferPointer(), path, MB_OK);
		return S_FALSE;
	}
	return S_OK;
}

//=============================================================================
// 解放処理
//=============================================================================
void Shader::Release(void)
{
	// 解放処理
	SAFE_RELEASE(pErrorBuff);	// シェーダ用コンパイルエラー
	SAFE_RELEASE(pEffect);		// シェーダ
}

////=============================================================================
//// 更新処理
////=============================================================================
//void Shader::Update(void)
//{
//
//}
//
////=============================================================================
//// 描画処理
////=============================================================================
//void Shader::Draw(void)
//{
//
//}

//=============================================================================
// 処理
//=============================================================================
D3DXVECTOR4 ShaderColorToVec(D3DCOLORVALUE color)
{
	D3DXVECTOR4 temp;
	temp.x = color.r;
	temp.y = color.g;
	temp.z = color.b;
	temp.w = color.a;
	return temp;
}

//=============================================================================
// 処理
//=============================================================================
void SetShaderMat(LPD3DXEFFECT pEffect, D3DMATERIAL9 pMat)
{
	if (FAILED(pEffect->SetValue("mat", &pMat, sizeof(D3DMATERIAL9))))
	{
		MessageBox(NULL, "てすと", "mat", MB_OK);
		//return E_FAIL;
	}

	D3DXVECTOR4  temp;

	// ディフューズ光用のマテリアルをセット
	temp = ShaderColorToVec(pMat.Diffuse);
	pEffect->SetVector("dif_mat", &temp);

	// 環境光用のマテリアルをセット
	temp = ShaderColorToVec(pMat.Ambient);
	pEffect->SetVector("amb_mat", &temp);

	// エミッシブ光用のマテリアルをセット
	temp = ShaderColorToVec(pMat.Emissive);
	pEffect->SetVector("emi_mat", &temp);

	// スペキュラー光用のマテリアルをセット
	temp = ShaderColorToVec(pMat.Specular);
	pEffect->SetVector("spc_mat", &temp);

	// パワー値をセット
	pEffect->SetFloat("pwr_mat", pMat.Power);
}

//=============================================================================
// 処理
//=============================================================================
void SetShaderLight(LPD3DXEFFECT pEffect, D3DLIGHT9 pLight)
{
	D3DXVECTOR4  temp;
	// 環境光用のマテリアルをセット
	temp = ShaderColorToVec(pLight.Ambient);
	pEffect->SetVector("amb_lt", &temp);

	// ディフューズ光用のマテリアルをセット
	temp = ShaderColorToVec(pLight.Diffuse);
	pEffect->SetVector("dif_lt", &temp);

	// スペキュラー光用のマテリアルをセット
	temp = ShaderColorToVec(pLight.Specular);
	pEffect->SetVector("spc_lt", &temp);

	// ライトのベクトルをセット
	temp = D3DXVECTOR4(pLight.Direction, 0.0f);
	pEffect->SetVector("dir_lt", &temp);
}
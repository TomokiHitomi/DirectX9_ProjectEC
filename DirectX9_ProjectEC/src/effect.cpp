//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "effect.h"

/* 全体で必要なインクルード */
#include "input.h"
#include "camera.h"
#include "calculate.h"

/* Debug */
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
::Effekseer::Matrix44 ConvertMtx44(D3DXMATRIX mtx);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
const WCHAR* EffectManager::c_filename[] = {
	// エフェクトファイル
	L"Sword_Sacredlight.efk"
	//L"TrinityField.efk",
	//L"high_sword",
	//L"test2.efk",
	//L"test3.efk",
	//L"test4.efk"
};

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
EffectManager::EffectManager(void)
{

	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::EFFECT, Priority::Low, Priority::Low);

	//// オブジェクトタイプの設定
	//SetObjectType(ObjectManager::ObjectType::NORMAL);

	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	m_manager = NULL;
	m_renderer = NULL;
	m_handle = -1;
	m_position;
	m_rotation;

	for (unsigned int i = 0; i < EFFECT_MAX; i++)
	{
		m_effect[i] = NULL;
	}

	//// XAudio2の初期化を行う
	//m_sound = NULL;
	//XAudio2Create(&m_xa2);

	//m_xa2->CreateMasteringVoice(&m_xa2_master);


	// 描画用インスタンスの生成
	m_renderer = ::EffekseerRendererDX9::Renderer::Create(pDevice, 2048);

	// エフェクト管理用インスタンスの生成
	m_manager = ::Effekseer::Manager::Create(2048);

	// 描画用インスタンスから描画機能を設定
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());

	// 左手座標系に設定
	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);


	//// 音再生用インスタンスの生成
	//m_sound = ::EffekseerSound::Sound::Create(m_xa2, 16, 16);

	//// 音再生用インスタンスから再生機能を指定
	//m_manager->SetSoundPlayer(m_sound->CreateSoundPlayer());

	//// 音再生用インスタンスからサウンドデータの読込機能を設定
	//// 独自拡張可能、現在はファイルから読み込んでいる。
	//m_manager->SetSoundLoader(m_sound->CreateSoundLoader());


	// 視点位置を確定	
	m_position = ::Effekseer::Vector3D(0.0f, 5.0f, 0.0f);
	m_rotation = ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f);


	// エフェクトの読込
	for (unsigned int i = 0; i < EFFECT_MAX; i++)
	{
		m_effect[i] = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)c_filename[i]);
	}
	//m_effect[EFFECT1] = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)L"TrinityField.efk");
	//m_effect[EFFECT2] = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)L"test2.efk");

	//g_effect = Effekseer::Effect::Create(g_manager, (const EFK_CHAR*)L"test.efk");

	// エフェクトの再生
	//m_handle = m_manager->Play(m_effect[EFFECT1], 0, 0, 0);
	//m_manager->Play(m_effect[EFFECT2], 0, 5.0f, 0);

	m_manager->SetLocation(m_handle, m_position);
	m_manager->SetScale(m_handle, 5.0f, 5.0f, 5.0f);

}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
EffectManager::~EffectManager(void)
{
	// 全エフェクトの停止
	m_manager->StopAllEffects();

	for (unsigned int i = 0; i < EFFECT_MAX; i++)
	{
		// エフェクトの破棄
		ES_SAFE_RELEASE(m_effect[i]);
	}

	// 先にエフェクト管理用インスタンスを破棄
	m_manager->Destroy();

	//// 次に音再生用インスタンスを破棄
	//g_sound->Destroy();

	// 次に描画用インスタンスを破棄
	m_renderer->Destroy();

	//// XAudio2の解放
	//if (g_xa2_master != NULL)
	//{
	//	g_xa2_master->DestroyVoice();
	//	g_xa2_master = NULL;
	//}
	//ES_SAFE_RELEASE(g_xa2);
}

//=============================================================================
// 更新処理
//=============================================================================
void EffectManager::Update(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// エフェクトの移動処理を行う
	//g_manager->AddLocation(g_handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));
	if (GetKeyboardTrigger(DIK_B))
	{
		m_handle = m_manager->Play(m_effect[EFFECT1], 0, 5.0f, 0);
		m_manager->SetScale(m_handle, 5.0f, 5.0f, 5.0f);
	}
	if (GetKeyboardTrigger(DIK_M))
	{
		m_manager->StopEffect(m_handle);
	}

	// エフェクトの更新処理を行う
	m_manager->Update();
}

//=============================================================================
// 描画処理
//=============================================================================
void EffectManager::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxView, mtxPro;

	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxPro);

	// カメラ行列を設定
	m_renderer->SetCameraMatrix(ConvertMtx44(mtxView));

	// 投影行列を設定
	m_renderer->SetProjectionMatrix(ConvertMtx44(mtxPro));


	// 両面描画する
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	// エフェクトの描画開始処理を行う。
	m_renderer->BeginRendering();

	//g_position = ::Effekseer::Vector3D(100.0f, 20.0f, 0.0f);
	//g_manager->SetLocation(g_handle, g_position);

	// エフェクトの描画を行う。
	m_manager->Draw();

	// エフェクトの描画終了処理を行う。
	m_renderer->EndRendering();

	// 裏面をカリングに戻す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

//=============================================================================
// Effekseer用行列変換処理
//=============================================================================
::Effekseer::Matrix44 ConvertMtx44(D3DXMATRIX mtx)
{
	::Effekseer::Matrix44 mtx44;
	mtx44.Values[0][0] = mtx._11;
	mtx44.Values[0][1] = mtx._12;
	mtx44.Values[0][2] = mtx._13;
	mtx44.Values[0][3] = mtx._14;
	mtx44.Values[1][0] = mtx._21;
	mtx44.Values[1][1] = mtx._22;
	mtx44.Values[1][2] = mtx._23;
	mtx44.Values[1][3] = mtx._24;
	mtx44.Values[2][0] = mtx._31;
	mtx44.Values[2][1] = mtx._32;
	mtx44.Values[2][2] = mtx._33;
	mtx44.Values[2][3] = mtx._34;
	mtx44.Values[3][0] = mtx._41;
	mtx44.Values[3][1] = mtx._42;
	mtx44.Values[3][2] = mtx._43;
	mtx44.Values[3][3] = mtx._44;
	return mtx44;
}
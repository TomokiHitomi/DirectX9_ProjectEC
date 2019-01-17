//=============================================================================
//
// シーン処理 [scene.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "scene.h"

/* Scene */
#include "title.h"
#include "game.h"
#include "result.h"

/* System */
#include "light.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

/* Ui */

/* Object */

/* デバッグ */
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
//始めはシーンを空にしておく
BaseScene *SceneManager::m_pScene = NULL;
ShaderManager		*SceneManager::ShaderMgr = NULL;
CameraManager		*SceneManager::CameraMgr = NULL;
ModelManager		*SceneManager::ModelMgr = NULL;

SceneManager::SCENE	SceneManager::m_eScene;

//=============================================================================
// シーン管理処理
//=============================================================================
void SceneManager::ChangeScene(SCENE scene)
{

	if (m_pScene != NULL)
	{
		delete m_pScene;
	}

	// SEの停止処理
	EndSeS();

	// 選択されたシーンを保管
	m_eScene = scene;

	switch (scene)
	{	//引数のシーン
	case SCENE::TITLE:
		SetSoundBgm(SOUND_BGM_TITLE);
		m_pScene = new TitleScene();	// タイトルシーンを現在のシーンにする
		break;
	case SCENE::GAME:
		SetSoundBgm(SOUND_BGM_GAME);
		m_pScene = new GameScene();		// ゲームシーンを現在のシーンにする
		break;
	case SCENE::RESULT:
		SetSoundBgm(SOUND_BGM_RESULT);
		m_pScene = new ResultScene();	// リザルトシーンを現在のシーンにする
		break;
	default:
		break;
	}
}

//=============================================================================
// 初期化処理
//=============================================================================
void SceneManager::Init(HINSTANCE hInst, HWND hWnd)
{
	// 初期のシーンをTITLEに設定
	m_eScene = GAME;

	LightManager::Init();	// ライト（シェーダ用）
	InitLight();			// ライト
	InitInput(hInst, hWnd);	// 入力
	InitSound(hWnd);		// サウンド
	InitFade();				// フェード

	// カメラマネージャーを実体化
	CameraMgr = new CameraManager;

	// シェーダマネージャを実体化
	ShaderMgr = new ShaderManager;

	// モデルマネージャを実体化
	ModelMgr = new ModelManager;

	ChangeScene(m_eScene);	// 初期シーン設定

#ifdef _DEBUG
	InitDebugProc();		// デバッグ
#endif
}

//=============================================================================
// 終了処理
//=============================================================================
void SceneManager::Uninit(void)
{

	UninitInput();			// 入力
	UninitSound();			// サウンド
	UninitFade();			// フェード

#ifdef _DEBUG
	UninitDebugProc();		// デバッグ
#endif
	SAFE_DELETE(CameraMgr);
	SAFE_DELETE(ShaderMgr);
	SAFE_DELETE(ModelMgr);
}

//=============================================================================
// 更新処理
//=============================================================================
void SceneManager::Update(void)
{
	UpdateInput();			// 入力
	UpdateFade();			// フェード

#ifdef _DEBUG
	DebugScene();			// デバッグ用
#endif

	m_pScene->Update();		// 現在のシーンの更新関数

	CameraManager::Update();	// カメラ

	UpdateSound();			// サウンド
}

//=============================================================================
// 描画処理
//=============================================================================
void SceneManager::Draw(void)
{
	CameraManager::Set(CameraManager::SINGLE);

	m_pScene->Draw();		// 現在のシーンの描画関数

	DrawFade();				// フェード

#ifdef _DEBUG
	DrawDebugProc();
#endif
}

//=============================================================================
// コンストラクタ処理
//=============================================================================
SceneManager::SceneManager(void)
{
	// 各変数を初期化

}

//=============================================================================
// デストラクタ処理
//=============================================================================
SceneManager::~SceneManager(void)
{
	if (m_pScene != NULL)
	{
		delete m_pScene;
	}
}

//=============================================================================
// シーン管理処理
//=============================================================================
void SceneManager::EndSeS(void)
{
	EndSe(SE_FIRE1);
	EndSe(SE_WATER_SUCK);
	EndSe(SE_WATER1);
	EndSe(SE_WATER2);
	EndSe(SE_WIND_SUCK);
	EndSe(SE_WIND1);
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void SceneManager::DebugScene(void)
{
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Scene");
	if (bGui)
	{
		// 現在のシーンを表示
		switch (m_eScene)
		{
		case SCENE::TITLE:
			ImGui::Text("Scene[TITLE]");
			break;
		case SCENE::GAME:
			ImGui::Text("Scene[GAME]");
			break;
		case SCENE::RESULT:
			ImGui::Text("Scene[RESULT]");
			break;
		}
		ImGui::TreePop();
	}

	// デバッグの更新処理
	UpdateDebugProc();

	// シーン情報を表示
	PrintDebugProc("【 SCENE 】\n");

	// 現在のシーンを表示
	switch (m_eScene)
	{
	case SCENE::TITLE:
		PrintDebugProc("Scene[TITLE ]  ");
		break;
	case SCENE::GAME:
		PrintDebugProc("Scene[ GAME ]  ");
		break;
	case SCENE::RESULT:
		PrintDebugProc("Scene[RESULT]  ");
		break;
	}


	//PrintDebugProc("【 Result 】\n");
	//PrintDebugProc("Rate [%f]  ", GetStageRate());
	//switch (GetStageWinPlayer())
	//{
	//case STAGE_WIN_NON:
	//	PrintDebugProc("Win[NON]\n");
	//	break;
	//case STAGE_WIN_1P:
	//	PrintDebugProc("Win[1P]\n");
	//	break;
	//case STAGE_WIN_2P:
	//	PrintDebugProc("Win[2P]\n");
	//	break;
	//}
	//PrintDebugProc("\n");
	if (GetKeyboardTrigger(DIK_F2))
	{
		SetFadeScene(TITLE);
		//ChangeScene(TITLE);
	}
	if (GetKeyboardTrigger(DIK_F3))
	{
		SetFadeScene(GAME);
		//ChangeScene(RESULT);
	}
	if (GetKeyboardTrigger(DIK_F4))
	{
		SetFadeScene(RESULT);
		//ChangeScene(SELECT);
	}
	// SEテスト（sound.hをincludeして関数を呼ぶ）
	//if (GetKeyboardTrigger(DIK_1))
	//{	// 
	//	SetSe(SE_SELECT_ERROR, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_2))
	//{	// 
	//	SetSe(SE_GAME_SELECT, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_3))
	//{	// 
	//	SetSe(SE_GAME_KETTEI, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_4))
	//{	//		
	//	SetSe(SE_RESULT_SELECT, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_5))
	//{	// 
	//	SetSe(SE_RESULT_KETTEI, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_6))
	//{	// 
	//	SetVoice(VOICE_3, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_7))
	//{	// 
	//	SetVoice(VOICE_2, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}
	//if (GetKeyboardTrigger(DIK_8))
	//{	// 
	//	SetVoice(VOICE_START, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
	//}

	PrintDebugProc("\n");
}
#endif
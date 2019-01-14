//=============================================================================
//
// タイトル処理 [title.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "title.h"

/* 全体で必要なインクルード */
#include "input.h"
#include "camera.h"
#include "sound.h"

/* タイトルで必要なインクルード */
#include "object.h"
#include "SkinMeshX.h"
#include "light.h"
#include "player.h"
#include "skydome.h"
#include "cube.h"
#include "stencil.h"
#include "effect.h"
#include "stage.h"
#include "debugobject.h"

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

//*****************************************************************************
// グローバル変数
//*****************************************************************************


//=============================================================================
// 更新処理
//=============================================================================
void TitleScene::Update(void)
{
	ObjectManager::UpdateAll();
#ifdef _DEBUG
	//if (GetKeyboardTrigger(DIK_5))
	//{
	//	new Copyright;
	//}
	//if (GetKeyboardTrigger(DIK_6))
	//{
	//	Object *pTemp = Object::GetObjectPointer(Object::COPYRIGHT);
	//	if (pTemp != NULL)
	//	{
	//		pTemp->Release();
	//	}
	//}
	//if (GetKeyboardTrigger(DIK_7))
	//{
	//	new AirWaterFream;
	//}
	//if (GetKeyboardTrigger(DIK_8))
	//{
	//	Object *pTemp = Object::GetObjectPointer(Object::FRAME);
	//	if (pTemp != NULL)
	//	{
	//		pTemp->Release();
	//	}
	//}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void TitleScene::Draw(void)
{
	ObjectManager::DrawAll();
}

//=============================================================================
// コンストラクタ処理（初期化）
//=============================================================================
TitleScene::TitleScene(void)
{
	//new Copyright;
	//new AirWaterFream;
	ObjectManager::CreateObject<DebugObject>();
	ObjectManager::CreateObject<PlayerManager>();
	ObjectManager::CreateObject<Stage>();
	//ObjectManager::CreateObject<Skydome>();
	ObjectManager::CreateObject<Cube>();
	ObjectManager::CreateObject<EffectManager>();

	//new Player;
	//new Skydome;
	//new Cube;
	//new Stencil;


	//// 指定オブジェクト取得テスト
	//Object *pTest1 = Object::GetObjectPointer(Object::PLAYER);
	//Object *pTest2 = Object::GetObjectPointer(Object::COPYRIGHT);
}

//=============================================================================
// デストラクタ処理（終了）
//=============================================================================
TitleScene::~TitleScene(void)
{
	ObjectManager::ReleaseAll();
}

//=============================================================================
// タイトルデモのリスタートメソッド
//=============================================================================
void TitleScene::DemoRestart(void)
{

}
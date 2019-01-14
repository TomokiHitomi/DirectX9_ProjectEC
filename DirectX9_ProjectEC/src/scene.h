//=============================================================================
//
// シーン処理 [scene.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _SCENE_H_
#define _SCENE_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"
#include "shader.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCENE_UIOBJECT_MAX		(10)
#define SCENE_GAME_WIN			(50.0f)		// 勝利割合

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class BaseScene
{
public:
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual ~BaseScene() {};
};

class SceneManager
{
public:
	// シーンリスト
	enum SCENE 	{TITLE, GAME, RESULT, MAX	};
	// コンストラクタ
	SceneManager(void);
	// デストラクタ
	~SceneManager(void);
	// シーンを引数によって変える関数
	static void			ChangeScene(SCENE scene);
	// シーンの初期化関数
	static void			Init(HINSTANCE hInst, HWND hWnd);
	// シーンの終了関数
	static void			Uninit(void);
	// シーンの更新関数
	static void			Update(void);
	// シーンの描画関数
	static void			Draw(void);

	// 現在のシーン取得
	static SCENE		GetScene(void)				{ return m_eScene; }

	// 残存割合設定
	static void			SetRate(float fRate)		{ m_fRate = fRate; }
	// 残存割合取得
	static float		GetRate(void)				{ return m_fRate; }
	// 残存割合設定（ボーナス込み）
	static void			SetRateInBonus(float fRateInBonus)	{ m_fRateInBonus = fRateInBonus; }
	// 残存割合取得（ボーナス込み）
	static float		GetRateInBonus(void)				{ return m_fRateInBonus; }

	// 勝利プレイヤー設定
	static void			SetWin(int nWin)			{ m_nWin = nWin; }
	// 勝利プレイヤー取得
	static int			GetWin(void)				{ return m_nWin; }

	// AIの強さ設定
	static void			SetAiLevel(int nAiLevel)	{ m_nAiLevel = nAiLevel; }
	// AIの強さ取得
	static int			GetAiLevel(void)			{ return m_nAiLevel; }

	// プレイヤー人数設定
	static void			SetPlayerNumS(int nNum)		{ m_nPlayerNum = nNum; }
	// プレイヤー人数取得
	static int			GetPlayerNumS(void)			{ return m_nPlayerNum; }
	// プレイヤータイプ設定
	static void			SetPlayerTypeS(int nType)	{ m_nPlayerType = nType; }
	// プレイヤータイプ取得
	static int			GetPlayerTypeS(void)		{ return m_nPlayerType; }
	// SEの終了関数
	static void			EndSeS(void);

	static int			m_nTotalBurnBonus;
	static int			m_nTotalWetBonus;
	static int			m_nTotalBurnChain;

private:
	static BaseScene	*m_pScene;			// 現在のシーン
	static SCENE		m_eScene;			// 現在のシーン番号
	static float		m_fRate;			// 残存割合
	static float		m_fRateInBonus;		// 残存割合（ボーナス込み）
	static int			m_nWin;				// 勝利プレイヤー
	static int			m_nAiLevel;			// AIの強さ
	static int			m_nPlayerNum;		// プレイヤー人数
	static int			m_nPlayerType;		// プレイヤータイプ

	static ShaderManager	*ShaderMgr;
	static CameraManager	*CameraMgr;




#ifdef _DEBUG
	// デバッグ用シーン遷移
	static void DebugScene(void);
#endif
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif
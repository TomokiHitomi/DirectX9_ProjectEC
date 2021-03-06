//=============================================================================
//
// 入力処理 [sound.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef ___SOUND_H___
#define ___SOUND_H___

#include <windows.h>
#include <tchar.h>
#include <dsound.h>
#include <mmsystem.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VOLUME_CONTROL_UP		(120)			// 音量上昇幅
#define VOLUME_CONTROL_DOWN		(20)			// 音量下降幅
#define VOLUME_MAX				(-500)			// 最大音量
#define VOLUME_HALF				(-2000)			// 中間音量
#define VOLUME_MIN				(-8000)			// 最小音量
#define VOLUME_SE				(-800)			// SEボリューム
#define VOLUME_VOICE			(-500)			// ボイスボリューム

#define SE_11_CONTINUITY		(5)				// ヒットSEの再生間隔
#define SE_11_VOLUME_SE			(-1200)			// SE11のボリューム設定

//#define SOUND_BGM_MAX			(BGM_MAX)
//#define SOUND_SE_MAX			(VOICE_00 - SE_00)
//#define SOUND_VOICE_MAX			(SOUND_MAX - VOICE_00)

#define SOUND_SE_DRAMROLL_END	(1000000)

enum
{	// BGM
	BGM_GAME,
	BGM_WHITELILY_V,
	BGM_RESULT,
	BGM_SELECT,
	BGM_RESULT2,
	BGM_MAX
};

enum
{	// SE
	SE_WING,
	SE_MAX
};

enum
{	// VOICE
	VOICE_ATK1,		// たぁっ！
	VOICE_ATK2,		// スキあり！
	VOICE_ATK3,		// かわせるかしら！？
	VOICE_ATK4,		// はあーっ！
	VOICE_GUARD1,	// きかないわ！
	VOICE_MAX
};

enum
{	// 再生用フラグ
	E_DS8_FLAG_NONE,
	E_DS8_FLAG_LOOP,	// DSBPLAY_LOOPING=1
	E_DS8_FLAG_MAX
};

enum
{	// 再生用フラグ
	VOLUME_FADE_OUT,
	VOLUME_FADE_IN,
	VOLUME_FADE_HALF
};

enum
{	// 連続再生フラグ
	CONTINUITY_OFF,
	CONTINUITY_ON
};

enum
{	// BGMフラグ
	SOUND_BGM_NON,
	SOUND_BGM_TITLE,
	SOUND_BGM_SELECT,
	SOUND_BGM_GAME,
	SOUND_BGM_RESULT,
	SOUND_BGM_TEST1,
	SOUND_BGM_TEST2
};

enum
{	// PlaySoundのnOption用
	SOUND_OPTION_CONTINUE_OFF,	// 連続再生オフ（連打無効）
	SOUND_OPTION_CONTINUE_ON,	// 連続再生オン（連打対応）
	SOUND_OPTION_BGM,			// BGM再生時に使用する
	SOUND_OPTION_CURRENT,		// カレントポジション変更（連打無効）
	SOUND_OPTION_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT					InitSound( HWND hWnd );	// 初期化
void					UninitSound();			// 後片付け
void					UpdateSound(void);
LPDIRECTSOUNDBUFFER8	LoadSound( int no );	// サウンドのロード
void					PlaySound(LPDIRECTSOUNDBUFFER8 pBuffer, int flag/*=0*/, int nOption, DWORD dwPos);
//void					PlayContinuitySound(LPDIRECTSOUNDBUFFER8 pBuffer, int flag = 0 );
bool					IsPlaying( LPDIRECTSOUNDBUFFER8 pBuffer );					// 再生中かどうか
HRESULT					SetVol(LPDIRECTSOUNDBUFFER8 pBuffer, LONG lVol);
void					SetSoundBgm(int nBgm);
void					SetSoundBgmBoss(bool bBgm);
void					SetSe(int nSe, int nFlag, int nOption, DWORD dwPos);
void					EndSe(int nSe);
void					SetVoice(int nVoice, int nFlag, int nOption, DWORD dwPos);
#endif
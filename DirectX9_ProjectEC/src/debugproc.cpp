//=============================================================================
//
// デバッグ表示処理 [debugproc.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include <stdio.h>
#include "debugproc.h"
#include "input.h"
#include "scene.h"
#include "calculate.h"
#include "debugobject.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPD3DXFONT	g_pD3DXFont = NULL;			// フォントへのポインタ
char		g_aStrDebug[4096 * 10] = {"\0"};	// デバッグ情報
long		g_lDrawPos = 0;
bool		g_bDispDebug = true;
int			g_nDispColor = 0;
D3DCOLOR	g_xColor = SetColorPallet(COLOR_PALLET_RED);

extern SceneManager		g_cScene;					// Sceneマネージャ

//=============================================================================
// デバッグ表示処理の初期化
//=============================================================================
HRESULT InitDebugProc(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	HRESULT hr;

	// 情報表示用フォントを設定
	hr = D3DXCreateFont(pDevice,
		20,					// 文字の高さ
		0,					// 文字幅
		FW_MEDIUM, 			// フォントの太さ
		2,					// MIPMAPのレベル
		FALSE,				// イタリックフラグ
		SHIFTJIS_CHARSET,	// 文字セット
		OUT_DEFAULT_PRECIS,	// 出力精度
		DEFAULT_QUALITY,	// 出力品質
		DEFAULT_PITCH, 		// フォントピッチとファミリ
		"Terminal",			// フォント名
		&g_pD3DXFont);		// ポインタ

	// 情報クリア
	memset(g_aStrDebug, 0, sizeof g_aStrDebug);

	return hr;
}

//=============================================================================
// デバッグ表示処理の終了処理
//=============================================================================
void UninitDebugProc(void)
{
	if(g_pD3DXFont != NULL)
	{// 情報表示用フォントの開放
		g_pD3DXFont->Release();
		g_pD3DXFont = NULL;
	}
}

//=============================================================================
// デバッグ表示処理の更新処理
//=============================================================================
void UpdateDebugProc(void)
{
	if (GetKeyboardTrigger(DIK_F1))
	{// デバッグ表示ON/OFF
		g_bDispDebug = g_bDispDebug ? false : true;
		DebugObject::SetUse(g_bDispDebug);
	}
	if (GetKeyboardPress(DIK_LSHIFT))
	{
		if		(GetKeyboardTrigger(DIK_1)) g_xColor = SetColorPallet(COLOR_PALLET_WHITE);
		else if (GetKeyboardTrigger(DIK_2)) g_xColor = SetColorPallet(COLOR_PALLET_BLACK);
		else if (GetKeyboardTrigger(DIK_3)) g_xColor = SetColorPallet(COLOR_PALLET_GRAY);
		else if (GetKeyboardTrigger(DIK_4)) g_xColor = SetColorPallet(COLOR_PALLET_RED);
		else if (GetKeyboardTrigger(DIK_5)) g_xColor = SetColorPallet(COLOR_PALLET_GREEN);
		else if (GetKeyboardTrigger(DIK_6)) g_xColor = SetColorPallet(COLOR_PALLET_BLUE);
		else if (GetKeyboardTrigger(DIK_7)) g_xColor = SetColorPallet(COLOR_PALLET_YELLOW);
		else if (GetKeyboardTrigger(DIK_8)) g_xColor = SetColorPallet(COLOR_PALLET_MAGENTA);
		else if (GetKeyboardTrigger(DIK_9)) g_xColor = SetColorPallet(COLOR_PALLET_CYAN);
	}
	if (g_bDispDebug)
	{
		// デバッグフォントのスクロール
		if (GetKeyboardPress(DIK_9) && GetKeyboardPress(DIK_0))
		{	// 同時押しは不動
		}
		else if (GetKeyboardPress(DIK_9))
		{
			g_lDrawPos += (long)DEBUG_SCROLL_SPEED;
		}
		else if (GetKeyboardPress(DIK_0))
		{
			g_lDrawPos -= (long)DEBUG_SCROLL_SPEED;
		}

		// マウスホイール
		long ModUseZ = GetMobUseZ();
		if (ModUseZ != 0)
		{
			g_lDrawPos += ModUseZ;
		}
	}
}

//=============================================================================
// デバッグ表示処理の描画処理
//=============================================================================
void DrawDebugProc(void)
{
	if (g_bDispDebug)
	{
		RECT rect = { 0, g_lDrawPos, SCREEN_WIDTH, SCREEN_HEIGHT };

		// 現在のシーンを取得
		SceneManager::SCENE eScene = g_cScene.GetScene();

		// 情報表示（黒文字）
		g_pD3DXFont->DrawText(NULL, g_aStrDebug, -1, &rect, DT_LEFT, g_xColor);

		// 情報クリア
		memset(g_aStrDebug, 0, sizeof g_aStrDebug);
	}
}

//=============================================================================
// デバッグ操作表示
//=============================================================================
void DebugHowTo(void)
{
	PrintDebugProc("【 Debug操作方法 】\n");
	PrintDebugProc("F1:デバッグ表示/非表示  F2:TITLE  F3:GAME  F4:RESULT  ");
	PrintDebugProc("9/0/マウスホイール:デバッグスクロール  ");
	PrintDebugProc("LSHIFT押下中  WASD:カメラ移動  ↑↓:拡大縮小  Enter:カメラリセット\n");
	PrintDebugProc("\n");
}

//=============================================================================
// デバッグ表示の登録
//=============================================================================
void PrintDebugProc(const char *fmt,...)
{
#if 0
	long *pParam;
	static char aBuf[256];

	pParam = (long*)&fmt;
	sprintf(aBuf, fmt, pParam[1], pParam[2], pParam[3], pParam[4],
									pParam[5], pParam[6], pParam[7], pParam[8],
									pParam[9], pParam[10], pParam[11], pParam[12]);
#else
	va_list list;			// 可変引数を処理する為に使用する変数
	char *pCur;
	char aBuf[256]={"\0"};
	char aWk[128];

	// 可変引数にアクセスする前の初期処理
	va_start(list, fmt);

	pCur = (char*)fmt;
	for( ; *pCur; ++pCur)
	{
		if(*pCur != '%')
		{
			sprintf(aWk, "%c", *pCur);
		}
		else
		{
			pCur++;

			switch(*pCur)
			{
			case 'd':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%d", va_arg(list, int));
				break;

			case 'u':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%u", va_arg(list, int));
				break;

			case 'f':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%.2f", va_arg(list, double));		// double型で指定
				break;

			case 'l':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%.2ld", va_arg(list, long));		// long型で指定
				break;

			case 's':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%s", va_arg(list, char*));
				break;

			case 'c':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%c", va_arg(list, char));
				break;

			default:
				sprintf(aWk, "%c", *pCur);
				break;
			}
		}
		strcat(aBuf, aWk);
	}

	// 可変引数にアクセスした後の終了処理
	va_end(list);

	// 連結
	if((strlen(g_aStrDebug) + strlen(aBuf)) < sizeof g_aStrDebug - 1)
	{
		strcat(g_aStrDebug, aBuf);
	}
#endif
}



//=============================================================================
// デバッグタイマー開始/再開処理
//=============================================================================
void Debugtimer::Restart(void)
{
	QueryPerformanceFrequency(&m_liFreq);	// 単位を習得
	QueryPerformanceCounter(&m_liStart);	// 開始時間を取得
}

//=============================================================================
// デバッグタイマー終了処理
//=============================================================================
double Debugtimer::End(void)
{
	LARGE_INTEGER liEnd;
	QueryPerformanceCounter(&liEnd);		// 終了時間を取得

	// 終了 - 開始 / 単位を計算した値を返す
	return (double)(liEnd.QuadPart - m_liStart.QuadPart) / m_liFreq.QuadPart;
}
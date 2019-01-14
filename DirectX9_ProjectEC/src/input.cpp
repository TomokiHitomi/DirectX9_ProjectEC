#pragma comment (lib, "xinput.lib")
//=============================================================================
//
// 入力処理 [input.cpp]
// Author :  GP12A295 25 人見友基
//
//=============================================================================
#include "input.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX			(256)

// game pad用設定値
#define DEADZONE		2500			// 各軸の25%を無効ゾーンとする
#define RANGE_MAX		1000			// 有効範囲の最大値
#define RANGE_MIN		-1000			// 有効範囲の最小値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);

HRESULT InitializeMobUse(HINSTANCE hInst, HWND hWindow); // マウスの初期化
void UninitMobUse();						// マウスの終了処理
HRESULT UpdateMobUse();					// マウスの更新処理

HRESULT InitializePad(void);			// パッド初期化
										//BOOL CALLBACK SearchPadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID);	// パッド検査コールバック
void UpdatePad(void);
void UninitPad(void);

HRESULT InitXinput(void);
void UpdateXinput(void);
void UninitXinput(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//------------------------------- keyboard
LPDIRECTINPUT8			g_pDInput = NULL;					// IDirectInput8インターフェースへのポインタ
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// IDirectInputDevice8インターフェースへのポインタ(キーボード)
BYTE					g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ

															//--------------------------------- mobUse
static LPDIRECTINPUTDEVICE8 pMobUse = NULL; // mobUse

static DIMOUSESTATE2	mobUseState;		// マウスのダイレクトな状態
static DIMOUSESTATE2	mobUseTrigger;		// 押された瞬間だけON
static int				g_nMobCount;
										//--------------------------------- game pad

static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = { NULL,NULL,NULL,NULL };// パッドデバイス

static DWORD	padState[GAMEPADMAX];	// パッド情報（複数対応）
static DWORD	padTrigger[GAMEPADMAX];
static DWORD	padRelease[GAMEPADMAX];

static int		padCount = 0;			// 検出したパッドの数

float			padlX;
float			padlY;
LONG			padlRx;
float			padlRy;
float			padlZ;
float			padlRz;
D3DXVECTOR3		g_vecGyro;
DIDEVCAPS		g_diDevCaps;

// ジャイロテスト用
LONG			lGyroX = 0, lGyroY = 0, lGyroZ = 0;
float			fGyroX = 0.0f, fGyroY = 0.0f, fGyroZ = 0.0f;
float			fPreX = 0.0f, fPreY = 0.0f, fPreZ = 0.0f;

int				g_nJoyconSlider;

/* XInput */
static XINPUT_STATE		g_xState[GAMEPADMAX_XINPUT];
static XINPUT_VIBRATION	g_xVibration[GAMEPADMAX_XINPUT];

static DWORD			g_dwPadState[GAMEPADMAX_XINPUT];	// パッド情報（複数対応）
static DWORD			g_dwPadTrigger[GAMEPADMAX_XINPUT];
static DWORD			g_dwPadRelease[GAMEPADMAX_XINPUT];

static int				g_nPadCountX = 0;			// 検出したパッドの数

													// デバッグ用
#ifdef _DEBUG
 bool					g_bDebug;					// デバッグフォント表示フラグ
#endif


//=============================================================================
// 入力処理の初期化
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if (!g_pDInput)
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// キーボードの初期化
	InitKeyboard(hInst, hWnd);

	// マウスの初期化
	InitializeMobUse(hInst, hWnd);

	// パッドの初期化
	InitializePad();

	// XInputの初期化
	InitXinput();

	return S_OK;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void UninitInput(void)
{
	// キーボードの終了処理
	UninitKeyboard();

	// マウスの終了処理
	UninitMobUse();

	// パッドの終了処理
	UninitPad();

	// XInputの初期化
	UninitXinput();

	if (g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void UpdateInput(void)
{
#ifdef _DEBUG
	PrintDebugProc("【INPUT】\n");
	PrintDebugProc("Ｃキー：パッド状態表示\n");
	PrintDebugProc("\n");
	// デバッグ情報の表示非表示
	if (GetKeyboardTrigger(DIK_C))
	{
		g_bDebug = g_bDebug ? false : true;
	}
#endif

	// キーボードの更新
	UpdateKeyboard();

	// マウスの更新
	UpdateMobUse();

	// パッドの更新
	//UpdatePad();

	// XInputの初期化
	UpdateXinput();



#ifdef _DEBUG
	//PrintDebugProc("【 INPUT 】\n");
	//PrintDebugProc("Mouse[X:%l Y:%l Z:%l]\n", GetMobUseX(), GetMobUseY(), GetMobUseZ());
	//PrintDebugProc("\n");
#endif
}

//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if (FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	if (g_pDIDevKeyboard)
	{
		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = NULL;
	}
}

//=============================================================================
// キーボードの更新
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	// 前回のデータを保存
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if (SUCCEEDED(hr))
	{
		for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			g_keyStateTrigger[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & g_keyState[cnt];
			g_keyStateRelease[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & ~g_keyState[cnt];
			g_keyStateRepeat[cnt] = g_keyStateTrigger[cnt];

			if (g_keyState[cnt])
			{
				g_keyStateRepeatCnt[cnt]++;
				if (g_keyStateRepeatCnt[cnt] >= 20)
				{
					g_keyStateRepeat[cnt] = g_keyState[cnt];
				}
			}
			else
			{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		// キーボードへのアクセス権を取得
		g_pDIDevKeyboard->Acquire();
	}

	return S_OK;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyboardPress(int key)
{
	return (g_keyState[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return (g_keyStateTrigger[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return (g_keyStateRepeat[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return (g_keyStateRelease[key] & 0x80) ? true : false;
}


//=============================================================================
// マウス関係の処理
//=============================================================================
// マウスの初期化
HRESULT InitializeMobUse(HINSTANCE hInst, HWND hWindow)
{
	HRESULT result;
	RECT rectMove;

	// マウスの操作カウントを初期化
	g_nMobCount = 0;

	// デバイス作成
	result = g_pDInput->CreateDevice(GUID_SysMouse, &pMobUse, NULL);
	if (FAILED(result) || pMobUse == NULL)
	{
		MessageBox(hWindow, "No mobUse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}
	// データフォーマット設定
	result = pMobUse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		MessageBox(hWindow, "Can't setup mobUse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	// 他のアプリと協調モードに設定
	result = pMobUse->SetCooperativeLevel(hWindow, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(result))
	{
		MessageBox(hWindow, "MobUse mode error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}


	// デバイスの設定
	DIPROPDWORD prop;

	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = DIPROPAXISMODE_REL;		// マウスの移動値　相対値

	result = pMobUse->SetProperty(DIPROP_AXISMODE, &prop.diph);
	if (FAILED(result))
	{
		MessageBox(hWindow, "MobUse property error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	// アクセス権を得る
	pMobUse->Acquire();

	// マウスカーソル非表示
	ShowCursor(false);

	SetRect(&rectMove, 0, 0, 1280 * SCREEN_SCALE, 720 * SCREEN_SCALE);
	return result;
}
//---------------------------------------------------------
void UninitMobUse()
{
	if (pMobUse)
	{
		ShowCursor(true);
		pMobUse->Unacquire();
		pMobUse->Release();
		pMobUse = NULL;
	}

}
//-----------------------------------------------------------
HRESULT UpdateMobUse()
{
	HRESULT result;
	// 前回の値保存
	DIMOUSESTATE2 lastMobUseState = mobUseState;

	// データ取得
	result = pMobUse->GetDeviceState(sizeof(mobUseState), &mobUseState);
	if (SUCCEEDED(result))
	{
		mobUseTrigger.lX = mobUseState.lX;
		mobUseTrigger.lY = mobUseState.lY;
		mobUseTrigger.lZ = mobUseState.lZ;
		// マウスのボタン状態
		for (int i = 0; i<8; i++)
		{
			mobUseTrigger.rgbButtons[i] = ((lastMobUseState.rgbButtons[i] ^
				mobUseState.rgbButtons[i]) & mobUseState.rgbButtons[i]);
		}

		// マウス操作中
		if (lastMobUseState.lX != mobUseState.lX
			|| lastMobUseState.lY != mobUseState.lY
			|| lastMobUseState.lZ != mobUseState.lZ)
		{
			// マウスカーソル表示
			ShowCursor(true);
			// マウス操作カウントを初期化
			g_nMobCount = 0;
		}

	}
	else	// 取得失敗
	{
		// アクセス権を得てみる
		result = pMobUse->Acquire();
	}

	// マウス操作カウント
	g_nMobCount++;
	if (g_nMobCount > MOUSE_COUNT_MAX)
	{
		// マウスカーソル非表示
		ShowCursor(false);
	}

	return result;
}


//----------------------------------------------
BOOL IsMobUseLeftPressed(void)
{
	return (BOOL)(mobUseState.rgbButtons[0] & 0x80);	// 押されたときに立つビットを検査
}
BOOL IsMobUseLeftTriggered(void)
{
	return (BOOL)(mobUseTrigger.rgbButtons[0] & 0x80);
}
BOOL IsMobUseRightPressed(void)
{
	return (BOOL)(mobUseState.rgbButtons[1] & 0x80);
}
BOOL IsMobUseRightTriggered(void)
{
	return (BOOL)(mobUseTrigger.rgbButtons[1] & 0x80);
}
BOOL IsMobUseCenterPressed(void)
{
	return (BOOL)(mobUseState.rgbButtons[2] & 0x80);
}
BOOL IsMobUseCenterTriggered(void)
{
	return (BOOL)(mobUseTrigger.rgbButtons[2] & 0x80);
}
//------------------
long GetMobUseX(void)
{
	return mobUseState.lX;
}
long GetMobUseY(void)
{
	return mobUseState.lY;
}
long GetMobUseZ(void)
{
	return mobUseState.lZ;
}
//================================================= game pad
//---------------------------------------- コールバック関数
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID)
{
	HRESULT result;

	result = g_pDInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], NULL);
	return DIENUM_CONTINUE;	// 次のデバイスを列挙

}
//---------------------------------------- 初期化
HRESULT InitializePad(void)			// パッド初期化
{
	HRESULT		result;
	int			i;

	padCount = 0;
	// ジョイパッドを探す
	g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);
	// セットしたコールバック関数が、パッドを発見した数だけ呼ばれる。

	for (i = 0; i<padCount; i++) {
		// ジョイスティック用のデータ・フォーマットを設定
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick2);
		if (FAILED(result))
			return false; // データフォーマットの設定に失敗

						  // モードを設定（フォアグラウンド＆非排他モード）
						  //		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
						  //		if ( FAILED(result) )
						  //			return false; // モードの設定に失敗

						  // 軸の値の範囲を設定
						  // X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
						  // (max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
						  // 差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドの性能による)
		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;
		// X軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Z軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// RX軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_RX;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// RY軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_RY;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// GYRO用のRANGEを設定
		diprg.lMin = RANGE_MIN_GYRO;
		diprg.lMax = RANGE_MAX_GYRO;
		// RZ軸の範囲を設定（Z回転）
		diprg.diph.dwObj = DIJOFS_RZ;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// SLIDER(0)の範囲を設定
		diprg.diph.dwObj = DIJOFS_SLIDER(0);
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// SLIDER(1)の範囲を設定
		diprg.diph.dwObj = DIJOFS_SLIDER(1);
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);



		// 各軸ごとに、無効のゾーン値を設定する。
		// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
		// 指定する値は、10000に対する相対値(2000なら20パーセント)。
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		//X軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		// Z軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		// RX軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_RX;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		// RY軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_RY;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		//// GYRO用のRANGEを設定
		//dipdw.dwData = DEADZONE_GYRO;
		//// RZ軸の無効ゾーンを設定（Z回転）
		//dipdw.diph.dwObj = DIJOFS_RZ;
		//pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//// SLIDER(0)の無効ゾーンを設定
		//dipdw.diph.dwObj = DIJOFS_SLIDER(0);
		//pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//// SLIDER(1)の無効ゾーンを設定
		//dipdw.diph.dwObj = DIJOFS_SLIDER(1);
		//pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		//ジョイスティック入力制御開始
		pGamePad[i]->Acquire();
	}

	return true;

}
//------------------------------------------- 終了処理
void UninitPad(void)
{
	for (int i = 0; i<GAMEPADMAX; i++) {
		if (pGamePad[i])
		{
			pGamePad[i]->Unacquire();
			pGamePad[i]->Release();
		}
	}

}

//------------------------------------------ 更新
void UpdatePad(void)
{


	HRESULT			result;
	DIJOYSTATE2		dijs;
	int				i;

	for (i = 0; i<padCount; i++)
	{
		DWORD lastPadState;
		lastPadState = padState[i];
		padState[i] = 0x00000000l;	// 初期化

		result = pGamePad[i]->Poll();	// ジョイスティックにポールをかける
		if (FAILED(result)) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE2), &dijs);	// デバイス状態を読み取る
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		// ３２の各ビットに意味を持たせ、ボタン押下に応じてビットをオンにする

		//* 方向キー上
		if (dijs.rgdwPOV[0] == 0)		padState[i] |= BUTTON_UP;
		//* 方向キー下
		if (dijs.rgdwPOV[0] == 18000)	padState[i] |= BUTTON_DOWN;
		//* 方向キー左
		if (dijs.rgdwPOV[0] == 27000)	padState[i] |= BUTTON_LEFT;
		//* 方向キー右
		if (dijs.rgdwPOV[0] == 9000)	padState[i] |= BUTTON_RIGHT;

		//* 方向キー右上
		if (dijs.rgdwPOV[0] == 4500)	padState[i] |= BUTTON_UP, padState[i] |= BUTTON_RIGHT;
		//* 方向キー右下
		if (dijs.rgdwPOV[0] == 13500)	padState[i] |= BUTTON_DOWN, padState[i] |= BUTTON_RIGHT;
		//* 方向キー左下
		if (dijs.rgdwPOV[0] == 22500)	padState[i] |= BUTTON_DOWN, padState[i] |= BUTTON_LEFT;
		//* 方向キー左上
		if (dijs.rgdwPOV[0] == 31500)	padState[i] |= BUTTON_UP, padState[i] |= BUTTON_LEFT;

		//* Ａボタン
		if (dijs.rgbButtons[0] & 0x80)	padState[i] |= BUTTON_A;
		//* Ｂボタン
		if (dijs.rgbButtons[1] & 0x80)	padState[i] |= BUTTON_B;
		//* Ｃボタン
		if (dijs.rgbButtons[2] & 0x80)	padState[i] |= BUTTON_C;
		//* Ｘボタン
		if (dijs.rgbButtons[3] & 0x80)	padState[i] |= BUTTON_X;
		////* Ｙボタン
		//if (dijs.rgbButtons[4] & 0x80)	padState[i] |= BUTTON_Y;
		////* Ｚボタン
		//if (dijs.rgbButtons[5] & 0x80)	padState[i] |= BUTTON_Z;
		//* Ｌボタン
		if (dijs.rgbButtons[6] & 0x80)	padState[i] |= BUTTON_L;
		//* Ｒボタン
		if (dijs.rgbButtons[7] & 0x80)	padState[i] |= BUTTON_R;

		//* キャプチャーボタン[13]
		if (dijs.rgbButtons[13] & 0x80)	padState[i] |= BUTTON_CAP;
		//* Homeボタン[28]
		if (dijs.rgbButtons[28] & 0x80)	padState[i] |= BUTTON_HOME;
		//* ＳＴＡＲＴボタン
		if (dijs.rgbButtons[9] & 0x80)	padState[i] |= BUTTON_START;
		//* Ｍボタン
		if (dijs.rgbButtons[8] & 0x80)	padState[i] |= BUTTON_MINUS;

		//*
		if (dijs.rgbButtons[16] & 0x80)	padState[i] |= R_BUTTON_Y;
		//*
		if (dijs.rgbButtons[17] & 0x80)	padState[i] |= R_BUTTON_X;
		//*
		if (dijs.rgbButtons[18] & 0x80)	padState[i] |= R_BUTTON_B;
		//*
		if (dijs.rgbButtons[19] & 0x80)	padState[i] |= R_BUTTON_A;
		//*
		if (dijs.rgbButtons[22] & 0x80)	padState[i] |= R_BUTTON_R;
		//*
		if (dijs.rgbButtons[23] & 0x80)	padState[i] |= R_BUTTON_ZR;
		//*
		if (dijs.rgbButtons[25] & 0x80)	padState[i] |= R_BUTTON_PLUS;
		//*
		if (dijs.rgbButtons[21] & 0x80)	padState[i] |= R_BUTTON_LEFT;

		//* y-axis (forward)
		if (dijs.lY < 0)				padState[i] |= LSTICK_UP;
		//* y-axis (backward)
		if (dijs.lY > 0)				padState[i] |= LSTICK_DOWN;
		//* x-axis (left)
		if (dijs.lX < 0)				padState[i] |= LSTICK_LEFT;
		//* x-axis (right)
		if (dijs.lX > 0)				padState[i] |= LSTICK_RIGHT;


		//* 右スティック上
		if (dijs.lRy < 0)				padState[i] |= RSTICK_UP;
		//* 右スティック下
		if (dijs.lRy > 0)				padState[i] |= RSTICK_DOWN;
		//* 右スティック左
		if (dijs.lRx < 0)				padState[i] |= RSTICK_LEFT;
		//* 右スティック右
		if (dijs.lRx > 0)				padState[i] |= RSTICK_RIGHT;


		////* 右スティック上
		//if (dijs.lRz < 0x8000 - RSTICK_MARGIN)	padState[i] |= RSTICK_UP;
		////* 右スティック下
		//if (dijs.lRz > 0x8000 + RSTICK_MARGIN)	padState[i] |= RSTICK_DOWN;
		////* 右スティック左
		//if (dijs.lZ < 0x8000 - RSTICK_MARGIN)	padState[i] |= RSTICK_LEFT;
		////* 右スティック右
		//if (dijs.lZ > 0x8000 + RSTICK_MARGIN)	padState[i] |= RSTICK_RIGHT;

		//* 左スティックボタン
		if (dijs.rgbButtons[10] & 0x80)	padState[i] |= LSTICK_BUTTON;
		//* 右スティックボタン
		if (dijs.rgbButtons[11] & 0x80)	padState[i] |= RSTICK_BUTTON;


		// Trigger設定
		padTrigger[i] = ((lastPadState ^ padState[i])	// 前回と違っていて
			& padState[i]);					// しかも今ONのやつ
											// Release設定
		padRelease[i] = ((lastPadState ^ padState[i])	// 前回と違っていて
			& ~padState[i]);					// しかも今OFFのやつ

		if (i == 0)
		{
			// 右スティックの傾き量を格納
			padlRx = dijs.lRx;
			padlRy = dijs.lRy;

			// 左スティックの傾き量を格納
			padlZ = dijs.lZ;
			padlRz = dijs.lRz;

			// ジャイロ情報を格納
			g_vecGyro = D3DXVECTOR3((float)dijs.rglSlider[0], (float)dijs.rglSlider[1], dijs.lRz);
		}


#ifdef _DEBUG
		if (g_bDebug)
		{
			PrintDebugProc("【GAME PAD】\n");
			PrintDebugProc("LStick[X:%l  Y:%l  Z:%l]  RStick[X:%l  Y:%l  Z:%l]\n",
				dijs.lX, dijs.lY, dijs.lZ, dijs.lRx, dijs.lRy, dijs.lRz);
			PrintDebugProc("POV[UP:%d  RIGHT:%d  DOWN:%d  LEFT:%d]\n",
				dijs.rgdwPOV[0], dijs.rgdwPOV[1], dijs.rgdwPOV[2], dijs.rgdwPOV[3]);
			PrintDebugProc("lV[X:%l  Y:%l  Z:%l]  lVR[X:%l  Y:%l  Z:%l]\n",
				dijs.lVX, dijs.lVY, dijs.lVZ, dijs.lVRx, dijs.lVRy, dijs.lVRz);
			PrintDebugProc("lA[X:%l  Y:%l  Z:%l]  lAR[X:%l  Y:%l  Z:%l]\n",
				dijs.lAX, dijs.lAY, dijs.lAZ, dijs.lARx, dijs.lARy, dijs.lARz);
			PrintDebugProc("lF[X:%l  Y:%l  Z:%l]  lFR[X:%l  Y:%l  Z:%l]\n",
				dijs.lFX, dijs.lFY, dijs.lFZ, dijs.lFRx, dijs.lFRy, dijs.lFRz);
			PrintDebugProc("rglSlider[0:%l  1:%l]  rglVSlider[0:%l  1:%l]\n",
				dijs.rglSlider[0], dijs.rglSlider[1], dijs.rglVSlider[0], dijs.rglVSlider[1]);
			PrintDebugProc("rglASlider[0:%l  1:%l]  rglFSlider[0:%l  1:%l]\n",
				dijs.rglASlider[0], dijs.rglASlider[1], dijs.rglFSlider[0], dijs.rglFSlider[1]);
			PrintDebugProc("rgbButtons\n");

			if (GetKeyboardTrigger(DIK_R))
			{
				lGyroX = 0;
				lGyroY = 0;
				lGyroZ = 0;
				fGyroX = 0.0f;
				fGyroY = 0.0f;
				fGyroZ = 0.0f;
			}
			lGyroX += dijs.lRz;
			lGyroY += dijs.rglSlider[1];
			lGyroZ += dijs.rglSlider[0];

			float fDegree;

			fDegree = (fPreX + dijs.lRz) * 15 / 2;
			fGyroX += fDegree * 0.0174533f;

			fDegree = (fPreY + dijs.rglSlider[1]) * 15 / 2;
			fGyroY += fDegree * 0.0174533f;

			fDegree = (fPreZ + dijs.rglSlider[0]) * 15 / 2;
			fGyroZ += fDegree * 0.0174533f;


			fPreX = dijs.lRz;
			fPreY = dijs.rglSlider[1];
			fPreZ = dijs.rglSlider[0];


			PrintDebugProc("lGyro[X:%l  Y:%l  Z:%l]\n",
				lGyroX, lGyroY, lGyroZ);

			PrintDebugProc("fGyro[X:%f  Y:%f  Z:%f]\n",
				fGyroX, fGyroY, fGyroZ);



			for (int i = 0; i < 128; i++)
			{
				PrintDebugProc("%d", dijs.rgbButtons[i]);
				if (i % 32 == 0 && i != 0)
				{
					PrintDebugProc("\n");
				}
			}
			PrintDebugProc("[%f] [%f]\n", (float)padlZ, (float)padlRz);
			PrintDebugProc("\n");
		}
#endif
	}

}
//----------------------------------------------- 検査
BOOL IsButtonPressed(int padNo, DWORD button)
{
	return (button & padState[padNo]);
}

BOOL IsButtonTriggered(int padNo, DWORD button)
{
	return (button & padTrigger[padNo]);
}

BOOL IsButtonReleased(int padNo, DWORD button)
{
	return (button & padRelease[padNo]);
}


//=============================================================================
// XInput初期化処理
//=============================================================================
HRESULT InitXinput(void)
{
	DWORD dwResult;
	for (DWORD i = 0; i< GAMEPADMAX_XINPUT; i++)
	{
		ZeroMemory(&g_xState[i], sizeof(XINPUT_STATE));
		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState( i, &g_xState[i]);
		if( dwResult == ERROR_SUCCESS )
		{
			// Controller is connected
			g_nPadCountX++;
			ZeroMemory(&g_xVibration[i], sizeof(XINPUT_VIBRATION));
			ZeroMemory(&g_dwPadState[i], sizeof(DWORD));
			ZeroMemory(&g_dwPadTrigger[i], sizeof(DWORD));
			ZeroMemory(&g_dwPadRelease[i], sizeof(DWORD));
		}
		else
		{
			// Controller is not connected
		}
	}

	return S_OK;
}

//=============================================================================
// XInput終了処理
//=============================================================================
void UninitXinput(void)
{

}

//=============================================================================
// XInput更新処理
//=============================================================================
void UpdateXinput(void)
{
	XINPUT_STATE xState;

	for (DWORD i = 0; i < g_nPadCountX; i++)
	{
		ZeroMemory(&xState, sizeof(XINPUT_STATE));
		// Simply get the state of the controller from XInput.
		XInputGetState(i, &xState);

		// ステートをグローバルに保存
		// 主に範囲が設定できるスティック・トリガー向け
		g_xState[i] = xState;

		// ステートをコピー
		DWORD dwLastPadState = g_dwPadState[i];

		// スティック・トリガーをボタンとして扱えるよう
		// WORD型からDWORD形へ拡張する
		g_dwPadState[i] = (DWORD)xState.Gamepad.wButtons;

		// スティック・トリガーをステートに追加格納
		if (xState.Gamepad.sThumbLY > 0)		g_dwPadState[i] |= XBOTTON_STICK_UP;
		if (xState.Gamepad.sThumbLY < 0)		g_dwPadState[i] |= XBOTTON_STICK_DOWN;
		if (xState.Gamepad.sThumbLX < 0)		g_dwPadState[i] |= XBOTTON_STICK_LEFT;
		if (xState.Gamepad.sThumbLX > 0)		g_dwPadState[i] |= XBOTTON_STICK_RIGHT;

		if (xState.Gamepad.sThumbRY > 0)		g_dwPadState[i] |= XBOTTON_STICKR_UP;
		if (xState.Gamepad.sThumbRY < 0)		g_dwPadState[i] |= XBOTTON_STICKR_DOWN;
		if (xState.Gamepad.sThumbRX < 0)		g_dwPadState[i] |= XBOTTON_STICKR_LEFT;
		if (xState.Gamepad.sThumbRX > 0)		g_dwPadState[i] |= XBOTTON_STICKR_RIGHT;

		if (xState.Gamepad.bLeftTrigger > 0)	g_dwPadState[i] |= XBOTTON_LT;
		if (xState.Gamepad.bRightTrigger > 0)	g_dwPadState[i] |= XBOTTON_RT;

		// Trigger設定
		g_dwPadTrigger[i] = ((dwLastPadState ^ g_dwPadState[i])	// 前回と違っていて
			& g_dwPadState[i]);					// しかも今ONのやつ

		// Release設定
		g_dwPadRelease[i] = ((dwLastPadState ^ g_dwPadState[i])	// 前回と違っていて
			& ~g_dwPadState[i]);					// しかも今OFFのやつ

		// バイブレーションを設定
		XInputSetState(i, &g_xVibration[i]);

#ifdef _DEBUG
		// バイブレーションテスト
		if (XButtonPress(i, XINPUT_GAMEPAD_LEFT_THUMB))
		{	// 左・右モーターの回転数を上げる
			g_xVibration[i].wLeftMotorSpeed = 
				min(g_xVibration[i].wLeftMotorSpeed + XINPUT_VIBRATION_DEBUG, XINPUT_VIBRATION_MAX);
			g_xVibration[i].wRightMotorSpeed =
				min(g_xVibration[i].wRightMotorSpeed + XINPUT_VIBRATION_DEBUG, XINPUT_VIBRATION_MAX);
		}
		if (XButtonPress(i, XINPUT_GAMEPAD_RIGHT_THUMB))
		{	// 左・右モーターの回転数を下げる
			g_xVibration[i].wLeftMotorSpeed =
				max(g_xVibration[i].wLeftMotorSpeed - XINPUT_VIBRATION_DEBUG, 0);
			g_xVibration[i].wRightMotorSpeed =
				max(g_xVibration[i].wRightMotorSpeed - XINPUT_VIBRATION_DEBUG, 0);
		}

		if (g_bDebug)
		{
			// デバッグフォント表示
			PrintDebugProc("【XINPUT】\n");
			//PrintDebugProc("左スティック押込:バイブ増\n");
			//PrintDebugProc("右スティック押込:バイブ減\n");
			PrintDebugProc("PAD       [%d]   PacketNum[%d]\n", i, xState.dwPacketNumber);
			PrintDebugProc("THUMB     [LX:%d LY:%d RX:%d RY:%d]\n",
				xState.Gamepad.sThumbLX, xState.Gamepad.sThumbLY,
				xState.Gamepad.sThumbRX, xState.Gamepad.sThumbRY);
			PrintDebugProc("TRIGGER   [LEFT:%d RIGHT:%d]\n",
				xState.Gamepad.bLeftTrigger, xState.Gamepad.bRightTrigger);
			PrintDebugProc("DPAD      [UP:%d DOWN:%d LEFT:%d RIGHT:%d]\n",
				XButtonPress(i, XBOTTON_DPAD_UP),
				XButtonPress(i, XBOTTON_DPAD_DOWN),
				XButtonPress(i, XBOTTON_DPAD_LEFT),
				XButtonPress(i, XBOTTON_DPAD_RIGHT));
			PrintDebugProc("LSTICK    [UP:%d DOWN:%d LEFT:%d RIGHT:%d]\n",
				XButtonPress(i, XBOTTON_STICK_UP),
				XButtonPress(i, XBOTTON_STICK_DOWN),
				XButtonPress(i, XBOTTON_STICK_LEFT),
				XButtonPress(i, XBOTTON_STICK_RIGHT));
			PrintDebugProc("RSTICK    [UP:%d DOWN:%d LEFT:%d RIGHT:%d]\n",
				XButtonPress(i, XBOTTON_STICKR_UP),
				XButtonPress(i, XBOTTON_STICKR_DOWN),
				XButtonPress(i, XBOTTON_STICKR_LEFT),
				XButtonPress(i, XBOTTON_STICKR_RIGHT));
			PrintDebugProc("THUMB     [LEFT:%d RIGHT:%d]\n",
				XButtonPress(i, XBOTTON_LEFT_THUMB),
				XButtonPress(i, XBOTTON_RIGHT_THUMB));
			PrintDebugProc("SHOULDER  [LEFT:%d RIGHT:%d]\n",
				XButtonPress(i, XBOTTON_LEFT_SHOULDER),
				XButtonPress(i, XBOTTON_RIGHT_SHOULDER));
			PrintDebugProc("BUTTON    [A:%d B:%d X:%d Y:%d START:%d BACK:%d]\n",
				XButtonPress(i, XBOTTON_A),
				XButtonPress(i, XBOTTON_B),
				XButtonPress(i, XBOTTON_X),
				XButtonPress(i, XBOTTON_Y),
				XButtonPress(i, XBOTTON_START),
				XButtonPress(i, XBOTTON_BACK));
			PrintDebugProc("          [LT:%d RT:%d START:%d BACK:%d]\n",
				XButtonPress(i, XBOTTON_LT),
				XButtonPress(i, XBOTTON_RT),
				XButtonPress(i, XBOTTON_START),
				XButtonPress(i, XBOTTON_BACK));
			PrintDebugProc("VIBRATION [LEFT:%d RIGHT:%d]\n",
				g_xVibration[i].wLeftMotorSpeed, g_xVibration[i].wRightMotorSpeed);
			PrintDebugProc("\n");
		}
#endif
	}
}

bool XButtonPress(int padNo, DWORD button)
{
	return (button & g_dwPadState[padNo]);
	//return (button & g_xState[padNo].Gamepad.wButtons);
}

bool XButtonTrigger(int padNo, DWORD button)
{
	return (button & g_dwPadTrigger[padNo]);
}

bool XButtonRelease(int padNo, DWORD button)
{
	return (button & g_dwPadRelease[padNo]);
}

//=============================================================================
// XInputのゲームパッド数の取得関数
//=============================================================================
int GetXInputPacCount(void)
{
	return g_nPadCountX;
}

//=============================================================================
// XInput用バイブレーション設定処理（wVib：0～65535）
//=============================================================================
void SetXInputVibration(int padNo, int nFlag, WORD wVib)
{
	if (nFlag == XINPUT_VIBRATION_BOTH)
	{
		g_xVibration[padNo].wLeftMotorSpeed = wVib;
		g_xVibration[padNo].wRightMotorSpeed = wVib;
	}
	else if (nFlag == XINPUT_VIBRATION_LEFT)
	{
		g_xVibration[padNo].wLeftMotorSpeed = wVib;
	}
	else if (nFlag == XINPUT_VIBRATION_RIGHT)
	{
		g_xVibration[padNo].wRightMotorSpeed = wVib;
	}
}

bool InputPress(INPUT_CHECK eInput)
{
	int nKey = 0;
	DWORD dwButton = 0, dwXButton = 0;

	switch (eInput)
	{
	case INPUT_UP:
		nKey = DIK_W;
		dwButton |= BUTTON_UP;
		dwXButton |= XBOTTON_DPAD_UP;
		break;
	case INPUT_DOWN:
		nKey = DIK_S;
		dwButton |= BUTTON_DOWN;
		dwXButton |= XBOTTON_DPAD_DOWN;
		break;
	case INPUT_LEFT:
		nKey = DIK_A;
		dwButton |= BUTTON_LEFT;
		dwXButton |= XBOTTON_DPAD_LEFT;
		break;
	case INPUT_RIGHT:
		nKey = DIK_D;
		dwButton |= BUTTON_RIGHT;
		dwXButton |= XBOTTON_DPAD_RIGHT;
		break;
	case INPUT_UP_R:
		nKey = DIK_UP;
		break;
	case INPUT_DOWN_R:
		nKey = DIK_DOWN;
		break;
	case INPUT_LEFT_R:
		nKey = DIK_LEFT;
		break;
	case INPUT_RIGHT_R:
		nKey = DIK_RIGHT;
		break;
	}
	
	if ((g_keyState[nKey] & 0x80) ? true : false
		|| (dwButton & padState[0])
		|| (dwXButton & g_dwPadState[0]))
	{
		return true;
	}
	return false;
}


float GetButtonlZ(int padNo)
{
	if (padlZ < 0.0f) padlZ *= -1.0f;
	padlZ = padlZ / RANGE_MAX;
#ifdef _DEBUG
	PrintDebugProc("padlZ[%f]\n", padlZ);
#endif
	return (padlZ);
}

float GetButtonlRz(int padNo)
{
	if (padlRz < 0.0f) padlRz *= -1.0f;
	//padlRz = padlRz / RANGE_MAX;
#ifdef _DEBUG
	PrintDebugProc("padlRz[%f]\n", padlRz);
#endif
	return (padlRz);
}

//=============================================================================
// スティック情報取得関数
//=============================================================================
float GetStick(int padNo, int nStick)
{
	float fStick = 0.0f;
	switch (nStick)
	{
	case PAD_STICK_R_X:
		fStick = (float)padlRx / RANGE_MAX;
		break;
	case PAD_STICK_R_Y:
		fStick = (float)padlRy / RANGE_MAX;
		break;
	}
	return (fStick);
}

//=============================================================================
// ジャイロ情報取得関数
//=============================================================================
D3DXVECTOR3 GetGyro(void)
{
	float fSlider = 0.0f;
	D3DXVECTOR3 vecGyro;
	vecGyro = g_vecGyro * PAD_SLIDER_SPEED * g_nJoyconSlider;
	return (vecGyro);
}


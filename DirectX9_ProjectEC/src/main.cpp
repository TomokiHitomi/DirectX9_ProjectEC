//=============================================================================
//
// メイン処理 [main.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include <thread>

/* System */
#include "scene.h"
#include "input.h"
#include "joycon.h"
#include "gui.h"

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void MainLoop(HWND hWnd);
void SubLoop(void);

// ImGui用プロシージャ
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3Dオブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// デバイスオブジェクト(描画に必要)
int					g_nCountFPS = 0;		// FPSカウンタ
bool				g_bContinue = true;		// ゲーム継続フラグ

#ifdef _DEBUG
Gui g_cImgui;
#endif

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),									// 構造体のサイズ
		CS_CLASSDC,											// クラスのスタイル
		WndProc,											// ウィンドプロシージャ
		0,													// 補助メモリ
		0,													// 補助メモリ
		hInstance,											// インスタンスハンドル
		NULL,												// アイコン画像
		LoadCursor(NULL, IDC_ARROW),						// カーソル画像
		(HBRUSH)(COLOR_WINDOW + 1),							// 背景色
		NULL,												// メニュー名
		CLASS_NAME,											// クラス名
		NULL												// アイコン16x16
	};

	HWND hWnd;
	MSG msg;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL,
		NULL,
		hInstance,
		NULL);

	// DirectXの初期化(ウィンドウを作成してから行う)
	bool flag = true; // フルスクリーンモードで初期化
	if (MessageBox(hWnd, START_UP_COMMENT, "起動確認", MB_YESNO) == IDYES)
	{
		flag = false;
	}

	// 初期化処理(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, flag)))
	{
		return -1;
	}

	//ランダムの初期化
	srand((unsigned)time(NULL));

	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();
	dwCurrentTime =
		dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//std::thread t1(SubLoop);

	// メッセージループ
	while (g_bContinue)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{	// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
#ifdef _DEBUG
				PrintDebugProc("【 FPS : %d 】\n\n", g_nCountFPS);
				DebugHowTo();
#endif
				// ウィンドウが有効な場合
				if (hWnd == GetActiveWindow())
				{
					// ウィンドウの座標を取得
					RECT	lpScreen;
					GetWindowRect(hWnd, &lpScreen);

					// マウスカーソルの移動範囲を指定
					lpScreen.left = lpScreen.left + (long)MOUSE_MARGIN;
					lpScreen.top = lpScreen.bottom - (long)SCREEN_HEIGHT + (long)MOUSE_MARGIN;
					lpScreen.right = lpScreen.right - (long)MOUSE_MARGIN;
					lpScreen.bottom = lpScreen.bottom - (long)MOUSE_MARGIN;

					// マウスカーソルの移動制限
					ClipCursor(&lpScreen);

					//// マウスカーソル非表示
					//ShowCursor(false);
				}
				else
				{
					// マウスカーソル表示
					ShowCursor(true);
				}

				dwExecLastTime = dwCurrentTime;

				// 更新処理
				//JoyconUpdate();
				Update();
				//pollLoop();

				// 描画処理
				Draw();

				dwFrameCount++;
			}
		}
	}

	// ゲーム終了フラグ
	EndGame();

	// スレッド1の処理が終わるまで待機
	//t1.join();

#ifdef _DEBUG
	// imguiの終了処理
	g_cImgui.Uninit();
#endif

	DestroyWindow(hWnd);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	//case WM_KEYDOWN:
	//	switch (wParam)
	//	{
	//	case VK_ESCAPE:
	//		DestroyWindow(hWnd);
	//		break;
	//	}
	//	break;

	default:
		break;
	}

#ifdef _DEBUG
	// imguiの終了処理
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}
#endif

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// ワークをゼロクリア
	d3dpp.BackBufferCount = 1;					// バックバッファの数
	d3dpp.BackBufferWidth = SCREEN_WIDTH;		// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;		// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed = bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil = TRUE;		// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;// デプスバッファとして24ビットZバッファ8ビットステンシルバッファ作成

	if (bWindow)
	{// ウィンドウモード
		d3dpp.FullScreen_RefreshRateInHz = 0;							// リフレッシュレート
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// インターバル
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// インターバル
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	//g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用

	//g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	//g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	//g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理を設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	//g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);	// ２番目のアルファ引数

#ifdef _DEBUG
	// imguiの初期化処理
	g_cImgui.Init(hWnd, g_pD3DDevice);
#endif

	// シーンの初期化処理
	SceneManager::Init(hInstance, hWnd);

	//start();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	if (g_pD3DDevice != NULL)
	{// デバイスオブジェクトの開放
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if (g_pD3D != NULL)
	{// Direct3Dオブジェクトの開放
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// シーンの終了処理
	SceneManager::Uninit();
	//actuallyQuit();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
#ifdef _DEBUG
	// imguiの更新開始
	g_cImgui.Start();
#endif

	// 更新処理
	{	
		//Debugtimer timer2;
		SceneManager::Update();
		//PrintDebugProc("【UpdateAt】\n[%f]\n", timer2.End());
	}

#ifdef _DEBUG
	// imguiの更新終了
	g_cImgui.End();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// ステンシルバッファ＆バックバッファ＆Ｚバッファのクリア
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画の開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		//Debugtimer timer;
		// 描画処理
		SceneManager::Draw();
		//PrintDebugProc("【DrawAt】\n[%f]\n", timer.End());

#ifdef _DEBUG
		// imguiの描画処理
		g_cImgui.Draw();
#endif

		// 描画の終了
		g_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//=============================================================================
// ゲーム処理関数
//=============================================================================
void MainLoop(HWND hWnd)
{
}

//=============================================================================
// ゲーム外処理関数
//=============================================================================
void SubLoop(void)
{
	while (g_bContinue)
	{
		pollLoop();
	}
}

//=============================================================================
// ゲーム終了関数
//=============================================================================
void EndGame(void)
{
	g_bContinue = false;
}

//=============================================================================
// フレームレート取得
//=============================================================================
int GetFps(void)
{
	return g_nCountFPS;
}

//=============================================================================
// デバイスの取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

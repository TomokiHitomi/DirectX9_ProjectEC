//=============================================================================
//
// GUI処理 [gui.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "gui.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define GUI_FONT ("resources\\fonts\\GenShinGothic\\GenShinGothic-P-Normal.ttf")
#define GUI_FONT_SIZE (15.0f * SCREEN_SCALE)
#define GUI_FONT_POS (20 * SCREEN_SCALE)

#define GUI_FPS_ALERT_1 (60)
#define GUI_FPS_ALERT_2 (55)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool Gui::s_bUse = true;

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Gui::Gui(void)
{
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Gui::~Gui(void)
{
	//Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void Gui::Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{
	// imguiの初期化処理
	ImGui::CreateContext();
	ImGui_ImplDX9_Init(hWnd, pDevice);

	//// スタイルを指定（黒）
	//ImGui::StyleColorsDark();

	// I/Oを初期化
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(
		GUI_FONT,
		GUI_FONT_SIZE,
		NULL,
		io.Fonts->GetGlyphRangesJapanese()
	);
}

//=============================================================================
// 更新処理
//=============================================================================
void Gui::Uninit(void)
{
	// imguiの解放
	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext();
}

//=============================================================================
// 更新開始処理
//=============================================================================
void Gui::Start(void)
{
	// 新規フレームを作成
	ImGui_ImplDX9_NewFrame();

	// FPSの取得
	int nFps = GetFps();

	// FPSの値が指定値より上下でタイトルの色を変更
	if (nFps >= GUI_FPS_ALERT_1)
	{	// グリーン
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.7f, 0.2f, 0.5f));
	}
	else if (nFps >= GUI_FPS_ALERT_2)
	{	// イエロー
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.7f, 0.7f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.7f, 0.7f, 0.2f, 0.5f));
	}
	else
	{	// レッド
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.7f, 0.2f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.7f, 0.2f, 0.2f, 0.5f));
	}

	// ウィンドウ色を黒（半透明）に設定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.4f));
	// ウィンドウの初期位置を設定
	ImGui::SetNextWindowPos(ImVec2(GUI_FONT_POS, GUI_FONT_POS), ImGuiSetCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiSetCond_Once);

	// imguiの更新開始
	ImGui::Begin("Debug");

	// FPSを表示
	ImGui::Text("FPS [%d]\n", nFps);
}

//=============================================================================
// 更新終了処理
//=============================================================================
void Gui::End(void)
{
	// imguiの更新終了
	ImGui::End();

	// 変更したスタイルの数だけポップ
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

//=============================================================================
// 描画処理
//=============================================================================
void Gui::Draw(void)
{
	// imguiの描画
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
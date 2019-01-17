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

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool Gui::s_bUse = false;

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

	// スタイルを指定（黒）
	ImGui::StyleColorsDark();

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
	// imguiの更新
	ImGui_ImplDX9_NewFrame();
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.7f, 0.2f, 0.2f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.7f, 0.2f, 0.2f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.4f));
	ImGui::SetNextWindowPos(ImVec2(GUI_FONT_POS, GUI_FONT_POS), ImGuiSetCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiSetCond_Once);

	ImGui::Begin(u8"Debug");
	ImGui::Text("FPS [%d]\n", GetFps());
}

//=============================================================================
// 更新終了処理
//=============================================================================
void Gui::End(void)
{
	// imguiの更新終了
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

//=============================================================================
// 描画処理
//=============================================================================
void Gui::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();


	//// Z比較なし
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	// imguiの描画
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	//// Z比較あり
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

}
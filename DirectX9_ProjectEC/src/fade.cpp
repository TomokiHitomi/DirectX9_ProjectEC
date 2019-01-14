//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "fade.h"
#include "scene.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexFade(void);
void SetColor(D3DCOLOR col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_p3DTextureFade = NULL;		// テクスチャへのポインタ
VERTEX_2D				g_vertexWkFade[NUM_VERTEX];		// 頂点情報格納ワーク

D3DXCOLOR				g_color;						// フェードカラー
FADE					g_eFade;						// フェード種類

extern SceneManager		g_cScene;						// Sceneマネージャ
SceneManager::SCENE		g_eNextScene;					// 遷移先シーン

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_eFade = FADE_IN;

	// 頂点情報の設定
	MakeVertexFade();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
{
	if(g_p3DTextureFade != NULL)
	{// テクスチャの開放
		g_p3DTextureFade->Release();
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFade(void)
{
	if(g_eFade != FADE_NONE)
	{// フェード処理中
		if(g_eFade == FADE_OUT)
		{// フェードアウト処理
			g_color.a += FADE_RATE;		// α値を加算して画面を消していく
			if(g_color.a >= 1.0f)
			{
				int nStage = 0;

				// 次のシーンへ遷移
				g_cScene.ChangeScene(g_eNextScene);

				// フェードイン処理に切り替え
				g_color.a = 1.0f;
				SetFade(FADE_IN);
			}

			// 色を設定
			SetColor(g_color);
		}
		else if (g_eFade == FADE_OUT_HALF)
		{	// フェードハーフアウト処理
			g_color.a += FADE_RATE;		// α値を加算して画面を消していく
			if (g_color.a >= FADE_HALF)
			{
				// フェードイン処理に切り替え
				g_color.a = FADE_HALF;
			}

			// 色を設定
			SetColor(g_color);
		}
		else if(g_eFade == FADE_IN)
		{// フェードイン処理
			g_color.a -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if(g_color.a <= 0.0f)
			{
				// フェード処理終了
				g_color.a = 0.0f;
				SetFade(FADE_NONE);
			}

			// 色を設定
			SetColor(g_color);
		}
	}
}

//=============================================================================
// フェード画面
//=============================================================================
void DrawFade()
{
	if (g_eFade != FADE_NONE)
	{// フェード処理中
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// ポリゴンの描画
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, NUM_POLYGON, g_vertexWkFade, sizeof(VERTEX_2D));
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexFade(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャのパースペクティブコレクト用
	g_vertexWkFade[0].rhw =
	g_vertexWkFade[1].rhw =
	g_vertexWkFade[2].rhw =
	g_vertexWkFade[3].rhw = 1.0f;

	// 反射光の設定
	g_color = D3DXCOLOR(FADE_COLOR_R, FADE_COLOR_G, FADE_COLOR_B, 1.0f);
	g_vertexWkFade[0].diffuse = g_color;
	g_vertexWkFade[1].diffuse = g_color;
	g_vertexWkFade[2].diffuse = g_color;
	g_vertexWkFade[3].diffuse = g_color;

	// 頂点座標の設定
	g_vertexWkFade[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexWkFade[1].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	g_vertexWkFade[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	g_vertexWkFade[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// テクスチャ座標の設定
	g_vertexWkFade[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexWkFade[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexWkFade[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexWkFade[3].tex = D3DXVECTOR2(1.0f, 1.0f);	

	return S_OK;
}

//=============================================================================
// 色を設定
//=============================================================================
void SetColor(D3DCOLOR col)
{
	// 反射光の設定
	g_vertexWkFade[0].diffuse = col;
	g_vertexWkFade[1].diffuse = col;
	g_vertexWkFade[2].diffuse = col;
	g_vertexWkFade[3].diffuse = col;
}

//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE fade)
{
	switch (fade)
	{
	case FADE_OUT:
		g_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		break;
	case FADE_OUT_HALF:
		g_color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	}
	g_eFade = fade;
}

//=============================================================================
// フェードの遷移設定
//=============================================================================
void SetFadeScene(SceneManager::SCENE eScene)
{
	g_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	g_eFade = FADE_OUT;
	g_eNextScene = eScene;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
FADE GetFade(void)
{
	return g_eFade;
}

//=============================================================================
//
// 入力処理 [sound.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "sound.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

// おまじない
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void		SetSound(void);
long		GetVol(LPDIRECTSOUNDBUFFER8 pBuffer);
void		SetBgmVol(int no, int type);
void		UpdateSoundBgm(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECTSOUNDBUFFER8	g_pBgm[SOUND_BGM_MAX];
LPDIRECTSOUNDBUFFER8	g_pSe[SOUND_SE_MAX];
LPDIRECTSOUNDBUFFER8	g_pVoice[SOUND_VOICE_MAX];
int						g_nBgmFlag;
bool					g_bBgmBoss;
LONG					g_nVol[SOUND_BGM_MAX];


LPDWORD pdwCurrentPlayCursorBGM[SOUND_BGM_MAX];
LPDWORD pdwCurrentWriteCursorBGM[SOUND_BGM_MAX];

LPDWORD pdwCurrentPlayCursorSE[SOUND_SE_MAX];
LPDWORD pdwCurrentWriteCursorSE[SOUND_SE_MAX];

// サウンドファイルのパス（sound.hの通しナンバーと順番を合わせること）
const TCHAR* c_soundFilename[] = {
	// BGM
	_T("data/SOUND/BGM/WhiteLily_WithoutV.wav"),
	_T("data/SOUND/BGM/WhiteLily_Violin.wav"),
	_T("data/SOUND/BGM/ED.wav"),
	_T("data/SOUND/BGM/select.wav"),
	_T("data/SOUND/BGM/result_BGM.wav"),
	// SE
	_T("data/SOUND/SE/decision26.wav"),
	_T("data/SOUND/SE/decision15.wav"),
	_T("data/SOUND/SE/punch-swing1.wav"),
	_T("data/SOUND/SE/bomb2.wav"),
	_T("data/SOUND/SE/nyu3.wav"),
	_T("data/SOUND/SE/robot-footstep2.wav"),
	_T("data/SOUND/SE/ホイッスル・単発.wav"),
	_T("data/SOUND/SE/ホイッスル・単発02.wav"),
	_T("data/SOUND/SE/ホイッスル・連続.wav"),
	_T("data/SOUND/SE/fire.wav"),
	_T("data/SOUND/SE/fire1.wav"),
	_T("data/SOUND/SE/water_suck.wav"),
	_T("data/SOUND/SE/water1.wav"),
	_T("data/SOUND/SE/water2.wav"),
	_T("data/SOUND/SE/wind_suck.wav"),
	_T("data/SOUND/SE/wind1.wav"),
	_T("data/SOUND/SE/darararara_daaan.wav"),
	_T("data/SOUND/SE/shizen_tinka.wav"),
	_T("data/SOUND/SE/mizu_tinka.wav"),
	_T("data/SOUND/SE/geme_start.wav"),
	_T("data/SOUND/SE/geme_select.wav"),
	_T("data/SOUND/SE/game_kettei.wav"),
	_T("data/SOUND/SE/result_select.wav"),
	_T("data/SOUND/SE/result_kettei.wav"),
	_T("data/SOUND/SE/select_bubu.wav"),
	// VOICE
	_T("data/SOUND/VOICE/start_voice.wav"),
	_T("data/SOUND/VOICE/1_voice.wav"),
	_T("data/SOUND/VOICE/2_voice.wav"),
	_T("data/SOUND/VOICE/3_voice.wav")
};

// グローバル変数
IDirectSound8 *g_pDirectSound = NULL;	// サウンドインターフェース

// サウンドの初期化
// hWnd:ウィンドウハンドル
HRESULT InitSound( HWND hWnd )
{
	// DirectSoundオブジェクトの作成
	if (FAILED(DirectSoundCreate8(NULL, &g_pDirectSound, NULL)))
		return E_FAIL;
	
	// 協調レベル設定 
	if (FAILED(g_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return E_FAIL;
	g_nBgmFlag = SOUND_BGM_TITLE;
	g_bBgmBoss = false;

	for (int i = 0; i < SOUND_BGM_MAX; i++)
	{
		g_nVol[i] = VOLUME_MIN;
	}

	SetSound();
	return S_OK;
}

void UpdateSound(void)
{
	UpdateSoundBgm();
#ifdef _DEBUG
	PrintDebugProc("【 SOUND 】  BGM FLAG[%d]\n", g_nBgmFlag);
	for (int i = 0; i < SOUND_BGM_MAX; i++)
	{
		pdwCurrentPlayCursorBGM[i] = NULL;
		pdwCurrentWriteCursorBGM[i] = NULL;
		g_pBgm[i]->GetCurrentPosition(pdwCurrentPlayCursorBGM[i], pdwCurrentWriteCursorBGM[i]);
		PrintDebugProc("BGM00%d[%l]  CurrentPos[%d] Write[%d]\n", i, GetVol(g_pBgm[i]), pdwCurrentPlayCursorBGM[i], pdwCurrentWriteCursorBGM[i]);
	}

	for (int i = 0; i < SOUND_SE_MAX; i++)
	{
		pdwCurrentPlayCursorSE[i] = NULL;
		pdwCurrentWriteCursorSE[i] = NULL;
		g_pSe[i]->GetCurrentPosition(pdwCurrentPlayCursorSE[i], pdwCurrentWriteCursorSE[i]);

		PrintDebugProc(" SE00%d[%l]  CurrentPos[%d] Write[%d]\n", i, GetVol(g_pSe[i]), pdwCurrentPlayCursorSE[i], pdwCurrentWriteCursorSE[i]);
	}

	PrintDebugProc("\n");
#endif
}

// 後片付け
void UninitSound()
{
	if( g_pDirectSound != NULL )
	{
		g_pDirectSound->Release();
		g_pDirectSound = NULL;
	}
}

// サウンドのロード
// no:サウンドナンバー（ヘッダに定義された列挙型定数）
LPDIRECTSOUNDBUFFER8 LoadSound( int no )
{
	// MMIO = マルチメディア入出力、の略。
	LPDIRECTSOUNDBUFFER  pBaseBuffer = NULL;// 曲データの総合バッファ
	LPDIRECTSOUNDBUFFER8 pBuffer = NULL;	// 曲データのバッファ
	DSBUFFERDESC buff;						// バッファ設定構造体

	HMMIO hMmio = NULL;						// MMIOハンドル
	MMIOINFO mmioInfo;						// マルチメディアデータ構造体

	MMRESULT mmRes;								// MM処理結果格納変数
	MMCKINFO riffChunk, formatChunk, dataChunk;	// チャンクデータ構造体
	DWORD size;									// データサイズ格納用
	WAVEFORMATEX pcm;							// 曲データフォーマット構造体

	LPVOID pBlock;				// セカンダリバッファのデータ書き込み先アドレス
	DWORD  dwSize;				// セカンダリバッファのサイズ

	// 1.ハンドルをもとにファイルを開く
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)c_soundFilename[no], &mmioInfo, MMIO_READ);
	if(!hMmio)
		return NULL;

	// 2.ファイル解析① RIFFチャンク検索
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// 検索キーワード
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR)		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.ファイル解析② フォーマットチャンク検索
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// 検索キーワード
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR)		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// 検索情報をもとに読み込み
	
	if(size != formatChunk.cksize) 		// 正しく読み込めなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0); 

	// 4.ファイル解析③ データチャンク検索
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// 検索キーワード
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	
	if(mmRes != MMSYSERR_NOERROR) 		// 見つからなかったら異常終了
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.データ読み込み
	char *pData = new char[dataChunk.cksize];					// 必要な大きさの領域を確保して
	size = mmioRead(hMmio, (HPSTR)pData, dataChunk.cksize);		// データを読み込む
	
	if(size != dataChunk.cksize)  		// 正しく読み込めなかったら異常終了
	{
		delete[] pData;
		return NULL;
	}

	// 6.曲を読み込む「セカンダリバッファ」を用意
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// まず初期化
	buff.dwSize = sizeof(DSBUFFERDESC);			// そこから各種設定
	buff.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;

	// 総合バッファを作る
	if (FAILED(g_pDirectSound->CreateSoundBuffer(&buff, &pBaseBuffer, NULL)))
		return NULL;

	// サウンドバッファを取り出す
	if (FAILED(pBaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pBuffer)))
		return NULL;

	// 取り出したら総合バッファを捨てる
	pBaseBuffer->Release();

	// 7.作られたセカンダリバッファにデータを転送
	// まずは書き込みできるようバッファをロック
	if (FAILED(pBuffer->Lock(0, 0, &pBlock, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
		return NULL;

	memcpy(pBlock, pData, dwSize);	// データ転送
	delete[] pData;					// 元の読み込み領域を消す

	// バッファロック解除
	pBuffer->Unlock(pBlock, dwSize, 0, 0);

	// セカンダリバッファを返してようやく完了...
	return pBuffer;
}

void UpdateSoundBgm(void)
{
	switch (g_nBgmFlag)
	{
	case SOUND_BGM_NON:
		SetBgmVol(BGM_WHITELILY_V_NON, false);
		SetBgmVol(BGM_WHITELILY_V, false);
		break;
	case SOUND_BGM_TITLE:

		break;
	case SOUND_BGM_SELECT:

		break;
	case SOUND_BGM_GAME:
		break;
	case SOUND_BGM_RESULT:

		break;
	case SOUND_BGM_TEST1:
		SetBgmVol(BGM_WHITELILY_V_NON, true);
		SetBgmVol(BGM_WHITELILY_V, false);
		break;
	case SOUND_BGM_TEST2:
		SetBgmVol(BGM_WHITELILY_V_NON, true);
		SetBgmVol(BGM_WHITELILY_V, true);
		break;
	}
}

void SetSoundBgm(int nBgm)
{
	g_nBgmFlag = nBgm;
}

void SetSoundBgmBoss(bool bBgm)
{
	g_bBgmBoss = bBgm;
}

// 音を鳴らす
// pBuffer:音を鳴らしたいサウンドデータのセカンダリバッファ
// flag   :1(E_DS8_FLAG_LOOP)ならループ再生
void PlaySound(  LPDIRECTSOUNDBUFFER8 pBuffer, int flag/*=0*/ ,int nOption, DWORD dwPos)
{	// 続きから鳴らすので、最初から鳴らしたい場合はSetCurrentPosition(0)をすること
	if (pBuffer != NULL)
	{
		// 連続再生オフ、または途中から再生する場合
		if (nOption == SOUND_OPTION_CONTINUE_OFF || nOption == SOUND_OPTION_CURRENT)
		{
			DWORD status;
			pBuffer->GetStatus(&status);
			// 再生しているかを確認
			if (status & DSBSTATUS_PLAYING)
			{	// 再生している場合は関数終了
				return;
			}
			if (nOption == SOUND_OPTION_CURRENT)
			{
				pBuffer->SetCurrentPosition(dwPos);
			}
		}
		else if (nOption == SOUND_OPTION_CONTINUE_ON)
		{
				pBuffer->SetCurrentPosition(dwPos);
		}
		pBuffer->Play(0, 0, flag);
	}
}

// 音を止める
void StopSound( LPDIRECTSOUNDBUFFER8 pBuffer )
{
	if (pBuffer != NULL)
	{
		DWORD status;
		pBuffer->GetStatus(&status);
		if (status & DSBSTATUS_PLAYING)	// 鳴っていたら
		{
			pBuffer->Stop();	// 意味的にはPabUseになる。
		}
	}
}

// 再生中かどうか調べる
// pBuffer:音を鳴らしたいサウンドデータのセカンダリバッファ
bool IsPlaying( LPDIRECTSOUNDBUFFER8 pBuffer )
{
	DWORD status;

	pBuffer->GetStatus(&status);
	if( status & DSBSTATUS_PLAYING )
	{
		return true;
	}
	return false;
}

HRESULT SetVol( LPDIRECTSOUNDBUFFER8 pBuffer, LONG lVol)
{
	DWORD status;
	pBuffer->SetVolume(lVol);
	return DS_OK;
}

long GetVol(LPDIRECTSOUNDBUFFER8 pBuffer)
{
	DWORD status;
	long lVol;
	pBuffer->GetStatus(&status);
	pBuffer->GetVolume(&lVol);
	return lVol;
}

//=============================================================================
// サウンド設定関数
//=============================================================================
void SetSound(void)
{
	// BGMがずれないようにロードと分ける
	for (int i = BGM_00; i < SOUND_BGM_MAX; i++)
	{
		// 各BGMロード
		g_pBgm[i] = LoadSound(i);
	}
	for (int i = BGM_00; i < SOUND_BGM_MAX; i++)
	{
		// 各BGMボリュームセット
		PlaySound(g_pBgm[i], E_DS8_FLAG_LOOP, SOUND_OPTION_BGM, 0);
		SetVol(g_pBgm[i], VOLUME_MIN);
	}

	for (int i = 0; i < SOUND_SE_MAX; i++)
	{	
		// 各SEロード
		g_pSe[i] = LoadSound(i + SE_00);
		//// 各SEボリュームセット
		//if (i != SE_DRAMROLL)
		//{
		//	SetVol(g_pSe[i], (LONG)-5000);
		//}
	}

	//SetVol(g_pSe[11], SE_11_VOLUME_SE);

	for (int i = 0; i < SOUND_VOICE_MAX; i++)
	{
		// 各SEロード
		g_pVoice[i] = LoadSound(i + VOICE_00);
	}
}

//=============================================================================
// SE再生関数
//=============================================================================
void SetSe(int nSe, int nFlag, int nOption, DWORD dwPos)
{
	PlaySound(g_pSe[nSe], nFlag, nOption, dwPos);
}

//=============================================================================
// SE停止関数
//=============================================================================
void EndSe(int nSe)
{
	StopSound(g_pSe[nSe]);
}

//=============================================================================
// Voice再生関数
//=============================================================================
void SetVoice(int nVoice, int nFlag, int nOption, DWORD dwPos)
{
		PlaySound(g_pVoice[nVoice], nFlag, nOption, dwPos);
}

//=============================================================================
// BGMボリュームコントロール
//=============================================================================
void SetBgmVol(int no, int type)
{
	// type 0=フェードアウト 1=フェードイン
	if (type == VOLUME_FADE_OUT)
	{
		if (g_nVol[no] > VOLUME_MIN)
		{
			g_nVol[no] -= VOLUME_CONTROL_DOWN;
			if (g_nVol[no] < VOLUME_MIN)
			{
				g_nVol[no] = VOLUME_MIN;
			}
			SetVol(g_pBgm[no], g_nVol[no]);
		}
	}
	else if (type == VOLUME_FADE_IN)
	{
		if (g_nVol[no] < VOLUME_MAX)
		{
			g_nVol[no] += VOLUME_CONTROL_UP;
			if (g_nVol[no] > VOLUME_MAX)
			{
				g_nVol[no] = VOLUME_MAX;
			}
			SetVol(g_pBgm[no], g_nVol[no]);
		}
	}
	else if (type == VOLUME_FADE_HALF)
	{
		if (g_nVol[no] > VOLUME_HALF)
		{
			g_nVol[no] -= VOLUME_CONTROL_DOWN;
			if (g_nVol[no] < VOLUME_HALF)
			{
				g_nVol[no] = VOLUME_HALF;
			}
			SetVol(g_pBgm[no], g_nVol[no]);
		}
	}
}
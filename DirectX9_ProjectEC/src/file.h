//=============================================================================
//
// ファイル処理 [file.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _FILE_H_
#define _FILE_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class File
{
public:
	// コンストラクタ（初期化処理）
	File(void);
	//デストラクタ（終了処理）
	~File(void);

	template <typename Data>
	static bool Load(Data* pData, const char* pFilePath, int nSize)
	{
		//バイナリファイルを開く
		FILE *fp = fopen(pFilePath, "rb");

		if (fp == NULL)
		{//エラーが起きたらNULLを返す
			return false;
		}

		// 読み込み処理
		fread(pData, sizeof(Data) * nSize, 1, fp);

		fclose(fp);
		return true;
	}

	template <typename Data>
	static bool Save(Data* pData, const char* pFilePath, int nSize)
	{
		//バイナリファイルを開く
		FILE *fp = fopen(pFilePath, "wb");

		if (fp == NULL)
		{//エラーが起きたらNULLを返す
			return false;
		}

		// 書き込み処理
		fwrite(pData, sizeof(Data) * nSize, 1, fp);

		fclose(fp);
		return true;
	}
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif

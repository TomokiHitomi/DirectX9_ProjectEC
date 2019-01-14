//=============================================================================
//
// 計算処理 [calculate.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _CALCULATE_H_
#define _CALCULATE_H_

/*******************************************************************************
* インクルード
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ZERO_D3DXVECTOR3		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
#define UP_D3DXVECTOR3			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))

//*****************************************************************************
// 列挙型定義
//*****************************************************************************
enum
{
	COLOR_PALLET_WHITE,
	COLOR_PALLET_RED,
	COLOR_PALLET_YELLOW,
	COLOR_PALLET_GREEN,
	COLOR_PALLET_CYAN,
	COLOR_PALLET_BLUE,
	COLOR_PALLET_MAGENTA,
	COLOR_PALLET_BROWN,
	COLOR_PALLET_LIGHTGREEN,
	COLOR_PALLET_ORANGE,
	COLOR_PALLET_BLACK,
	COLOR_PALLET_LIGHTBLUE,
	COLOR_PALLET_GRAY,
	COLOR_PALLET_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
template <typename Type>
bool LimitAdjust(int nType, Type* pValue, Type tAdjust, Type tLimit)
{
	bool bFlag = true;
	if (nType == 0)
	{
		if (*pValue < tLimit)
		{
			*pValue += tAdjust;
			if (*pValue > tLimit)
			{
				*pValue = tLimit;
				bFlag = false;
			}
		}
	}
	else if (nType == 1)
	{
		if (*pValue > tLimit)
		{
			*pValue -= tAdjust;
			if (*pValue < tLimit)
			{
				*pValue = tLimit;
				bFlag = false;
			}
		}
	}
	return bFlag;
}

D3DXVECTOR3 RandVector(void);
void WorldConvert(D3DXMATRIX *pOut, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl);
void WorldConvertXYZ(D3DXMATRIX *world, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl);
void WorldConvertAxis(D3DXMATRIX *world, D3DXVECTOR3 pos,
	D3DXVECTOR3 vLook, D3DXVECTOR3 vUp, D3DXVECTOR3 scl);
void MatrixRotationVecAndUp(D3DXMATRIX *mtxOut, D3DXVECTOR3 vLook, D3DXVECTOR3 vUp);
float CompHigh(float f1, float f2);
float DotProduct(D3DXVECTOR3 *vl, D3DXVECTOR3 *vr);
D3DXVECTOR3 *CrossProduct(D3DXVECTOR3 *ret, D3DXVECTOR3 *vl, D3DXVECTOR3 *vr);
float SetLimit(float fTag, float fMax, float fMin);
float PiCalculate180(float fAngle);
float PiCalculate360(float fAngle);
void QuaternionCalculate(D3DXVECTOR3 *pInit, D3DXVECTOR3 *pC, float Rad, D3DXVECTOR3 *ret);
D3DXCOLOR SetColorPallet(int nColor);
D3DXCOLOR SetColorPalletRandom(void);
void SetShuffle(int *nShuffle, int nSize);
D3DXVECTOR3 Calculate_Vec_2pos(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
float Calculate_Atan2XZ(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);
bool D3DXMatrixNormalize(D3DXMATRIX *pOut, D3DXMATRIX *pIn);
D3DXCOLOR SetColorPalletRandomGreen(void);
D3DXCOLOR SetColorPalletGreen(int nColor);
float saturate(const float& V);
#endif

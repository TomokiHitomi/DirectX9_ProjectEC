//=============================================================================
//
// 計算処理 [calculate.cpp]
// Author : GP12A295 25 人見友基
//
// 計算処理全般を行うcpp
//
//=============================================================================
#include "calculate.h"
#include <cmath>
#include <iostream>

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//=============================================================================
// ランダムベクトル生成処理
//=============================================================================
D3DXVECTOR3 RandVector(void)
{
	D3DXVECTOR3		vecTemp;
	float			fAngleX, fAngleY;

	fAngleX = float(D3DXToRadian(rand() % 180 + 1));
	fAngleY = float(D3DXToRadian(rand() % 360 + 1));

	vecTemp.x = sinf(fAngleX) * cosf(fAngleY);
	vecTemp.y = sinf(fAngleX) * sinf(fAngleY);
	vecTemp.z = cosf(fAngleX);
	return(vecTemp);
}

//=============================================================================
// ワールド変換
//=============================================================================
void WorldConvert(D3DXMATRIX *world, D3DXVECTOR3 pos,
	D3DXVECTOR3 rot, D3DXVECTOR3 scl)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	/******************** ワールド変換 ********************/
	// ワールドマトリクスの初期化
	D3DXMatrixIdentity(world);

	// 【S】スケールを反映(Multiplyは行列計算)
	D3DXMatrixScaling(&mtxScl, scl.x, scl.y, scl.z);
	D3DXMatrixMultiply(world, world, &mtxScl);

	// 【R】回転を反映(YawPitchRollはy,x,z)
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(world, world, &mtxRot);

	// 【T】平行移動を反映(オブジェクトを配置している）
	D3DXMatrixTranslation(&mtxTranslate, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(world, world, &mtxTranslate);
}

//=============================================================================
// ワールド変換
//=============================================================================
void WorldConvertXYZ(D3DXMATRIX *world, D3DXVECTOR3 pos,
	D3DXVECTOR3 rot, D3DXVECTOR3 scl)
{
	D3DXMATRIX mtxScl, mtxRotX, mtxRotY, mtxRotZ, mtxTranslate;

	/******************** ワールド変換 ********************/
	// ワールドマトリクスの初期化
	D3DXMatrixIdentity(world);

	// 【S】スケールを反映(Multiplyは行列計算)
	D3DXMatrixScaling(&mtxScl, scl.x, scl.y, scl.z);
	D3DXMatrixMultiply(world, world, &mtxScl);

	// 【R】回転を反映(XYZの順番で行列計算)
	D3DXMatrixRotationX(&mtxRotX, rot.x);
	D3DXMatrixRotationY(&mtxRotY, rot.y);
	D3DXMatrixRotationZ(&mtxRotZ, rot.z);

	D3DXMatrixMultiply(world, world, &mtxRotX);
	D3DXMatrixMultiply(world, world, &mtxRotY);
	D3DXMatrixMultiply(world, world, &mtxRotZ);

	// 【T】平行移動を反映(オブジェクトを配置している）
	D3DXMatrixTranslation(&mtxTranslate, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(world, world, &mtxTranslate);
}

//=============================================================================
// ワールド変換（MatrixRotationVecAndUp仕様）
//=============================================================================
void WorldConvertAxis(D3DXMATRIX *world, D3DXVECTOR3 pos,
	D3DXVECTOR3 vLook, D3DXVECTOR3 vUp, D3DXVECTOR3 scl)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	/******************** ワールド変換 ********************/
	// ワールドマトリクスの初期化
	D3DXMatrixIdentity(world);

	// 【S】スケールを反映(Multiplyは行列計算)
	D3DXMatrixScaling(&mtxScl, scl.x, scl.y, scl.z);
	D3DXMatrixMultiply(world, world, &mtxScl);

	// 【R】回転を反映(YawPitchRollはy,x,z)
	MatrixRotationVecAndUp(&mtxRot, vLook, vUp);
	D3DXMatrixMultiply(world, world, &mtxRot);

	// 【T】平行移動を反映(オブジェクトを配置している）
	D3DXMatrixTranslation(&mtxTranslate, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(world, world, &mtxTranslate);
}

//=============================================================================
// 回転行列計算
// vLookを向き、vUpを上とする回転行列をmtxOutにアウトプットする
//=============================================================================
void MatrixRotationVecAndUp(D3DXMATRIX *mtxOut, D3DXVECTOR3 vLook, D3DXVECTOR3 vUp)
{
	D3DXVECTOR3 X, Y, Z, Up;

	D3DXVec3Normalize(&Z, &vLook);
	D3DXVec3Cross(&X, D3DXVec3Normalize(&Y, &vUp), &Z);
	D3DXVec3Normalize(&X, &X);
	D3DXVec3Normalize(&Y, D3DXVec3Cross(&Y, &Z, &X));

	mtxOut->_11 = X.x;
	mtxOut->_12 = X.y;
	mtxOut->_13 = X.z;
	mtxOut->_14 = 0.0f;

	mtxOut->_21 = Y.x;
	mtxOut->_22 = Y.y;
	mtxOut->_23 = Y.z;
	mtxOut->_24 = 0.0f;

	mtxOut->_31 = Z.x;
	mtxOut->_32 = Z.y;
	mtxOut->_33 = Z.z;
	mtxOut->_34 = 0.0f;

	mtxOut->_41 = 0.0f;
	mtxOut->_42 = 0.0f;
	mtxOut->_43 = 0.0f;
	mtxOut->_44 = 1.0f;
}

//=============================================================================
// 行列のXYZ成分の正規化（ポジションは正規化、出力しない）
//=============================================================================
bool D3DXMatrixNormalize(D3DXMATRIX *pOut, D3DXMATRIX *pIn)
{
	D3DXVECTOR3 x, y, z;

	// マトリクス内のXYZ成分を抽出
	x = D3DXVECTOR3(pIn->_11, pIn->_12, pIn->_13);
	y = D3DXVECTOR3(pIn->_21, pIn->_22, pIn->_23);
	z = D3DXVECTOR3(pIn->_31, pIn->_32, pIn->_33);

	// 各ベクトルの正規化
	D3DXVec3Normalize(&x, &x);
	D3DXVec3Normalize(&y, &y);
	D3DXVec3Normalize(&z, &z);

	// 正規化したベクトルを行列に格納
	pOut->_11 = x.x;
	pOut->_12 = x.y;
	pOut->_13 = x.z;
	pOut->_21 = y.x;
	pOut->_22 = y.y;
	pOut->_23 = y.z;
	pOut->_31 = z.x;
	pOut->_32 = z.y;
	pOut->_33 = z.z;

	return true;
}

//=============================================================================
// 絶対値が大きい数値を返す
//=============================================================================
float CompHigh(float f1, float f2)
{
	float f1t, f2t;
	f1t = std::abs(f1);
	f2t = std::abs(f2);
	if (f1t > 1.0f || f2t > 1.0f)
	{
		return 1.0f;
	}
	if (f1t > f2t)
	{
		return f1t;
	}
	else
	{
		return f2t;
	}
}

//=============================================================================
// 内積
//=============================================================================
float DotProduct(D3DXVECTOR3 *vl, D3DXVECTOR3 *vr)
{
	float ans = 0;
	//ans = (vl->x * vr->x) + (vl->y * vr->y) + (vl->z * vr->z);
	ans = D3DXVec3Dot(vl, vr);
	return(ans);
}


//=============================================================================
// 外積
//=============================================================================
D3DXVECTOR3 *CrossProduct(D3DXVECTOR3 *ret, D3DXVECTOR3 *vl, D3DXVECTOR3 *vr)
{
	//ret->x = (vl->y * vr->z) - (vl->z * vr->y);
	//ret->y = (vl->z * vr->x) - (vl->x * vr->z);
	//ret->z = (vl->x * vr->y) - (vl->y * vr->x);
	D3DXVec3Cross(ret, vl, vr);
	return(ret);
}

//=============================================================================
// 数値限界設定
//=============================================================================
bool SetLimit(float* pOut, float fTag, float fMax, float fMin)
{
	if (fTag > fMax)
	{
		*pOut = fMax;
		return true;
	}
	else if (fMin > fTag)
	{ 
		*pOut = fMin;
		return true;
	}
	else
	{ 
		*pOut = fTag;
		return false;
	}
}
//=============================================================================
// PI調整処理（180度）
//=============================================================================
float PiCalculate180(float fAngle)
{
	// 180度を超えたら360度を引く
	if (fAngle >= D3DX_PI)
	{
		fAngle -= D3DX_PI * 2.0f;
	}
	else if (fAngle < -D3DX_PI)
	{
		fAngle += D3DX_PI * 2.0f;
	}
	return fAngle;
}

//=============================================================================
// PI調整処理（360度）
//=============================================================================
float PiCalculate360(float fAngle)
{
	// 360度を超えたら360度を引く
	if (fAngle > D3DX_PI * 2)
	{
		fAngle -= D3DX_PI * 2.0f;
	}
	else if (fAngle < 0.0f)
	{
		fAngle += D3DX_PI * 2.0f;
	}
	return fAngle;
}

//=============================================================================
// クォータニオン処理
//=============================================================================
void QuaternionCalculate(D3DXVECTOR3 *pOrigin, D3DXVECTOR3 *pAxis, float fAngle, D3DXVECTOR3 *pPos)
{
	// 反数、処理結果用の変数を定義
	D3DXQUATERNION qReversal, qAnswer;
	// 回転させたい点の初期位置
	D3DXQUATERNION qOrigin(pOrigin->x, pOrigin->y, pOrigin->z, 0.0f);
	// 単位クォータニオン
	D3DXQUATERNION qQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

	// 軸pAxis回りの回転クォータニオンを生成
	D3DXQuaternionRotationAxis(&qQuaternion, pAxis, fAngle);
	// XYZマイナスの共役を算出
	D3DXQuaternionConjugate(&qReversal, &qQuaternion);
	// qReversal・qOrigin・qQuaternionを行列計算
	D3DXQuaternionMultiply(&qAnswer, &qReversal, &qOrigin);
	// qAnswerに回転処理後の結果を格納
	D3DXQuaternionMultiply(&qAnswer, &qAnswer, &qQuaternion);

	// 回転処理の結果を反映
	pPos->x = qAnswer.x;
	pPos->y = qAnswer.y;
	pPos->z = qAnswer.z;
}

//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPallet(int nColor)
{
	D3DXCOLOR temp;
	//temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	switch (nColor)
	{
	case COLOR_PALLET_WHITE:		// ホワイト
		temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_RED:			// レッド
		temp = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_YELLOW:		// イエロー
		temp = D3DXCOLOR(0.9f, 0.9f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_GREEN:		// グリーン
		temp = D3DXCOLOR(0.0f, 1.0f, 0.05f, 1.0f);
		break;
	case COLOR_PALLET_CYAN:			// シアン
		temp = D3DXCOLOR(0.0f, 0.9f, 0.9f, 1.0f);
		break;
	case COLOR_PALLET_BLUE:			// ブルー
		temp = D3DXCOLOR(0.1f, 0.1f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_MAGENTA:		// マゼンダ
		temp = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_BROWN:		// ブラウン
		temp = D3DXCOLOR(0.6f, 0.4f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTGREEN:	// ライトグリーン
		temp = D3DXCOLOR(0.3f, 0.9f, 0.3f, 1.0f);
		break;
	case COLOR_PALLET_ORANGE:		// オレンジ
		temp = D3DXCOLOR(1.0f, 0.3f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_BLACK:		// ブラック
		temp = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTBLUE:	// ブルー
		temp = D3DXCOLOR(0.4f, 0.7f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_GRAY:			// グレイ
		temp = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		break;
	}
	return temp;
}

//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPalletRandom(void)
{
	return SetColorPallet(rand() % COLOR_PALLET_MAX);
}

//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPalletGreen(int nColor)
{
	D3DXCOLOR temp;
	switch (nColor)
	{
	case 0:
		temp = D3DXCOLOR(0.0f, 0.971f, 0.443f, 1.0f);
		break;
	case 1:
		temp = D3DXCOLOR(0.0f, 0.435f, 0.321f, 1.0f);
		break;
	case 2:
		temp = D3DXCOLOR(0.0f, 0.956f, 0.431f, 1.0f);
		break;
	case 3:
		temp = D3DXCOLOR(0.0f, 0.886f, 0.362f, 1.0f);
		break;
	case 4:
		temp = D3DXCOLOR(0.0f, 0.364f, 0.258f, 1.0f);
		break;
	case 5:
		temp = D3DXCOLOR(0.0f, 0.275f, 0.202f, 1.0f);
		break;
	case 6:
		temp = D3DXCOLOR(0.0f, 0.800f, 0.364f, 1.0f);
		break;
	case 7:
		temp = D3DXCOLOR(0.0f, 0.415f, 0.247f, 1.0f);
		break;
	case 8:
		temp = D3DXCOLOR(0.0f, 0.415f, 0.294f, 1.0f);
		break;
	case 9:
		temp = D3DXCOLOR(0.0f, 0.850f, 0.278f, 1.0f);
		break;
	}

	return temp;
}


//=============================================================================
// カラーパレット呼び出し処理
//=============================================================================
D3DXCOLOR SetColorPalletRandomGreen(void)
{
	return SetColorPalletGreen(rand() % 10);
}


//=============================================================================
// int配列のシャッフル関数
//=============================================================================
void SetShuffle(int *nShuffle, int nSize)
{
	for (int i = 0; i < nSize; i++)
	{
		int j = rand() % nSize;
		int t = nShuffle[i];
		nShuffle[i] = nShuffle[j];
		nShuffle[j] = t;
	}
}

/*******************************************************************************
* 関数名：D3DXVECTOR3 Calculate_Vec_2pos(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
* 引数  ：二つの位置
* 戻り値：float型の長さ
* 説明  ：pos2からpos1のベクトルを求める処理
*******************************************************************************/
D3DXVECTOR3 Calculate_Vec_2pos(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	// ベクトルを返す
	return(pos1 - pos2);
}

/*******************************************************************************
* 関数名：float Calculate_Atan2XZ(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
* 引数  ：二つの位置
* 戻り値：float型の角度
* 説明  ：pos1みたpos2への角度（XZ軸）
*******************************************************************************/
float Calculate_Atan2XZ(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	D3DXVECTOR3 vecTemp;
	vecTemp.x = pos2.x - pos1.x;
	vecTemp.z = pos2.z - pos1.z;

	// atan2のエラー防止
	if (vecTemp.x == 0 && vecTemp.z == 0)
	{
		vecTemp.x += 0.00001;
	}

	// 角度計算
	return atan2(vecTemp.z, vecTemp.x);
}

//=============================================================================
// 浮動小数点を 0.0 ～ 1.0 の範囲でクランプ
//=============================================================================
float saturate(const float& V)
{
	long l = *(long*)&V;
	l &= ((long)(l ^ 0x80000000) >> 31);
	l -= 0x3F800000;
	l &= l >> 31;
	l += 0x3F800000;
	return *(float*)&l;
}

//#ifdef _DEBUG
//PrintDebugProc("mtxX[%f,%f,%f]\n",
//	m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13);
//PrintDebugProc("mtxY[%f,%f,%f]\n",
//	m_mtxWorld._21, m_mtxWorld._22, m_mtxWorld._23);
//PrintDebugProc("mtxZ[%f,%f,%f]\n",
//	m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33);
//PrintDebugProc("mtxA[%f,%f,%f]\n",
//	m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
//#endif
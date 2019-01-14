//=============================================================================
//
// 判定処理 [collision.cpp]
// Author : GP12A295 25 人見友基
//
// 当たり判定を行うcpp
//
//=============================================================================
#include "main.h"
#include "collision.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//=============================================================================
// 当たり判定関数
//=============================================================================
void ChackHit(void)
{

}

//=============================================================================
// BCの判定関数
//=============================================================================
bool CheckHitBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	D3DXVECTOR3 temp = pos1 - pos2;
	float fTempLengthSq = D3DXVec3LengthSq(&temp);

	// 当たり判定の確認(BC)
	if (fTempLengthSq <= (size1 + size2)*(size1 + size2))
	{
		// 当たり判定
		return true;
	}
	return false;
}

//=============================================================================
// レイとスフィアの当たり判定
//=============================================================================
bool CheckHitRayToSphere(D3DXVECTOR3 posRay, D3DXVECTOR3 vecRay, D3DXVECTOR3 posTag, float nLength)
{
	posTag.x -= posRay.x;
	posTag.y -= posRay.y;
	posTag.z -= posRay.z;

	float fA = vecRay.x * vecRay.x + vecRay.y * vecRay.y + vecRay.z * vecRay.z;
	float fB = vecRay.x * posTag.x + vecRay.y * posTag.y + vecRay.z * posTag.z;
	float fC = posTag.x * posTag.x + posTag.y * posTag.y + posTag.z * posTag.z - nLength * nLength;

	if (fA == 0.0f)
	{	// レイの長さが0
		return false;
	}

	float fS = fB * fB - fA * fC;
	if (fS < 0.0f)
	{	// 衝突なし
		return false;
	}

	fS = sqrtf(fS);
	float fA1 = (fB - fS) / fA;
	float fA2 = (fB + fS) / fA;

	if (fA1 < 0.0f || fA2 < 0.0f)
	{	// レイの反対で衝突
		return false;
	}

	return true;
}

//=============================================================================
// レイとメッシュの当たり判定
//=============================================================================
float CheckHitRayToMesh(D3DXVECTOR3 posRay, D3DXVECTOR3 vecRay, LPD3DXBASEMESH lpMesh)
{
	BOOL bHit = false;
	float fDist = 0.0f;

	D3DXIntersect(
		lpMesh,		// ベースメッシュ
		&posRay,	// レイの始点
		&vecRay,	// レイのベクトル
		&bHit,		// 当たり判定[out]
		NULL,		// レイに最も近い面のインデックス値[out]
		NULL,		// ヒット座標U[out]
		NULL,		// ヒット座標V[out]
		&fDist,		// レイの始点から交点まので距離[out]
		NULL,		// ヒットしたすべての面情報[out]
		NULL);		// ヒット回数[out]

	return fDist;
}
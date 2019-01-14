//=============================================================================
//
// 当たり判定処理 [collision.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void ChackHit(void);
bool CheckHitBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2);
bool CheckHitRayToSphere(D3DXVECTOR3 posRay, D3DXVECTOR3 vecRay, D3DXVECTOR3 posTag, float nLength);
float CheckHitRayToMesh(D3DXVECTOR3 posRay, D3DXVECTOR3 vecRay, LPD3DXBASEMESH lpMesh);

#endif

//=============================================================================
//
// ライト処理 [light.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// シェーダ用ライト
#define LIGHT_DIFFUSE			(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))
#define LIGHT_AMBIENT			(D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f))
#define LIGHT_SPECULAR			(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))

#define LIGHT_POSITION			(D3DXVECTOR3(0.0f,0.0f,0.0f))
#define LIGHT_DIRECTION			(D3DXVECTOR3(0.0f,-0.3f,-0.8f))

// 固定機能用ライト
#define	LIGHT_MAX				(3)						// ライトの数

#define LIGHT_DIFFUSE_0			(1.0f)
#define LIGHT_DIFFUSE_1			(0.5f)
#define LIGHT_DIFFUSE_2			(0.5f)

#define LIGHT_SUB_DIFFUSE		(0.3f)
#define LIGHT_SUB_XZ			(0.8)
#define LIGHT_SUB_Y				(1.0f)

enum
{
	LIGHT_MAIN,
	LIGHT_SUB1,
	LIGHT_SUB2
};

class Light
{
public:
	typedef struct _LIGHTVALUE
	{
		D3DCOLORVALUE   Diffuse;	// 拡散光
		D3DCOLORVALUE   Ambient;	// 環境光
		D3DCOLORVALUE   Specular;	// 反射光
		D3DVECTOR       Position;	// 座標
		D3DVECTOR       Direction;	// 平行光源
	}LIGHTVALUE;
	typedef enum _LIGHTTYPE
	{
		POINT,
		SPOT,
		DIRECTIONAL
	}LIGHTTYPE;

	LIGHTTYPE type;
	LIGHTVALUE value;

	// コンストラクタ
	Light()
	{
		type = DIRECTIONAL;
		ZeroMemory(&value, sizeof(value));
	}
};

class LightManager
{
public:
	enum LightArray
	{
		Main,
		Max
	};
	static Light cLight[Max];

	static HRESULT Init();
	static Light *GetLightAdr(LightArray eLight) { return &cLight[eLight]; }
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitLight(void);
void SetLight(int nLight, bool bFlag);
D3DLIGHT9 GetLight(int nLight);

#endif

//
// Skinned Mesh Effect file 
// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//

//float4 lhtDir = { 0.0f, 0.0f, -1.0f, 1.0f };    //light Direction 
//float4 lightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f }; // Light Diffuse
//float4 MaterialAmbient : MATERIALAMBIENT = { 0.1f, 0.1f, 0.1f, 1.0f };
//float4 MaterialDiffuse : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };

// Matrix Pallette
static const int MAX_MATRICES = 26;
float4x4    mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;
float4x4    mViewProj : VIEWPROJECTION;
float4x4	mView;
float4		eye;		// 視点座標
float4		test = float4(0.0f, 0.0f, 0.0f, 1.0f);


// ライト
typedef struct _LIGHT
{
	float4		dif;	// 拡散光
	float4		amb;	// 環境光
	float4		spc;	// 反射光
	float3		pos;	// 座標
	float3		dir;	// 平行光源
}LIGHT;

// マテリアル
typedef struct _MATERIAL
{	// D3DMATERIAL9と同じ並び
	float4		dif;	// 拡散光
	float4		amb;	// 環境光
	float4		spc;	// 反射光
	float4		emi;	// 自発光
	float		pwr;	// 反射光のパワー値
}MATERIAL;

LIGHT		lt;
MATERIAL	mat;

texture tex;			// 使用するテクスチャ
sampler smp = sampler_state {
	texture = <tex>;
	//MinFilter = LINEAR;
	//MagFilter = LINEAR;
	//MipFilter = NONE;

	//AddressU = Clamp;
	//AddressV = Clamp;
};

float4x4 Scl(float scl)
{
	float4x4 mtx;
	mtx._11 = scl;
	mtx._12 = 0.0f;
	mtx._13 = 0.0f;
	mtx._14 = 0.0f;
	mtx._21 = 0.0f;
	mtx._22 = scl;
	mtx._23 = 0.0f;
	mtx._24 = 0.0f;
	mtx._31 = 0.0f;
	mtx._32 = 0.0f;
	mtx._33 = scl;
	mtx._34 = 0.0f;
	mtx._41 = 0.0f;
	mtx._42 = 0.0f;
	mtx._43 = 0.0f;
	mtx._44 = 1.0f;
	return mtx;
}

///////////////////////////////////////////////////////
struct VS_IN
{
	float4	Pos				: POSITION;
	float4	BlendWeights	: BLENDWEIGHT;
	float4	BlendIndices	: BLENDINDICES;
	float3	Normal			: NORMAL;
	float3	Tex0			: TEXCOORD0;
};

struct VS_OUT
{
	float4	Pos				: POSITION;
	float4	Color			: COLOR;
	float2	Tex0			: TEXCOORD0;
	float3	Normal			: TEXCOORD1;
	float4	Specular		: TEXCOORD2;
};

//
//float3 Diffuse(float3 Normal)
//{
//	float CosTheta;
//
//	// N.L Clamped
//	CosTheta = max(0.0f, dot(Normal, lhtDir.xyz));
//
//	// propogate scalar result to vector
//	return (CosTheta);
//}


VS_OUT VSOutline(VS_IN In, uniform int NumBones)
{
	VS_OUT	Out = (VS_OUT)0;
	float3      Pos = 0.0f;
	float       LastWeight = 0.0f;

	// Compensate for lack of UBYTE4 on Geforce3
	int4 IndexVector = D3DCOLORtoUBYTE4(In.BlendIndices);

	// cast the vectors to arrays for use in the for loop below
	float BlendWeightsArray[4] = (float[4])In.BlendWeights;
	int   IndexArray[4] = (int[4])IndexVector;

	// calculate the pos/normal using the "normal" weights 
	//        and accumulate the weights to calculate the last weight
	for (int iBone = 0; iBone < NumBones - 1; iBone++)
	{
		LastWeight = LastWeight + BlendWeightsArray[iBone];

		Pos += mul(In.Pos , mul(mWorldMatrixArray[IndexArray[iBone]], mView)) * BlendWeightsArray[iBone];
		Out.Normal += mul(In.Normal, (float3x3)mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
	}
	LastWeight = 1.0f - LastWeight;

	// Now that we have the calculated weight, add in the final influence
	Pos += (mul(In.Pos, mul(mWorldMatrixArray[IndexArray[NumBones - 1]], mView)) * LastWeight);
	Out.Normal += (mul(In.Normal, (float3x3)mWorldMatrixArray[IndexArray[NumBones - 1]]) * LastWeight);

	Out.Normal = normalize(Out.Normal);

	//Pos.xyz += Out.Normal.xyz * 1.0f;

	// transform position from world space into view and then projection space
	Out.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);

	Out.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return Out;
}


VS_OUT VShade(VS_IN In, uniform int NumBones)
{
	VS_OUT	Out = (VS_OUT)0;
	float3      Pos = 0.0f;
	float       LastWeight = 0.0f;

	float3	N = 0.0f;		// ワールド空間上の法線ベクトル
	float3	L = 0.0f;		// 光の差し込む方向
	float3	P = 0.0f;		// ワールド空間上の頂点座標
	float3	V = 0.0f;		// （カメラ座標ー頂点座標）ベクトル
	float3  H = 0.0f;		//  ハーフベクトル（視線ベクトルと光の方向ベクトル）

	// Compensate for lack of UBYTE4 on Geforce3
	int4 IndexVector = D3DCOLORtoUBYTE4(In.BlendIndices);

	// cast the vectors to arrays for use in the for loop below
	float BlendWeightsArray[4] = (float[4])In.BlendWeights;
	int   IndexArray[4] = (int[4])IndexVector;

	// calculate the pos/normal using the "normal" weights 
	//        and accumulate the weights to calculate the last weight
	for (int iBone = 0; iBone < NumBones - 1; iBone++)
	{
		LastWeight = LastWeight + BlendWeightsArray[iBone];

		Pos += mul(In.Pos, mul(mWorldMatrixArray[IndexArray[iBone]], mView)) * BlendWeightsArray[iBone];
		Out.Normal += mul(In.Normal, (float3x3)mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
	}
	LastWeight = 1.0f - LastWeight;

	// Now that we have the calculated weight, add in the final influence
	Pos += (mul(In.Pos, mul(mWorldMatrixArray[IndexArray[NumBones - 1]],mView)) * LastWeight);
	Out.Normal += (mul(In.Normal, (float3x3)mWorldMatrixArray[IndexArray[NumBones - 1]]) * LastWeight);

	// transform position from world space into view and then projection space
	Out.Pos = mul(float4(Pos.xyz, 1.0f), mViewProj);

	// normalize normals
	Out.Normal = normalize(Out.Normal);


	P = Out.Pos.xyz;

	N = Out.Normal;

	// 平行光の差し込む方向	単位ベクトル化
	L = normalize(-lt.dir);

	// 視線ベクトルを求める
	V = normalize(eye.xyz - P);

	// 光ベクトルと視線とのハーフベクトルを求める
	H = normalize(L + V);

	// 光源計算を行って出力カラーを決める
	Out.Color = mat.emi +
		lt.amb * mat.amb +
		lt.dif * mat.dif *
		max(0.0f, dot(N, L));	// 0.0未満の場合は0.0に

	Out.Color.a = mat.dif.a;

	// スペキュラーによる反射色を計算　g_powerが大きいほど鋭く光る
	Out.Specular = lt.spc * mat.spc *
		pow(max(0.0f, dot(N, H)), mat.pwr);

	// copy the input texture coordinate through
	Out.Tex0 = In.Tex0.xy;
	return Out;
}

int CurNumBones = 2;
VertexShader vsArray[4] = { compile vs_3_0 VShade(1),
							compile vs_3_0 VShade(2),
							compile vs_3_0 VShade(3),
							compile vs_3_0 VShade(4)
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	float4 out_color;
	// テクスチャの色とポリゴンの色を掛け合わせて出力
	out_color = saturate(In.Color * tex2D(smp, In.Tex0)/* + In.Specular*/);
	out_color.a = In.Color.a;
	return out_color;
}

float4 ps_notex(VS_OUT In) : COLOR0
{
	float4 out_color;
	// テクスチャの色とポリゴンの色を掛け合わせて出力
	out_color = saturate(In.Color /*+ In.Specular*/);
	out_color.a = In.Color.a;
	return out_color;
}

float4 ps_Outline(VS_OUT In) : COLOR0
{
	return In.Color;
}


//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
	//pass p0
	//{
	//	 表面カリング
	//	CullMode = CW;
	//		CullMode = None;

	//	VertexShader = compile vs_3_0 VSOutline(2);
	//	PixelShader = compile ps_3_0 ps_Outline();
	//	 裏面カリング
	//	CullMode = CCW;
	//}

	pass p0
	{
		// 両面描画
		//CullMode = None;
		BLENDOPALPHA = ADD;
		SRCBLEND = SRCALPHA;		// SRCを半透明合成
		DESTBLEND = INVSRCALPHA;			// DESTをそのまま描画
		ZWRITEENABLE = TRUE;		// 深度バッファへの書き込みを無効


		FILLMODE = SOLID;		// 塗りつぶしモード = 面を塗りつぶす（標準）

		VertexShader = (vsArray[CurNumBones]);
		PixelShader = compile ps_3_0 ps_nomal();
		// 裏面カリング
		//CullMode = CCW;


	}
}

technique t1
{
	pass p0
	{
		VertexShader = (vsArray[CurNumBones]);
		PixelShader = compile ps_3_0 ps_notex();
	}
}



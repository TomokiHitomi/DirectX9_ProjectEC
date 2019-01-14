//=============================================================================
//
// ボックスシェーダ [box.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス
float		rot;		// グローバルの回転量
float		color;
//float		AddU;
//float		AddV;

texture tex;			// 使用するテクスチャ
sampler smp = sampler_state {
	texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};



//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;
	float3	worldPos : TEXCOORD1;
	float	angle : TEXCOORD2;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4 pos : POSITION;
	float4 col : COLOR0;
	float2 uv : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float2 uv2 : TEXCOORD2;
	float2 uv3 : TEXCOORD3;
};

float4x4 RotationX(float agl)
{
	float4x4 mtx;
	mtx._11 = 1.0f;
	mtx._12 = 0.0f;
	mtx._13 = 0.0f;
	mtx._14 = 0.0f;
	mtx._21 = 0.0f;
	mtx._22 = cos(agl);
	mtx._23 = sin(agl);
	mtx._24 = 0.0f;
	mtx._31 = 0.0f;
	mtx._32 = -sin(agl);
	mtx._33 = cos(agl);
	mtx._34 = 0.0f;
	mtx._41 = 0.0f;
	mtx._42 = 0.0f;
	mtx._43 = 0.0f;
	mtx._44 = 1.0f;
	return mtx;
}

float4x4 RotationY(float agl)
{
	float4x4 mtx;
	mtx._11 = cos(agl);
	mtx._12 = 0.0f;
	mtx._13 = -sin(agl);
	mtx._14 = 0.0f;
	mtx._21 = 0.0f;
	mtx._22 = 1.0f;
	mtx._23 = 0.0f;
	mtx._24 = 0.0f;
	mtx._31 = sin(agl);
	mtx._32 = 0.0f;
	mtx._33 = cos(agl);
	mtx._34 = 0.0f;
	mtx._41 = 0.0f;
	mtx._42 = 0.0f;
	mtx._43 = 0.0f;
	mtx._44 = 1.0f;
	return mtx;
}

float4x4 RotationZ(float agl)
{
	float4x4 mtx;
	mtx._11 = cos(agl);
	mtx._12 = -sin(agl);
	mtx._13 = 0.0f;
	mtx._14 = 0.0f;
	mtx._21 = sin(agl);
	mtx._22 = cos(agl);
	mtx._23 = 0.0f;
	mtx._24 = 0.0f;
	mtx._31 = 0.0f;
	mtx._32 = 0.0f;
	mtx._33 = 1.0f;
	mtx._34 = 0.0f;
	mtx._41 = 0.0f;
	mtx._42 = 0.0f;
	mtx._43 = 0.0f;
	mtx._44 = 1.0f;
	return mtx;
}

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main( VS_IN In )
{
	VS_OUT Out = (VS_OUT)0;
	//VS_OUT Out;

	Out.pos = float4(
		In.pos.x,
		In.pos.y,
		In.pos.z,
		1.0f
	);

	Out.pos = mul(Out.pos, RotationX(In.angle + rot));
	Out.pos = mul(Out.pos, RotationY(In.angle + (rot / 2.0)));

	Out.pos = mul(Out.pos, world);

	Out.pos = float4(
		Out.pos.x + In.worldPos.x,
		Out.pos.y + In.worldPos.y,
		Out.pos.z + In.worldPos.z,
		1.0f
		);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);


	Out.uv  = In.uv;

	//Out.uv = In.uv + float2(-1 / 256, -1 / 256);
	//Out.uv1 = In.uv + float2( 1 / 256, -1 / 256);
	//Out.uv2 = In.uv + float2(-1 / 256,  1 / 256);
	//Out.uv3 = In.uv + float2( 1 / 256,  1 / 256);



	Out.col = In.col;
	Out.col.y = Out.col.y + color;
	Out.col.z = Out.col.z + color / 2.0;

	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	//float4 txl0 = tex2D(smp,In.uv + float2(0.0f,0.0f))*0.2f;
	//float4 txl1 = tex2D(smp,In.uv + float2(-AddU,-AddV))*0.1f;
	//float4 txl2 = tex2D(smp,In.uv + float2(0.0f,-AddV))*0.1f;
	//float4 txl3 = tex2D(smp,In.uv + float2(+AddU,-AddV))*0.1f;
	//float4 txl4 = tex2D(smp,In.uv + float2(-AddU,0.0f))*0.1f;
	//float4 txl5 = tex2D(smp,In.uv + float2(+AddU,0.0f))*0.1f;
	//float4 txl6 = tex2D(smp,In.uv + float2(-AddU,+AddV))*0.1f;
	//float4 txl7 = tex2D(smp,In.uv + float2(0.0f,+AddV))*0.1f;
	//float4 txl8 = tex2D(smp,In.uv + float2(+AddU,+AddV))*0.1f;
	//float4 color = txl0 + txl1 + txl2 + txl3 + txl4 + txl5 + txl6 + txl7 + txl8;

	//return color * In.col;

	return tex2D( smp, In.uv ) * In.col;
	//return tex2D(smp, In.uv);
}
technique Tec01		// テクスチャ描画
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
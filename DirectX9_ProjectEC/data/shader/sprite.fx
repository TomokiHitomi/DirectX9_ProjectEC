//=============================================================================
//
// スプライトシェーダ [sprite.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス
float		moveTime;

texture tex;			// 使用するテクスチャ
sampler smp = sampler_state {
	texture = <tex>;
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	//float3 pos : POSITION;
	//float4 col : COLOR0;

	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;
	float3	worldPos : TEXCOORD1;
	float3	vec : TEXCOORD2;
	float	move : TEXCOORD3;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4 pos : POSITION;
	float4 col : COLOR0;
	float2 uv : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main( VS_IN In )
{
	//VS_OUT Out = (VS_OUT)0;
	VS_OUT Out;
	float mover = moveTime - In.move;
	Out.pos = float4(
		In.pos.x,
		In.pos.y,
		In.pos.z,
		1.0f
	);

	Out.pos = mul(Out.pos, world);

	Out.pos = float4(
		Out.pos.x + In.worldPos.x + (In.vec.x * mover),
		Out.pos.y + In.worldPos.y + (In.vec.y * mover),
		Out.pos.z + In.worldPos.z + (In.vec.z * mover),
		1.0f
		);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	//mat = mul(mul(world, view), proj);
	//Out.pos = mul( float4(In.pos, 1.0f), mat );
	Out.uv  = In.uv;
	Out.col = In.col;
	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
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
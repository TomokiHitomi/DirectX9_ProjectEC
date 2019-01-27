//=============================================================================
//
// Xファイルシェーダ [xmodel.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	proj;		// プロジェクションマトリクス

// テクスチャ
texture tex;
sampler smp = sampler_state {
	texture = <tex>;
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;

};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main( VS_IN In )
{
	VS_OUT Out = (VS_OUT)0;
	//VS_OUT Out;

	Out.pos = In.pos;

	//Out.pos = mul(Out.pos, world);
	//Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	Out.col = In.col;
	// uvはそのまま使用
	Out.uv  = In.uv;

	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return tex2D(smp, In.uv) * In.col;
}

//=============================================================================
// テクニック
//=============================================================================
technique Tec01
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファブレンド = 有効
		ALPHABLENDENABLE = TRUE;
		// アルファブレンドオプション = 転送元 + 転送先（標準）
		BLENDOPALPHA = ADD;

		// 新規ブレンド = 係数(As, As, As, As)
		SRCBLEND = SRCALPHA;
		// 既存ブレンド = 係数( 1 - As, 1 - As, 1 - As, 1 - As)
		DESTBLEND = INVSRCALPHA;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
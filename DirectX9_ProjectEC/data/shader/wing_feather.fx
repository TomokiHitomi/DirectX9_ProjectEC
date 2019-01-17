//=============================================================================
//
// スプライトシェーダ [sprite.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス

float		g_fMorph;
float		g_fMove;


texture tex;			// 使用するテクスチャ
sampler smp = sampler_state {
	texture = <tex>;
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	// 頂点情報
	float4	pos			: POSITION;
	float2	uv			: TEXCOORD0;
	float4	col			: COLOR0;

	// インスタンス情報
	float3	worldPos	: TEXCOORD1;
	float3	vec			: TEXCOORD2;
	float	morph		: TEXCOORD3;
	float2	shiftuv		: TEXCOORD4;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4 pos			: POSITION;
	float4 col			: COLOR0;
	float2 uv			: TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// 頂点座標を格納
	Out.pos = float4(In.pos.x, In.pos.y, In.pos.z, 1.0f);
	
	// ワールド行列を乗算（ビルボード化）
	Out.pos = mul(Out.pos, world);

	// インスタンシング用ワールド座標を加算（Y座標には移動量 * モーフ値）
	Out.pos = float4(
		Out.pos.x + In.worldPos.x,
		Out.pos.y + In.worldPos.y + (g_fMove * saturate(g_fMorph - In.morph)),
		Out.pos.z + In.worldPos.z,
		1.0f
		);

	// ビュー・プロジェクション変換
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// UV座標の設定：初期UV + 相対UV
	Out.uv = In.uv + In.shiftuv;

	// カラーの設定
	Out.col = In.col;
	// α値はモーフ値で透明化
	Out.col.a = Out.col.a - saturate(g_fMorph - In.morph);

	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return tex2D(smp, In.uv) * In.col;
//return tex2D(smp, In.uv);
}
technique Tec01		// テクスチャ描画
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファブレンド = 有効
		ALPHABLENDENABLE = TRUE;
		// アルファブレンドオプション = 転送元 + 転送先（標準）
		BLENDOP = ADD;

		// 新規ブレンド = 係数(As, As, As, As)
		SRCBLEND = SRCALPHA;
		// 既存ブレンド = 係数(1, 1, 1, 1)
		DESTBLEND = ONE;

		// 深度バッファへの書き込み = 無効
		ZWRITEENABLE = FALSE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
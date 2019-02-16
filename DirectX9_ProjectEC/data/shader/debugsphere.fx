//=============================================================================
//
// デバッグ球体シェーダ [debugsphere.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス

float4		g_vColor;

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	float3	pos : POSITION;
	// インスタンス情報
	float3	worldPos	: TEXCOORD0;
	float	size		: TEXCOORD1;
	float	use			: TEXCOORD2;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4	pos : POSITION;
	float4	col	: COLOR0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main( VS_IN In )
{
	VS_OUT Out = (VS_OUT)0;

	Out.pos = float4(
		In.pos.x * In.size,
		In.pos.y * In.size,
		In.pos.z * In.size,
		1.0f
		);

	float4x4 mtxWorld = world;

	// インスタンシング用ワールド座標をワールドマトリクスに追加
	mtxWorld._41 = In.worldPos.x;
	mtxWorld._42 = In.worldPos.y;
	mtxWorld._43 = In.worldPos.z;

	Out.pos = mul(Out.pos, mtxWorld);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	//Out.col = g_vColor;
	Out.col = g_vColor * In.use;
	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return In.col;
}

//=============================================================================
// テクニック
//=============================================================================
technique Tec01
{
	pass p0
	{
		// 塗りつぶしモード = ワイヤーフレーム
		FILLMODE = WIREFRAME;

		// アルファテスト = 無効（標準）
		ALPHATESTENABLE = FALSE;

		// アルファブレンド = 無効（標準）
		ALPHABLENDENABLE = FALSE;

		// 新規ブレンド = 係数(1, 1, 1, 1)（標準）
		SRCBLEND = ONE;
		// 既存ブレンド = 係数(0, 0, 0, 0)（標準）
		DESTBLEND = ZERO;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
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
float4		g_vMove;
float4		g_vPos;		// 設置座標

int			alphaRef = 0x08;


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
	float	rot		: TEXCOORD3;
	float2	shiftuv		: TEXCOORD4;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4 pos			: POSITION;
	float4 col			: COLOR0;
	float2 uv			: TEXCOORD0;
};

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
VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// 頂点座標を格納
	Out.pos = float4(In.pos.x, In.pos.y, In.pos.z, 1.0f);

	// ワールドマトリクスに回転をかけてコピー
	float4x4 mtxWorld = mul(RotationZ(In.rot), world);
	//float4x4 mtxWorld =  world;

	// インスタンシング用ワールド座標をワールドマトリクスに追加
	mtxWorld._41 = g_vPos.x + ((In.worldPos.x + g_vMove.x) % (g_vMove.w));
	mtxWorld._42 = In.worldPos.y;
	mtxWorld._43 = g_vPos.z + ((In.worldPos.z + g_vMove.z) % (g_vMove.w));

	// ワールド行列を変換（ビルボード化）
	Out.pos = mul(Out.pos, mtxWorld);

	// ビュー・プロジェクション変換
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// UV座標の設定：初期UV + 相対UV
	Out.uv = In.uv + In.shiftuv;

	// カラーの設定
	Out.col = In.col;

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

		//Blendop = ADD;			// SRC(これから描画する側) + DEST(既に描画されてる側)
		//SrcBlend = SRCALPHA;
		//DestBlend = ONE;			// DESTをそのまま描画
		//ZWriteEnable = FALSE;		// 深度バッファへの書き込みを無効
		//ZWRITEENABLE = FALSE;

		//// αブレンドの有効
		//AlphaBlendEnable = true;	// α合成を使用


		//// αテスト
		//AlphaTestEnable = true;
		//AlphaFunc = Greater;
		//AlphaRef = <alphaRef>;


		//BLENDOP = MAX;
		BLENDOPALPHA = MAX;
		SRCBLEND = SRCALPHA;		// SRCを半透明合成
		DESTBLEND = ONE;			// DESTをそのまま描画
		ZWRITEENABLE = FALSE;		// 深度バッファへの書き込みを無効
		//AlphaBlendEnable = true;	// α合成を使用

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();

		//ZWriteEnable = TRUE;		// 深度バッファへの書き込みを無効
		//AlphaBlendEnable = FALSE;	// α合成を使用


		//AlphaBlendEnable = false;
		//DestBlend = INVSRCALPHA;			// DESTをそのまま描画
		//SrcBlend = SRCALPHA;

		//SrcBlend = SRCALPHA;		// SRCを半透明合成
		//DestBlend = INVSRCALPHA;	// DESTをそのまま描画
		//ZWriteEnable = true;		// 深度バッファへの書き込みを有効
	}
}
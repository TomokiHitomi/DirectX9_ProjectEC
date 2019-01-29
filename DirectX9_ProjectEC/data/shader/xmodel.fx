//=============================================================================
//
// Xファイルシェーダ [xmodel.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス
float4		eye;		// 視点座標

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
	float3	pos : POSITION;
	float3	nor : NORMAL;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;

};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4	pos : POSITION;
	float3	nor : NORMAL;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;
	float4	spc : TEXCOORD1;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_light_on( VS_IN In )
{
	VS_OUT Out = (VS_OUT)0;
	//VS_OUT Out;

	Out.pos = float4(
		In.pos.x,
		In.pos.y,
		In.pos.z,
		1.0f
	);

	Out.pos = mul(Out.pos, world);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// 法線をワールド空間へ
	Out.nor = mul(In.nor, (float3x3)world);

	float3	N = 0.0f;		// ワールド空間上の法線ベクトル
	float3	L = 0.0f;		// 光の差し込む方向
	float3	P = 0.0f;		// ワールド空間上の頂点座標
	float3	V = 0.0f;		// （カメラ座標ー頂点座標）ベクトル
	float3  H = 0.0f;		//  ハーフベクトル（視線ベクトルと光の方向ベクトル）

	P = Out.pos.xyz;

	N = normalize(Out.nor);

	// 平行光の差し込む方向	単位ベクトル化
	L = normalize(-lt.dir);

	// 視線ベクトルを求める
	V = normalize(eye.xyz - P);

	// 光ベクトルと視線とのハーフベクトルを求める
	H = normalize(L + V);

	// 光源計算を行って出力カラーを決める
	Out.col = mat.emi +
		lt.amb * mat.amb +
		lt.dif * mat.dif *
		max(0.0f, dot(N, L));	// 0.0未満の場合は0.0に

	Out.col.a = mat.dif.a;

	// スペキュラーによる反射色を計算　g_powerが大きいほど鋭く光る
	Out.spc = lt.spc * mat.spc *
		pow(max(0.0f, dot(N, H)), mat.pwr);



	// uvはそのまま使用
	Out.uv  = In.uv;

	return Out;
}

VS_OUT vs_light_off(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.pos = float4(
		In.pos.x,
		In.pos.y,
		In.pos.z,
		1.0f
		);

	Out.pos = mul(Out.pos, world);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	Out.uv = In.uv;

	// 光源計算を行って出力カラーを決める
	Out.col = saturate(mat.emi + mat.amb + mat.dif);	// 0.0未満の場合は0.0に

	Out.col.a = mat.dif.a;

	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_light_on_tex(VS_OUT In) : COLOR0
{
	float4 out_color;
	// テクスチャの色とポリゴンの色を掛け合わせて出力
	out_color = saturate(In.col * tex2D(smp, In.uv) + In.spc);
	out_color.a = In.col.a;
	return out_color; 
}

float4 ps_light_off_tex(VS_OUT In) : COLOR0
{
	return tex2D(smp, In.uv);
}

float4 ps_light_on(VS_OUT In) : COLOR0
{
	float4 out_color;
	// テクスチャの色とポリゴンの色を掛け合わせて出力
	out_color = saturate(In.col + In.spc);
	out_color.a = In.col.a;
	return out_color;
}

float4 ps_light_off(VS_OUT In) : COLOR0
{
	return In.col;
}

//=============================================================================
// テクニック
//=============================================================================
technique LIGHT_ON_TEX
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファテスト = 無効（標準）
		ALPHATESTENABLE = FALSE;

		// アルファブレンド = 無効（標準
		ALPHABLENDENABLE = FALSE;
		//// アルファブレンドオプション = 転送元 + 転送先（標準）
		//BLENDOPALPHA = ADD;

		// 新規ブレンド = 係数(1, 1, 1, 1)（標準）
		SRCBLEND = ONE;
		// 既存ブレンド = 係数(0, 0, 0, 0)（標準）
		DESTBLEND = ZERO;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;

		VertexShader = compile vs_3_0 vs_light_on();
		PixelShader = compile ps_3_0 ps_light_on_tex();
	}
}

technique LIGHT_OFF_TEX
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファテスト = 無効（標準）
		ALPHATESTENABLE = FALSE;

		// アルファブレンド = 無効（標準
		ALPHABLENDENABLE = FALSE;
		//// アルファブレンドオプション = 転送元 + 転送先（標準）
		//BLENDOPALPHA = ADD;

		// 新規ブレンド = 係数(1, 1, 1, 1)（標準）
		SRCBLEND = ONE;
		// 既存ブレンド = 係数(0, 0, 0, 0)（標準）
		DESTBLEND = ZERO;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;
		VertexShader = compile vs_3_0 vs_light_off();
		PixelShader = compile ps_3_0 ps_light_off_tex();
	}
}

technique LIGHT_ON
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファテスト = 無効（標準）
		ALPHATESTENABLE = FALSE;

		// アルファブレンド = 無効（標準
		ALPHABLENDENABLE = FALSE;
		//// アルファブレンドオプション = 転送元 + 転送先（標準）
		//BLENDOPALPHA = ADD;

		// 新規ブレンド = 係数(1, 1, 1, 1)（標準）
		SRCBLEND = ONE;
		// 既存ブレンド = 係数(0, 0, 0, 0)（標準）
		DESTBLEND = ZERO;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;
		VertexShader = compile vs_3_0 vs_light_on();
		PixelShader = compile ps_3_0 ps_light_on();
	}
}

technique LIGHT_OFF
{
	pass p0
	{
		// 塗りつぶしモード = 面を塗りつぶす（標準）
		FILLMODE = SOLID;

		// アルファテスト = 無効（標準）
		ALPHATESTENABLE = FALSE;

		// アルファブレンド = 無効（標準
		ALPHABLENDENABLE = FALSE;
		//// アルファブレンドオプション = 転送元 + 転送先（標準）
		//BLENDOPALPHA = ADD;

		// 新規ブレンド = 係数(1, 1, 1, 1)（標準）
		SRCBLEND = ONE;
		// 既存ブレンド = 係数(0, 0, 0, 0)（標準）
		DESTBLEND = ZERO;

		// 深度バッファへの書き込み = 有効（標準）
		ZWRITEENABLE = TRUE;
		VertexShader = compile vs_3_0 vs_light_off();
		PixelShader = compile ps_3_0 ps_light_off();
	}
}
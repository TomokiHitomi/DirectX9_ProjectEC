//=============================================================================
//
// 八面体シェーダ [octa.fx]
// Author : GP12A295 25 人見友基
//
//=============================================================================

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FX_OCTA_AMBIENT	(0.4f)
#define FX_OCTA_POWER	(5.0f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VS_IN		// 頂点シェーダの引数
{
	float3	pos : POSITION;
	float3	nor : NORMAL;
	// インスタンス情報
	float3	worldPos	: TEXCOORD0;
	float	size		: TEXCOORD1;
	float	use			: TEXCOORD2;
	float	rot			: TEXCOORD3;
	float4	col			: COLOR0;
};

struct VS_OUT		// 頂点シェーダの戻り値かつピクセルシェーダーの引数
{
	float4	pos : POSITION;
	float3	nor : NORMAL;
	float4	col	: COLOR0;
	float4	spc : TEXCOORD0;
};

// ライト
typedef struct _LIGHT
{
	float4		dif;	// 拡散光
	float4		amb;	// 環境光
	float4		spc;	// 反射光
	float3		pos;	// 座標
	float3		dir;	// 平行光源
}LIGHT;

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
float4x4	world;		// ワールドマトリクス
float4x4	view;		// ビューマトリクス
float4x4	proj;		// プロジェクションマトリクス
float4		eye;		// 視点座標
LIGHT		lt;			// ライト

//*****************************************************************************
// サンプラー
//*****************************************************************************


//=============================================================================
// 回転（Y）
//=============================================================================
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

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.pos = float4(
		In.pos.x * In.size,
		In.pos.y * In.size,
		In.pos.z * In.size,
		1.0f
		);

	float4x4 mtxWorld = world;

	mtxWorld = mul(RotationY(In.rot), mtxWorld);

	// インスタンシング用ワールド座標をワールドマトリクスに追加
	mtxWorld._41 = In.worldPos.x;
	mtxWorld._42 = In.worldPos.y;
	mtxWorld._43 = In.worldPos.z;

	Out.pos = mul(Out.pos, mtxWorld);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// 法線をワールド空間へ
	Out.nor = mul(In.nor, (float3x3)mtxWorld);

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
	V = normalize(P - eye.xyz);

	// 光ベクトルと視線とのハーフベクトルを求める
	H = normalize(L + V);

	// 光源計算を行って出力カラーを決める
	Out.col = In.col * FX_OCTA_AMBIENT +
		lt.dif * In.col *
		max(0.0f, dot(N, L));	// 0.0未満の場合は0.0に

	Out.col.a = In.col.a * In.use;

	// スペキュラーによる反射色を計算　g_powerが大きいほど鋭く光る
	Out.spc = lt.spc *
		pow(max(0.0f, dot(N, H)), FX_OCTA_POWER);

	//Out.col = saturate(Out.col + Out.spc);
	//Out.col = In.col;


	//Out.col = g_vColor;
	//Out.col = In.col * In.use;
	return Out;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	float4 out_color;
	// テクスチャの色とポリゴンの色を掛け合わせて出力
	out_color = saturate(In.col + In.spc);
	out_color.a = In.col.a;
	return out_color;
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

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
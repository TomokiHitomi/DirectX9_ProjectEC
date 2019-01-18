//=============================================================================
//
// ���ʑ̃V�F�[�_ [octa.fx]
// Author : GP12A295 25 �l���F��
//
//=============================================================================

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define FX_OCTA_AMBIENT	(0.4f)
#define FX_OCTA_POWER	(5.0f)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct VS_IN		// ���_�V�F�[�_�̈���
{
	float3	pos : POSITION;
	float3	nor : NORMAL;
	// �C���X�^���X���
	float3	worldPos	: TEXCOORD0;
	float	size		: TEXCOORD1;
	float	use			: TEXCOORD2;
	float	rot			: TEXCOORD3;
	float4	col			: COLOR0;
};

struct VS_OUT		// ���_�V�F�[�_�̖߂�l���s�N�Z���V�F�[�_�[�̈���
{
	float4	pos : POSITION;
	float3	nor : NORMAL;
	float4	col	: COLOR0;
	float4	spc : TEXCOORD0;
};

// ���C�g
typedef struct _LIGHT
{
	float4		dif;	// �g�U��
	float4		amb;	// ����
	float4		spc;	// ���ˌ�
	float3		pos;	// ���W
	float3		dir;	// ���s����
}LIGHT;

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
float4x4	world;		// ���[���h�}�g���N�X
float4x4	view;		// �r���[�}�g���N�X
float4x4	proj;		// �v���W�F�N�V�����}�g���N�X
float4		eye;		// ���_���W
LIGHT		lt;			// ���C�g

//*****************************************************************************
// �T���v���[
//*****************************************************************************


//=============================================================================
// ��]�iY�j
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
// ���_�V�F�[�_
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

	// �C���X�^���V���O�p���[���h���W�����[���h�}�g���N�X�ɒǉ�
	mtxWorld._41 = In.worldPos.x;
	mtxWorld._42 = In.worldPos.y;
	mtxWorld._43 = In.worldPos.z;

	Out.pos = mul(Out.pos, mtxWorld);
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// �@�������[���h��Ԃ�
	Out.nor = mul(In.nor, (float3x3)mtxWorld);

	float3	N = 0.0f;		// ���[���h��ԏ�̖@���x�N�g��
	float3	L = 0.0f;		// ���̍������ޕ���
	float3	P = 0.0f;		// ���[���h��ԏ�̒��_���W
	float3	V = 0.0f;		// �i�J�������W�[���_���W�j�x�N�g��
	float3  H = 0.0f;		//  �n�[�t�x�N�g���i�����x�N�g���ƌ��̕����x�N�g���j

	P = Out.pos.xyz;

	N = normalize(Out.nor);

	// ���s���̍������ޕ���	�P�ʃx�N�g����
	L = normalize(-lt.dir);

	// �����x�N�g�������߂�
	V = normalize(P - eye.xyz);

	// ���x�N�g���Ǝ����Ƃ̃n�[�t�x�N�g�������߂�
	H = normalize(L + V);

	// �����v�Z���s���ďo�̓J���[�����߂�
	Out.col = In.col * FX_OCTA_AMBIENT +
		lt.dif * In.col *
		max(0.0f, dot(N, L));	// 0.0�����̏ꍇ��0.0��

	Out.col.a = In.col.a * In.use;

	// �X�y�L�����[�ɂ�锽�ːF���v�Z�@g_power���傫���قǉs������
	Out.spc = lt.spc *
		pow(max(0.0f, dot(N, H)), FX_OCTA_POWER);

	//Out.col = saturate(Out.col + Out.spc);
	//Out.col = In.col;


	//Out.col = g_vColor;
	//Out.col = In.col * In.use;
	return Out;
}

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	float4 out_color;
	// �e�N�X�`���̐F�ƃ|���S���̐F���|�����킹�ďo��
	out_color = saturate(In.col + In.spc);
	out_color.a = In.col.a;
	return out_color;
}

//=============================================================================
// �e�N�j�b�N
//=============================================================================
technique Tec01
{
	pass p0
	{
		// �h��Ԃ����[�h = �ʂ�h��Ԃ��i�W���j
		FILLMODE = SOLID;

		// �A���t�@�u�����h = �����i�W��
		ALPHABLENDENABLE = FALSE;
		//// �A���t�@�u�����h�I�v�V���� = �]���� + �]����i�W���j
		//BLENDOPALPHA = ADD;

		// �V�K�u�����h = �W��(1, 1, 1, 1)�i�W���j
		SRCBLEND = ONE;
		// �����u�����h = �W��(0, 0, 0, 0)�i�W���j
		DESTBLEND = ZERO;

		// �[�x�o�b�t�@�ւ̏������� = �L���i�W���j
		ZWRITEENABLE = TRUE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
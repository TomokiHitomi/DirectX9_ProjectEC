//=============================================================================
//
// �_�V�F�[�_ [cloud.fx]
// Author : GP12A295 25 �l���F��
//
//=============================================================================

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct VS_IN		// ���_�V�F�[�_�̈���
{
	// ���_���
	float4	pos			: POSITION;
	float2	uv			: TEXCOORD0;
	float4	col			: COLOR0;

	// �C���X�^���X���
	float3	worldPos	: TEXCOORD1;
	float3	vec			: TEXCOORD2;
	float	rot		: TEXCOORD3;
	float2	shiftuv		: TEXCOORD4;
};

struct VS_OUT		// ���_�V�F�[�_�̖߂�l���s�N�Z���V�F�[�_�[�̈���
{
	float4 pos			: POSITION;
	float4 col			: COLOR0;
	float2 uv			: TEXCOORD0;
};

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
float4x4	world;		// ���[���h�}�g���N�X
float4x4	view;		// �r���[�}�g���N�X
float4x4	proj;		// �v���W�F�N�V�����}�g���N�X

float		g_fMorph;
float4		g_vMove;
float4		g_vPos;		// �ݒu���W




texture tex;			// �g�p����e�N�X�`��

//*****************************************************************************
// �T���v���[
//*****************************************************************************
sampler smp = sampler_state {
	texture = <tex>;
};

//=============================================================================
// ��]�s��iZ�j
//=============================================================================
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
// ���_�V�F�[�_
//=============================================================================
VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// ���_���W���i�[
	Out.pos = float4(In.pos.x, In.pos.y, In.pos.z, 1.0f);

	// ���[���h�}�g���N�X�ɉ�]�������ăR�s�[
	float4x4 mtxWorld = mul(RotationZ(In.rot), world);
	//float4x4 mtxWorld =  world;

	// �C���X�^���V���O�p���[���h���W�����[���h�}�g���N�X�ɒǉ�
	mtxWorld._41 = g_vPos.x + ((In.worldPos.x + g_vMove.x) % (g_vMove.w));
	mtxWorld._42 = In.worldPos.y;
	mtxWorld._43 = g_vPos.z + ((In.worldPos.z + g_vMove.z) % (g_vMove.w));

	// ���[���h�s���ϊ��i�r���{�[�h���j
	Out.pos = mul(Out.pos, mtxWorld);

	// �r���[�E�v���W�F�N�V�����ϊ�
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// UV���W�̐ݒ�F����UV + ����UV
	Out.uv = In.uv + In.shiftuv;

	// �J���[�̐ݒ�
	Out.col = In.col;

	return Out;
}

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return tex2D(smp, In.uv) * In.col;
//return tex2D(smp, In.uv);
}
technique Tec01		// �e�N�X�`���`��
{
	pass p0
	{
		// �h��Ԃ����[�h = �ʂ�h��Ԃ��i�W���j
		FILLMODE = SOLID;

		// �A���t�@�e�X�g = �����i�W���j
		ALPHATESTENABLE = FALSE;

		// �A���t�@�u�����h = �L��
		ALPHABLENDENABLE = TRUE;
		// �A���t�@�u�����h�I�v�V���� = MAX(�]����, �]����)
		BLENDOP = MAX;

		// �V�K�u�����h = �W��(As, As, As, As)
		SRCBLEND = SRCALPHA;
		// �����u�����h = �W��(1, 1, 1, 1)
		DESTBLEND = ONE;

		// �[�x�o�b�t�@�ւ̏������� = ����
		ZWRITEENABLE = FALSE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
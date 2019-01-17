//=============================================================================
//
// �X�v���C�g�V�F�[�_ [sprite.fx]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
float4x4	world;		// ���[���h�}�g���N�X
float4x4	view;		// �r���[�}�g���N�X
float4x4	proj;		// �v���W�F�N�V�����}�g���N�X

float		g_fMorph;
float		g_fMove;


texture tex;			// �g�p����e�N�X�`��
sampler smp = sampler_state {
	texture = <tex>;
};

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
	float	morph		: TEXCOORD3;
	float2	shiftuv		: TEXCOORD4;
};

struct VS_OUT		// ���_�V�F�[�_�̖߂�l���s�N�Z���V�F�[�_�[�̈���
{
	float4 pos			: POSITION;
	float4 col			: COLOR0;
	float2 uv			: TEXCOORD0;
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
VS_OUT vs_main(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// ���_���W���i�[
	Out.pos = float4(In.pos.x, In.pos.y, In.pos.z, 1.0f);
	
	// ���[���h�s�����Z�i�r���{�[�h���j
	Out.pos = mul(Out.pos, world);

	// �C���X�^���V���O�p���[���h���W�����Z�iY���W�ɂ͈ړ��� * ���[�t�l�j
	Out.pos = float4(
		Out.pos.x + In.worldPos.x,
		Out.pos.y + In.worldPos.y + (g_fMove * saturate(g_fMorph - In.morph)),
		Out.pos.z + In.worldPos.z,
		1.0f
		);

	// �r���[�E�v���W�F�N�V�����ϊ�
	Out.pos = mul(Out.pos, view);
	Out.pos = mul(Out.pos, proj);

	// UV���W�̐ݒ�F����UV + ����UV
	Out.uv = In.uv + In.shiftuv;

	// �J���[�̐ݒ�
	Out.col = In.col;
	// ���l�̓��[�t�l�œ�����
	Out.col.a = Out.col.a - saturate(g_fMorph - In.morph);

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

		// �A���t�@�u�����h = �L��
		ALPHABLENDENABLE = TRUE;
		// �A���t�@�u�����h�I�v�V���� = �]���� + �]����i�W���j
		BLENDOP = ADD;

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
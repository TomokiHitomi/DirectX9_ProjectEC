//=============================================================================
//
// X�t�@�C���V�F�[�_ [xmodel.fx]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
float4x4	world;		// ���[���h�}�g���N�X
float4x4	view;		// �r���[�}�g���N�X
float4x4	proj;		// �v���W�F�N�V�����}�g���N�X

float4		g_vColor;

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct VS_IN		// ���_�V�F�[�_�̈���
{
	float3	pos : POSITION;
	// �C���X�^���X���
	float3	worldPos	: TEXCOORD0;
	float	size		: TEXCOORD1;
	float	use			: TEXCOORD2;
};

struct VS_OUT		// ���_�V�F�[�_�̖߂�l���s�N�Z���V�F�[�_�[�̈���
{
	float4	pos : POSITION;
	float4	col	: COLOR0;
};

//=============================================================================
// ���_�V�F�[�_
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

	// �C���X�^���V���O�p���[���h���W�����[���h�}�g���N�X�ɒǉ�
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
// �s�N�Z���V�F�[�_
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return In.col;
}

//=============================================================================
// �e�N�j�b�N
//=============================================================================
technique Tec01
{
	pass p0
	{
		FILLMODE = WIREFRAME;	// �h��Ԃ����[�h = ���C���[�t���[���i�W���j
		BLENDOPALPHA = ADD;
		SRCBLEND = SRCALPHA;		// SRC�𔼓�������
		DESTBLEND = INVSRCALPHA;			// DEST�����̂܂ܕ`��
		ZWRITEENABLE = TRUE;		// �[�x�o�b�t�@�ւ̏������݂𖳌�

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
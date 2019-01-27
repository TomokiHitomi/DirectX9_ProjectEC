//=============================================================================
//
// X�t�@�C���V�F�[�_ [xmodel.fx]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
float4x4	proj;		// �v���W�F�N�V�����}�g���N�X

// �e�N�X�`��
texture tex;
sampler smp = sampler_state {
	texture = <tex>;
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct VS_IN		// ���_�V�F�[�_�̈���
{
	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;

};

struct VS_OUT		// ���_�V�F�[�_�̖߂�l���s�N�Z���V�F�[�_�[�̈���
{
	float4	pos : POSITION;
	float4	col : COLOR0;
	float2	uv : TEXCOORD0;
};

//=============================================================================
// ���_�V�F�[�_
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
	// uv�͂��̂܂܎g�p
	Out.uv  = In.uv;

	return Out;
}

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
float4 ps_nomal(VS_OUT In) : COLOR0
{
	return tex2D(smp, In.uv) * In.col;
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

		// �A���t�@�u�����h = �L��
		ALPHABLENDENABLE = TRUE;
		// �A���t�@�u�����h�I�v�V���� = �]���� + �]����i�W���j
		BLENDOPALPHA = ADD;

		// �V�K�u�����h = �W��(As, As, As, As)
		SRCBLEND = SRCALPHA;
		// �����u�����h = �W��( 1 - As, 1 - As, 1 - As, 1 - As)
		DESTBLEND = INVSRCALPHA;

		// �[�x�o�b�t�@�ւ̏������� = �L���i�W���j
		ZWRITEENABLE = TRUE;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_nomal();
	}
}
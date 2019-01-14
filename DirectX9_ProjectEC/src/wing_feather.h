//=============================================================================
//
// �E�B���O�t�F�U�[���� [wing_feather.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _WING_FEATHER_H_
#define _WING_FEATHER_H_

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �ǂݍ��ރe�N�X�`���t�@�C����
#define	WING_FEATHER_TEXTURE			("data/texture/Feather.png")

// �e�N�X�`����������
#define WING_FEATHER_TEXTURE_DIVIDE_X	(4)
#define WING_FEATHER_TEXTURE_DIVIDE_Y	(4)
// �e�N�X�`�������p�^�[��
#define WING_FEATHER_TEXTURE_NUM		(WING_FEATHER_TEXTURE_DIVIDE_X * WING_FEATHER_TEXTURE_DIVIDE_Y)

// �T�C�Y
#define	WING_FEATHER_SIZE_X				(1.5f)
#define	WING_FEATHER_SIZE_Y				(1.5f)

// �ő吔
#define WING_FEATHER_MAX				(100)

// ���[�t�̕ω��X�s�[�h
#define WING_FEATHER_MORPH_SPEED		(0.01f)

// �ő�ړ��ʁi�����j
#define WING_FEATHER_MOVE				(-50.0f)

// �ݒu�Ԋu�i�t���[���j
#define WING_FEATHER_SET_TIME			(10)

// �ݒu�����i1�`�w��l�̃����_���j
#define WING_FEATHER_SET_LENGTH			(20)


//*****************************************************************************
// �N���X��`
//*****************************************************************************
class WingFeather
{
private:
	LPDIRECT3DTEXTURE9				m_pTexture;	// �e�N�X�`��
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;	// ���_�o�b�t�@
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// �C���X�^���X�o�b�t�@
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;	// �C���f�b�N�X�o�b�t�@
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// ���_�錾

	// �V�F�[�_�֘A
	LPD3DXEFFECT					pEffect;
	enum EH
	{
		EH_TEX,
		EH_PROJ,
		EH_VIEW,
		EH_WORLD,
		EH_MOVE,
		EH_MORPH,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// �V�F�[�_�n���h��

	int								m_nCount;
	float							m_fMorph;

	typedef struct
	{
		D3DXVECTOR4 vtx;
		D3DXVECTOR2 uv;
		D3DCOLOR	diffuse;
	}VERTEX;

	typedef struct
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 vec;
		float		morph;
		D3DXVECTOR2 shiftuv;
	}INSTANCE;

	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
public:
	// �R���X�g���N�^
	WingFeather(void);
	//�f�X�g���N�^
	~WingFeather(void);
	// ����������
	HRESULT	Init(void);
	// �������
	void	Release(void);
	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);
	// �ݒu����
	void	Set(D3DXVECTOR3 pos);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

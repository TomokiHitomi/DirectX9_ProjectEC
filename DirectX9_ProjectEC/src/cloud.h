//=============================================================================
//
// �N���E�h���� [cloud.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _CLOUD_H_
#define _CLOUD_H_

#include "main.h"
#include "object.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �ǂݍ��ރe�N�X�`���t�@�C����
#define	CLOUD_TEXTURE			("data/TEXTURE/cloud2.png")

// �e�N�X�`����������
#define CLOUD_TEXTURE_DIVIDE_X	(2)
#define CLOUD_TEXTURE_DIVIDE_Y	(2)

// �e�N�X�`�������p�^�[��
#define CLOUD_TEXTURE_NUM		(CLOUD_TEXTURE_DIVIDE_X * CLOUD_TEXTURE_DIVIDE_Y)

// �T�C�Y
#define	CLOUD_SIZE					(150.0f)
#define	CLOUD_SIZE_X				(CLOUD_SIZE)
#define	CLOUD_SIZE_Y				(CLOUD_SIZE)

// �ő吔
#define CLOUD_MAX_X				(100)
#define CLOUD_MAX_Y				(3)
#define CLOUD_MAX_Z				(100)
#define CLOUD_MAX				(CLOUD_MAX_X * CLOUD_MAX_Y * CLOUD_MAX_Z)

#define CLOUD_POS_MARGIN		(100.0f)
#define CLOUD_POS_MARGIN_Y		(30.0f)

#define CLOUD_POS_RANDOM_VECTOR	(10.0f)

// ���[�t�̕ω��X�s�[�h
#define CLOUD_MORPH_SPEED		(0.01f)

// �ő�ړ��ʁi�����j
#define CLOUD_MOVE				(-50.0f)

// �ݒu�Ԋu�i�t���[���j
#define CLOUD_SET_TIME			(10)

// �ݒu�����i1�`�w��l�̃����_���j
#define CLOUD_SET_LENGTH			(20)


//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Cloud : public ObjectManager
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
		EH_DATA,
		EH_MORPH,
		EH_POS,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// �V�F�[�_�n���h��

	int								m_nCount;
	float							m_fMorph;
	float							m_fMove;
	D3DXVECTOR4						m_vPos;
	D3DXVECTOR4						m_vPosSet;
	D3DXVECTOR4						m_vMove;	// (move.x, move.y, 0.0f, size)

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
		float		rot;
		D3DXVECTOR2 shiftuv;
	}INSTANCE;

	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
public:
	// �R���X�g���N�^
	Cloud(void);
	//�f�X�g���N�^
	~Cloud(void);
	// ����������
	HRESULT	Init(void);
	// �������
	void	Release(void);
	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);
	// �ݒu����
	//void	Set(D3DXVECTOR3 pos);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _BULLET_H_
#define _BULLET_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX					(8192)
#define BULLET_TEX					("data/texture/bullet.png")
#define BULLET_TEXTURE_DIVIDE_X		(5)
#define BULLET_TEXTURE_DIVIDE_Y		(3)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Bullet
{
public:
	// �R���X�g���N�^�i�����������j
	Bullet(void);
	//�f�X�g���N�^�i�I�������j
	~Bullet(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);

	// �g�p��Ԃɂ���
	int Set(float fSize);
	// �J���[���Z�b�g
	void SetColor(int nIdx, float fCol);
	// ���W���Z�b�g
	void SetPos(int nIdx, D3DXVECTOR3 vPos);


	// ���
	void Release(int nIdx);
private:


	LPD3DXEFFECT		pEffect;		// �V�F�[�_�[�֘A
	enum EH
	{
		EH_TEX,
		EH_PROJ,
		EH_VIEW,
		EH_WORLD,
		EH_COLORPALLET,
		EH_MAX
	};
	D3DXHANDLE						hEffectHandle[EH_MAX];	// �V�F�[�_�n���h��

	LPDIRECT3DTEXTURE9				m_pTexture;	// �e�N�X�`��

	D3DXMATRIX			m_mtxWorld;		// ���[���h�}�g���N�X
	UINT				m_nCount;
	bool				m_bUse;			// �g�p�t���O

	// �C���X�^���V���O�p
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;		// ���_�o�b�t�@
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// �C���X�^���X�o�b�t�@
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;		// �C���f�b�N�X�o�b�t�@
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// ���_�錾

	typedef struct
	{
		D3DXVECTOR4 vtx;
		D3DXVECTOR2 uv;
	}VERTEX;

	// VRAM�p
	typedef struct
	{
		D3DXVECTOR3 vPos;		// ���[���h���W
		float		fSize;		// �T�C�Y
		float		fUse;		// �g�p�t���O�i�V�F�[�_�p�j
		float		fCol;		// �J���[�C���f�b�N�X
	}INSTANCE;

	// RAM�p
	typedef struct : public INSTANCE
	{
		bool bUse;				// �g�p�t���O
	}DATA;

	DATA	m_tData[BULLET_MAX];


	// �}�g���N�X�̏�����
	HRESULT InitMatrix(void);

	// ���b�V���̐���
	HRESULT CreateMesh(LPDIRECT3DDEVICE9 pDevice);
	// ���b�V�����̎擾
	HRESULT GetMeshData(LPD3DXMESH pMesh);

	// �C���X�^���V���O�p�f�[�^�̏�����
	HRESULT InitInst(void);
	// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
	HRESULT CreateInst(LPDIRECT3DDEVICE9 pDevice);
	// �C���X�^���V���O�o�b�t�@�Ƀf�[�^�̑��
	HRESULT SetInst(void);
	// ���_�V�F�[�_�錾�̍쐬
	HRESULT CreateDecl(LPDIRECT3DDEVICE9 pDevice);
};

class BulletData
{
public:
	int		nIdx;
	float	fSize;
	bool	bUse;
	BulletData()
	{
		nIdx = -1;
		fSize = 0.0f;
		bUse = false;
	}
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

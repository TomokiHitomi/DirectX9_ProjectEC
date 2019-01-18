//=============================================================================
//
// ���ʑ̏��� [octa.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _OCTA_H_
#define _OCTA_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define OCTA_MAX		(50)
#define	OCTA_MODEL			"data/model/octahedral.x"	// �ǂݍ��ރ��f����

#define DEBUGSPHERE_COLOR	(D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f))

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Octa
{
public:
	// �R���X�g���N�^�i�����������j
	Octa(void);
	//�f�X�g���N�^�i�I�������j
	~Octa(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);

	// �g�p��Ԃɂ���
	int Set(float fSize);
	// ���W���Z�b�g
	void SetPos(int nIdx, D3DXVECTOR3 vPos);
	// ���
	void Release(int nIdx);
private:
	LPD3DXEFFECT		pEffect;		// �V�F�[�_�[�֘A
	D3DXMATRIX			m_mtxWorld;		// ���[���h�}�g���N�X
	bool				m_bUse;			// �g�p�t���O

	// �C���X�^���V���O�p
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;		// ���_�o�b�t�@
	LPDIRECT3DVERTEXBUFFER9			m_pInstBuff;	// �C���X�^���X�o�b�t�@
	LPDIRECT3DINDEXBUFFER9			m_pIdxBuff;		// �C���f�b�N�X�o�b�t�@
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		// ���_�錾
	DWORD							m_dwVtxSize;	// ���_�T�C�Y
	DWORD							m_dwNumVtx;		// ���_��
	DWORD							m_dwNumFace;	// �ʐ�
	//D3DXATTRIBUTERANGE*			m_pAttrTable;	// �����e�[�u��
	//DWORD							m_dwNumAttr;	// �����e�[�u����

	// VRAM�p
	typedef struct
	{
		D3DXVECTOR3 vPos;		// ���[���h���W
		float		fSize;		// �T�C�Y
		float		fUse;		// �g�p�t���O�i�V�F�[�_�p�j
		D3DCOLOR	vColor;		// �J���[
	}INSTANCE;

	// RAM�p
	typedef struct : public INSTANCE
	{
		bool bUse;				// �g�p�t���O
	}DATA;

	DATA	m_tData[OCTA_MAX];


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
	HRESULT CreateDecl(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif
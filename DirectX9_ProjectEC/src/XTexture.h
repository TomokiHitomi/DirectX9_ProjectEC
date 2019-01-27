//=============================================================================
//
// 2D�e�N�X�`������ [XTexture.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _XTEXTURE_H_
#define _XTEXTURE_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define XTEXTURE_INIT_SIZE		(100.0f)


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class CXTexture
{
private:


	LPDIRECT3DTEXTURE9			m_pTexture;		// �e�N�X�`���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9		m_pVertex;		// ���_�o�b�t�@�ւ̃|�C���^

	// �V�F�[�_�֘A
	LPD3DXEFFECT				pEffect;

	int							m_nNum;			// �C���X�^���X��
	bool						m_bUse;			// �S�̂̎g�p�t���O

	class XTextureData
	{
	public:
		typedef struct Divide
		{
			int x, y;
		};
		D3DXVECTOR3		vPos;					// �X�N���[�����W
		D3DXVECTOR2		vSize;					// �T�C�Y
		Divide			tDivide;				// ������
		D3DXCOLOR		xColor;					// �J���[
		int				nTexNum;				// �e�N�X�`���i���o�[�iUV�����j
		float			fBaseAngle;				// �W���p�x�i��]�p�j
		float			fRadius;				// ���a�i��]�p�j
		float			fScl;					// �X�P�[��
		float			fRot;					// ���b�g
		bool			bUse;					// �P�̂̎g�p�t���O
		XTextureData()
		{
			// �e�f�[�^�̏�����
			vPos = D3DXVECTOR3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
			vSize = D3DXVECTOR2(XTEXTURE_INIT_SIZE, XTEXTURE_INIT_SIZE);
			tDivide.x = 1;
			tDivide.y = 1;
			xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			nTexNum = 0;
			fBaseAngle = 0.0f;
			fRadius = 0.0f;
			fScl = 1.0f;
			fRot = 0.0f;
			bUse = true;
		}
	};

	// ���_�̍X�V
	HRESULT UpdateVertex(void);

	// ���_�̍쐬
	HRESULT MakeVertex(LPDIRECT3DDEVICE9 pDevice);
	

public:

	// �f�[�^�̃|�C���^
	XTextureData* m_pData;

	// �R���X�g���N�^
	CXTexture();
	// �f�X�g���N�^
	~CXTexture() { Release(); }

	// ����������
	HRESULT Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPass, int nNum);
	// �������
	void Release();
	// �X�V����
	void Update(void);
	// �`�揈��
	void Draw(void);
};
#endif
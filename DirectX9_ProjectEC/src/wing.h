//=============================================================================
//
// �E�B���O���� [wing.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _WING_H_
#define _WING_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "SkinMeshX.h"
#include "wing_feather.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// ���f��
#define WING_MODEL				("data/model/wing/wing.X")

#define WING_POS				(D3DXVECTOR3(0.0f, 0.0f, 2.0f))
#define WING_ROT				(D3DXVECTOR3(2.1f, 0.0f, 0.0f))
#define WING_SCL				(20.0f)

// �A�j���[�V�����X�s�[�h
#define WING_ANIM_SPEED_DEF		(60.0f / 4800.0f)

// �A�j���[�V�����̃u�����h�E�F�C�g
#define WING_ANIM_WEIGHT_DEF	(0.1f)

#define WING_BONE_MAX			(12)

// �A�j���[�V�����t���O
#define WING_ANIM_FRY			(0x00000001)
#define WING_ANIM_FLOAT			(0x00000002)
#define WING_ANIM_BACK			(0x00000004)
#define WING_ANIM_CLOSE			(0x00000008)
//#define WING_ANIM_	0x00000010
//#define WING_ANIM_	0x00000020
//#define WING_ANIM_	0x00000040
//#define WING_ANIM_	0x00000080
//#define WING_ANIM_	0x00000100
//#define WING_ANIM_	0x00000200
//#define WING_ANIM_	0x00001000
//#define WING_ANIM_	0x00002000

// SE
#define WING_SE_FLAG_MARGIN		(0.1f)
#define WING_SE_FLOAT			(0.5f)
#define WING_SE_FLY1			(0.4f)
#define WING_SE_FLY2			(1.2f)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Wing
{
private:
	CSkinMesh*		m_CSkinMesh;					// �X�L�����b�V���i�[�p
	D3DXMATRIX		m_mtxWorld;						// ���[���h�}�g���N�X
	D3DXMATRIX*		m_pSetMtx;						// �ݒu��}�g���N�X
	D3DXMATRIX*		m_pBoneMtxArray[WING_BONE_MAX];	// �{�[���}�g���N�X

	bool			m_bUse;							// �g�p�t���O
	DWORD			m_dwAnim;

	WingFeather		m_cWingFeader;
	int				m_nWingFeaderCount;
	int				m_nWingFeaderTime;

	bool			m_bSe;

public:
	// �R���X�g���N�^�i�����������j
	Wing(void);
	//�f�X�g���N�^�i��������j
	~Wing(void);
	// �X�V����
	void Update(void);
	// �`�揈��
	void Draw(void);

	// �A�j���[�V�����ݒ菈��
	void SetAnim(DWORD dwFlag) { m_dwAnim |= dwFlag; }
	void SetMtx(D3DXMATRIX* pSetMtx) { m_pSetMtx = pSetMtx; }
private:
	enum WingAnim
	{	// �A�j���[�V�����Z�b�g�͒ǉ��̋t��
		WING_CLOSE,
		WING_BACK,
		WING_FLOAT,
		WING_FLY,
		WING_ANIM_MAX
	};
	void ChangeAnim(DWORD dwAnime, FLOAT fShift)	{ if (m_CSkinMesh != NULL) m_CSkinMesh->ChangeAnim(dwAnime, fShift); }
	void ChangeAnimSpeed(FLOAT AnimSpeed)			{ if (m_CSkinMesh != NULL) m_CSkinMesh->SetAnimSpeed(AnimSpeed); }
	void CheckAnim(void);

	// Se�ݒ菈��
	void Se(void);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

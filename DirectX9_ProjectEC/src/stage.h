//=============================================================================
//
// �X�e�[�W���� [stage.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _STAGE_H_
#define _STAGE_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "object.h"
#include "skydome.h"
#include "cloud.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************

class Stage : public ObjectManager
{
private:
	bool			m_bUse;				// �g�p�t���O
	Skydome*		pSkydome;
	Cloud*			pCloud;
public:
	enum STAGE
	{	// �v���C���[�Ǘ�
		STAGE_MAIN,
		STAGE_MAX
	};

	// �R���X�g���N�^�i�����������j
	Stage(void);
	//�f�X�g���N�^�i�I�������j
	~Stage(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif
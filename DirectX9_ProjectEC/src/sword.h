//=============================================================================
//
// �\�[�h���� [sword.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _SWORD_H_
#define _SWORD_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "object.h"
#include "XModel.h"
#include "debugobject.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// ���f��
//#define SWORD_MODEL					("data/model/sword/Espada.x")
#define SWORD_MODEL					("data/model/sword/Anelace.x")
#define SWORD_MODEL_TEX				("Espada_AlbedoTransparency.png")

#define SWORD_MAX					(50)
#define SWORD_MAX_INIT				(8)

#define SWORD_SCL					(3.0f)
#define SWORD_POS_Y					(20.0f)
#define SWORD_POS_Y_FLY				(8.0f)
#define SWORD_POS_Y_GUARD			(35.0f)

#define SWORD_SPEED_MIN				(8.0f)
#define SWORD_SPEED_MAX				(20.0f)
#define SWORD_SPEED_RETRUN			(20.0f)
#define SWORD_SPEED_ADD				(5)


//#define SWORD_ROT_SPEED			(0.02f)
#define SWORD_ROT_SPEED_MIN			(-0.1f)
#define SWORD_ROT_SPEED_COUNT		(30)
#define SWORD_ROT_SPEED_MAX			(0.02f)
#define SWORD_ROT_SPEED_ADD			(20)
#define SWORD_ROT_SPEED_SUB			(3)

#define SWORD_ROT_LENGTH			(20.0f)
#define SWORD_ROT_LENGTH_FLY		(30.0f)
#define SWORD_ROT_ADJUST_TIME		(15)

#define SWORD_TRUN					(0.1f)
#define SWORD_LENGTH_RETRUN			(200.0f)
#define SWORD_LENGTH_RETRUN_SUB		(3.5f)

// ���̌����ƓW�J���a�̕ω��X�s�[�h
#define SWORD_MORPH_SPEED			(0.05f)

#define SWORD_HAND_ROT				(D3DXVECTOR3(1.5f,0.9f,0.0f))

#define SWORD_COLLISION_SIZE		(10.0f)
#define SWORD_COLLISION_POS			(10)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Sword
{
public:

	// �R���X�g���N�^�i�����������j
	Sword(void);
	//�f�X�g���N�^�i�I�������j
	~Sword(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);
	void SetMtx(D3DXMATRIX* pSetMtx) { m_pSetMtx = pSetMtx; }
private:
	CXModel			*m_CXModel;			// ���f���i�[�p
	class SwordData
	{
	public:
		enum Mode
		{
			STAY,
			SHOT,
			RETURN
		};
		D3DXVECTOR3			vPos;			// ���W���

		// ���[�J���x�N�g��
		D3DXVECTOR3			vX;
		D3DXVECTOR3			vY;
		D3DXVECTOR3			vZ;
		SwordData::Mode		eMode;
		float				fSpeed;
		float				fLength;
		float				fAngle;
		bool				bUse;
#ifdef _DEBUG
		DebugObjectData m_cDebug;
#endif
	};

	enum Mode
	{
		SHOT,
		ROT,
		RETURN
	};
	Sword::Mode		m_eMode;

	// ���̏��F[SWORD_MAX]�͎������p
	SwordData		m_cSword[SWORD_MAX + 1];
	D3DXMATRIX		m_mtxSword;
	D3DXMATRIX		*m_pSetMtx;			// �ݒu��}�g���N�X
	D3DXVECTOR3		m_vScl;				// �g�k���
	int				m_nMax;


	int				m_nCnt;
	int				m_nCntMax;
	int				m_nCntRot;

	float			m_fAngle;
	float			m_fRot;
	float			m_fRotSpeed;

	// ���[�t
	float			m_fMorph;
	float			m_fMorphGuard;

	bool			m_bUse;

	void	Stay(void);
	void	StayAll(void);
	void	Rot(void);
public:
	void	Shot(void);
	void	RetrunAll(void);
	void	Add(void);
	void	Sub(void);
};

	//Sword	m_cSword;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

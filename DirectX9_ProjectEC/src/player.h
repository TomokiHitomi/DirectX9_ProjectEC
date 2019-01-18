//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "object.h"
#include "SkinMeshX.h"
#include "XModel.h"
#include "XTexture.h"
#include "sword.h"
#include "wing.h"

// �f�o�b�O�p
#ifdef _DEBUG
#include "debugobject.h"
#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// ���f��
#define PLAYER_MODEL				("data/model/haku/haku.X")

#define PLAYER_POS					(D3DXVECTOR3(0.0f,250.0f,500.0f))
#define PLAYER_ROT					(D3DXVECTOR3(D3DX_PI / 2, 0.0f, 0.0f))
#define PLAYER_SCL					(20.0f)

#define PLAYER_HEIGHT				(30.0f)

#define PLAYER_COLLISION_Y			(20.0f)
#define PLAYER_COLLISION_POS		(D3DXVECTOR3(0.0f, PLAYER_COLLISION_Y, 0.0f))
#define PLAYER_COLLISION_SIZE		(30.0f)

// ���f���̃{�[����
#define PLAYER_MODEL_BONE_WING			("No_12_joint_Torso2")
#define PLAYER_MODEL_BONE_RIGHT_HAND	("No_59_i_joint_RightDummy")

// �ړ��X�s�[�h
#define PLAYER_MOVE_SPEED			(0.05f)
#define PLAYER_MOVE_SPEED_MAX		(10.0f)
#define PLAYER_MOVE_SPEED_MIN		(3.0f)

#define PLAYER_ROT_SPEED_X			(0.0005f)
#define PLAYER_ROT_SPEED_Z			(0.0015f)

#define PLAYER_ROT_SPEED_MAX_X		(0.02f)
#define PLAYER_ROT_SPEED_MAX_Z		(0.03f)


#define PLAYER_MOVE_INERTIA			(0.3f)
#define PLAYER_ROT_INERTIA			(0.3f)

#define PLAYER_ALPHA_TEST			(150)

// �ړ�����
#define PLAYER_HEIGHT_MAX			(4500.0f)
#define PLAYER_HEIGHT_MIN			(0.0f)

/***** FloatMode *****/
#define PLAYER_FLOAT_ROT_X			(D3DX_PI * 0.5f)
#define PLAYER_FLOAT_ROT_Y			(D3DX_PI)

/***** FlyMode *****/

/***** ChangeMode *****/
#define PLAYER_CHANGE_FRAME			(10)

// �e�X�g�p
#define PLAYER_GYRO_MARGIN			(300.0f)

// �A�j���[�V�����t���O
#define PLAYER_ANIM_FRY					(0x00000001)
#define PLAYER_ANIM_FLOAT				(0x00000002)
#define PLAYER_ANIM_ATK_HORIZON			(0x00000004)
#define PLAYER_ANIM_ATK_THRUST			(0x00000008)
#define PLAYER_ANIM_ATK_ROUNDUP			(0x00000010)
//#define PLAYER_ANIM_	(0x00000020)
//#define PLAYER_ANIM_	(0x00000040)
//#define PLAYER_ANIM_	(0x00000080)
//#define PLAYER_ANIM_	(0x00000100)
//#define PLAYER_ANIM_	(0x00000200)
//#define PLAYER_ANIM_	(0x00001000)
//#define PLAYER_ANIM_	(0x00002000)
#define PLAYER_ANIM_DUMMY				(0x80000000)

// �A�^�b�N
#define PLAYER_ATK_COMBOTIME_HORIZON	(12 *2)
#define PLAYER_ATK_COMBOTIME_THRUST		(9*2)
#define PLAYER_ATK_COMBOTIME_ROUNDUP	(25*2)
#define PLAYER_ATK_COMBO_MAX			(3)


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************

class Player
{
public:
	enum MODE
	{	// �v���C���[�̃��[�h�Ǘ�
		MODE_FLOAT,
		MODE_FLY,
		MODE_LOCKON,
		MODE_CHANGE,
		MODE_MAX
	};
	enum MODEL_ANIM
	{
		CHARACTER,
		WING,
		MODEL_ANIM_MAX
	};
	enum MODEL_NORMAL
	{
		SWORD,
		MODEL_NORMAL_MAX
	};
	CSkinMesh		*m_CSkinMesh;	// �X�L�����b�V���i�[�p
	//enum PLAYER
	//{	// �v���C���[�Ǘ�
	//	PLAYER_1P,
	//	PLAYER_MAX
	//};

	// �R���X�g���N�^�i�����������j
	Player(void);
	//�f�X�g���N�^�i�I�������j
	~Player(void);

	// ����������
	void	Init(void);
	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);

	// �t�@�C���m�F����
	void	CheckFile(void);
	// �t�@�C���ǂݍ��ݏ���
	bool	LoadFile(void);
	// �t�@�C���������ݏ���
	void	WriteFile(void);

private:
	//typedef struct
	//{
	D3DXVECTOR3		m_vPos;				// ���W���
	D3DXVECTOR3		m_vRot;				// ��]���
	D3DXVECTOR3		m_vScl;				// �g�k���
	D3DXVECTOR3		m_vMove;			// �ړ��ʏ��
	D3DXVECTOR3		m_vRotIner;			// ��]���
	D3DXVECTOR3		m_vRotChange;		// ��]���
	D3DXVECTOR3		m_vUp;

	D3DXVECTOR3		m_vTarget;			// �^�[�Q�b�g���W

	// ���[�J���x�N�g��
	D3DXVECTOR3		m_vX;
	D3DXVECTOR3		m_vY;
	D3DXVECTOR3		m_vZ;

	MODE			m_eMode;
	MODE			m_eModeNext;
	MODE			m_eModeOld;

	float			m_fMoveSpeed;

	//}Prop;

	//Prop m_sProp;

	int				m_nCount;			// �ėp�J�E���^�[
	float			m_fRiseSpeed;
	bool			m_bUse;				// �g�p�t���O



	D3DXMATRIX		m_mtxWorld;				// ���[���h�}�g���N�X
	D3DXMATRIX*		m_pMtxTorso;			// ���[���h�}�g���N�X



#ifdef _DEBUG
	DebugObjectData m_cDebug;
#endif

	// �A�j���[�V����
	enum PlayerAnim
	{	// �A�j���[�V�����Z�b�g�͒ǉ��̋t��
		PLAYER_ATK_ROUNDUP,
		PLAYER_ATK_THRUST,
		PLAYER_ATK_HORIZON,
		PLAYER_FLOAT,
		PLAYER_FLY,
		PLAYER_ANIM_MAX
	};
	DWORD			m_dwAnim;
	// �A�j���[�V�����ݒ菈��
	void SetAnim(DWORD dwFlag) { m_dwAnim |= dwFlag; }
	void ChangeAnim(DWORD dwAnime, FLOAT fShift) { if (m_CSkinMesh != NULL) m_CSkinMesh->ChangeAnim(dwAnime, fShift); }
	void ChangeAnimSpeed(FLOAT AnimSpeed) { if (m_CSkinMesh != NULL) m_CSkinMesh->SetAnimSpeed(AnimSpeed); }
	void CheckAnim(void);


	// �A�^�b�N
	int			m_nComboTime;
	int			m_nComboCount;
	void Attack(void);



	void Float(void);
	void Fly(void);
	void Lockon(void);
	void Change(void);
	void ModeChange(void);
	void Move(void);
	void MoveFunc(float);
	void RotFunc(D3DXVECTOR3);
	void SetCamera(void);

	Wing	*m_cWing;
	Sword	*m_cSword;
public:
	D3DXMATRIX GetMtx(void) { return m_mtxWorld; }
	D3DXVECTOR3 GetVecX(void) { return m_vX; }
	D3DXVECTOR3 GetVecY(void) { return m_vY; }
	D3DXVECTOR3 GetVecZ(void) { return m_vZ; }
	D3DXVECTOR3 GetPos(void) { return m_vPos; }
	D3DXVECTOR3 GetPosTaget(void) { return m_vTarget; }
	MODE GetMode(void) { return m_eMode; }
};

class PlayerManager : public ObjectManager
{
public:
	enum PLAYER
	{	// �v���C���[�Ǘ�
		PLAYER_1P,
		PLAYER_MAX
	};
	static Player		*m_pPlayer[PLAYER_MAX];

public:
	// �R���X�g���N�^�i�����������j
	PlayerManager(void);
	//�f�X�g���N�^�i�I�������j
	~PlayerManager(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);

	static void Reset(void);
	static Player *GetPlayer(PLAYER player) { return m_pPlayer[player]; }
	static D3DXVECTOR3 GetPos(PLAYER player) { return m_pPlayer[player]->GetPos(); }
	static D3DXMATRIX GetMtx(PLAYER player) { return m_pPlayer[player]->GetMtx(); }
	static Player::MODE GetMode(PLAYER player) { return m_pPlayer[player]->GetMode(); }
	static D3DXVECTOR3 GetVecX(PLAYER player) { return m_pPlayer[player]->GetVecX(); }
	static D3DXVECTOR3 GetVecY(PLAYER player) { return m_pPlayer[player]->GetVecY(); }
	static D3DXVECTOR3 GetVecZ(PLAYER player) { return m_pPlayer[player]->GetVecZ(); }
	static D3DXVECTOR3 GetPosTaget(PLAYER player) { return m_pPlayer[player]->GetPosTaget(); }
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

//=============================================================================
//
// �E�B���O���� [wing.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "wing.h"
#include "calculate.h"
#include "sound.h"
#include "input.h"

// �f�o�b�O�p
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPSTR g_lpWingBone[WING_BONE_MAX] = {
	(LPSTR)"No_5_joint_migiwabone1",
	(LPSTR)"No_6_joint_migiwabone2",
	(LPSTR)"No_7_joint_migiwabone3",
	(LPSTR)"No_8_joint_migiwabone4",
	(LPSTR)"No_9_joint_migiwabone5",
	(LPSTR)"No_10_joint_migiwabone6",
	(LPSTR)"No_11_joint_hidariwabone1",
	(LPSTR)"No_12_joint_hidariwabone2",
	(LPSTR)"No_13_joint_hidariwabone3",
	(LPSTR)"No_14_joint_hidariwabone4",
	(LPSTR)"No_15_joint_hidariwabone5",
	(LPSTR)"No_16_joint_hidariwabone6"
};

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
Wing::Wing(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �X�L�����b�V���̏�����
	m_CSkinMesh = new CSkinMesh;
	// �X�L�����b�V���Ƀ��f����ݒ�
	m_CSkinMesh->Init(pDevice, (LPSTR)WING_MODEL);
	ChangeAnimSpeed(WING_ANIM_SPEED_DEF);
	// �E�B���O�ݒu�p�̃}�g���N�X��������
	m_pSetMtx = NULL;


	// �H���ݒu�J�E���g�̏�����
	m_nWingFeaderCount = 0;
	// �H���ݒu�^�C���̏�����
	m_nWingFeaderTime = 0;

	for (UINT i = 0; i < WING_BONE_MAX; i++)
	{
		// �{�[���̃}�g���N�X��������
		m_pBoneMtxArray[i] = NULL;
		// �{�[���}�g���N�X�̃A�h���X���擾
		m_pBoneMtxArray[i] = m_CSkinMesh->GetBoneMatrixAdr(g_lpWingBone[i]);
	}

	// ���[�J�������쐬
	D3DXVECTOR3 pos, rot, scl;
	pos = WING_POS;
	rot = WING_ROT;
	scl = D3DXVECTOR3(WING_SCL, WING_SCL, WING_SCL);

	// ���[���h�s����쐬
	WorldConvert(&m_mtxWorld, pos, rot, scl);
}

//=============================================================================
// �f�X�g���N�^�i��������j
//=============================================================================
Wing::~Wing(void)
{
	SAFE_RELEASE(m_CSkinMesh);
	SAFE_DELETE(m_CSkinMesh);
}

//=============================================================================
// �X�V����
//=============================================================================
void Wing::Update(void)
{
	// �A�j���[�V�����̕ύX���m�F
	CheckAnim();
	// �X�L�����b�V���̃A�b�v�f�[�g
	m_CSkinMesh->Update();

	// ���t���[�����ƂɃt�F�U�[��ݒu
	if (m_nWingFeaderTime % WING_FEATHER_SET_TIME == 0)
	{
		D3DXVECTOR3 temp;
		// �E�B���O�̃{�[���}�g���N�X������W���擾
		temp.x = (*m_pBoneMtxArray[m_nWingFeaderCount])._41;
		temp.y = (*m_pBoneMtxArray[m_nWingFeaderCount])._42;
		temp.z = (*m_pBoneMtxArray[m_nWingFeaderCount])._43;
		// �t�F�U�[��ݒu
		m_cWingFeader.Set(temp);
		// �{�[���̎Q�ƃJ�E���^�����Z
		m_nWingFeaderCount++;
		if (m_nWingFeaderCount >= WING_BONE_MAX) m_nWingFeaderCount = 0;
	}
	// �t�F�U�[�^�C�������Z
	m_nWingFeaderTime++;
	// �t�F�U�[���X�V
	m_cWingFeader.Update();
}

//=============================================================================
// �`�揈��
//=============================================================================
void Wing::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxTemp;		// �A�j���[�V�����X�V����

	if (m_pSetMtx != NULL)
	{
		// �ݒu��}�g���N�X���R�s�[
		mtxTemp = *m_pSetMtx;

		// �}�g���N�X�𐳋K��
		D3DXMatrixNormalize(&mtxTemp, &mtxTemp);
	}
	else D3DXMatrixIdentity(&mtxTemp);

	// ���f���s��ɃZ�b�g�s���������
	D3DXMatrixMultiply(&mtxTemp, &m_mtxWorld, &mtxTemp);

	// �Z�b�g�s���n���ĕ`��
	m_CSkinMesh->Draw(pDevice, mtxTemp);

	m_cWingFeader.Draw();
}

//=============================================================================
// �A�j���[�V�����m�F
//=============================================================================
void Wing::CheckAnim(void)
{
	// �D�揇�ʂ��Ƃ� if ����
	if		(WING_ANIM_FRY & m_dwAnim)		ChangeAnim(WING_FLY, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_FLOAT & m_dwAnim)	ChangeAnim(WING_FLOAT, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_BACK & m_dwAnim)		ChangeAnim(WING_BACK, WING_ANIM_WEIGHT_DEF);
	else if	(WING_ANIM_CLOSE & m_dwAnim)	ChangeAnim(WING_CLOSE, WING_ANIM_WEIGHT_DEF);

	// �A�j���[�V�����r�b�g�p�^�[����������
	m_dwAnim = 0x00000000l;
}
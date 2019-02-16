//=============================================================================
//
// �v���C���[�@�\���� [playerfunc.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "calculate.h"
#include "input.h"
#include "camera.h"
#include "sound.h"
#include "file.h"
#include "scene.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

//=============================================================================
// �U������
//=============================================================================
void Player::Attack(void)
{
	//m_eMode = MODE_LOCKON;
	if (!m_bGuard)
	{
		if (!m_bChargeAttack)
		{
			if (IsMobUseLeftPressed())
			{
				m_nCharge++;
				m_bCharge = true;
				if (m_nCharge == PLAYER_ATK_CHARGE)
				{
					// �`���[�W����
				}
			}
			else
			{
				if (m_nCharge > PLAYER_ATK_CHARGE)
				{
					m_bChargeAttack = true;
				}
				m_nCharge = 0;
				m_bCharge = false;
			}

			if (IsMobUseLeftTriggered() && !IsMobUseRightPressed())
			{
				if (m_eMode == MODE_FLY)
					m_eMode = MODE_FLOAT;

				m_bAttack = true;
				m_nComboCount++;
				if (m_nComboCount > PLAYER_ATK_COMBO_MAX) m_nComboCount = PLAYER_ATK_COMBO_MAX;
			}

			if (m_cSword != NULL)
			{
				// �\�[�h�ˏo
				if (IsMobUseRightTriggered()) m_cSword->Shot();
			}
		}
		else
		{
			m_nCharge++;
			if(m_nCharge <= PLAYER_ATK_CHARGE_TIME)
			{
				SetAnim(PLAYER_ANIM_ATK_RICARD);
			}
			else
			{
				m_bChargeAttack = false;
				m_nCharge = 0;
			}
		}
	}
	else
	{
		m_nCharge = 0;
	}

	// �\�[�h���
	if (IsMobUseRightPressed()) { if (IsMobUseLeftTriggered())m_cSword->RetrunAll(); }


#ifdef _DEBUG
	if (m_cSword != NULL)
	{
		// �e�X�g�p�\�[�h�ǉ�
		if (GetKeyboardTrigger(DIK_E)) m_cSword->Add();
		else if (GetKeyboardTrigger(DIK_Q))m_cSword->Sub();
	}
#endif

	if (m_nComboCount > 0)
	{
		m_nComboTime++;
		DWORD dwAnim = 0x00000000l;
		int nComboTimeMax = 0;
		for (UINT i = 0; i < m_nComboCount; i++)
		{
			switch (i)
			{
			case 0:
				dwAnim |= PLAYER_ANIM_ATK_HORIZON;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_HORIZON;
				break;
			case 1:
				if (m_nComboTime > nComboTimeMax)
				dwAnim |= PLAYER_ANIM_ATK_THRUST;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_THRUST;
				break;
			case 2:
				if (m_nComboTime > nComboTimeMax)
				dwAnim |= PLAYER_ANIM_ATK_ROUNDUP;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_ROUNDUP;
				break;
			}
		}
		if (m_nComboTime <= nComboTimeMax)
		{
			switch (m_nComboTime)
			{
			case PLAYER_ATK_VOICE_COMBOTIME_HORIZON:
				SetVoice(VOICE_ATK1, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case PLAYER_ATK_VOICE_COMBOTIME_THRUST:
				SetVoice(VOICE_ATK2, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case PLAYER_ATK_VOICE_COMBOTIME_ROUNDUP:
				SetVoice(VOICE_ATK4, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			}
			SetAnim(dwAnim);
		}
		else
		{
			m_nComboCount = 0;
			m_nComboTime = 0;
			m_bAttack = false;
		}
	}
}

//=============================================================================
// �h�䏈��
//=============================================================================
void Player::Guard(void)
{
	if (!m_bAttack && !m_bChargeAttack)
	{
		if (GetKeyboardPress(DIK_G))
		{
			if (m_eMode == MODE_FLY)
				m_eMode = MODE_FLOAT;

			if (!m_bGuard)
			{
				SetVoice(VOICE_GUARD1, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				m_bGuard = true;
			}

			// �h��A�j���[�V�������Z�b�g
			SetAnim(PLAYER_ANIM_GUARD);
		}
		else
		{
			m_bGuard = false;
		}
	}
}

//=============================================================================
// ��s����
//=============================================================================
void Player::Fly(void)
{
	// ��s�A�j���[�V�������Z�b�g
	SetAnim(PLAYER_ANIM_FRY);
	// �E�B���O�̃A�j���[�V�������Z�b�g
	m_cWing->SetAnim(WING_ANIM_FRY);

	// �s�b�`
	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R))
	{	// ���������͊������~�߂�
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}
	else if (InputPress(INPUT_DOWN_R))
	{	// �s�b�`�p�x�������ŉ��Z
		m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, PLAYER_ROT_SPEED_MAX_X);
	}
	else if (InputPress(INPUT_UP_R))
	{	// �s�b�`�p�x�������Ō��Z
		m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, -PLAYER_ROT_SPEED_MAX_X);
	}
	else
	{	// ���͂Ȃ��͊������~�߂�
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}

	// ���[��
	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R))
	{	// ���������͊������~�߂�
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}
	else if (InputPress(INPUT_RIGHT_R))
	{	// ���[���p�x�������ŉ��Z
		m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, PLAYER_ROT_SPEED_MAX_Z);
	}
	else if (InputPress(INPUT_LEFT_R))
	{	// ���[���p�x�������Ō��Z
		m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, -PLAYER_ROT_SPEED_MAX_Z);
	}
	else
	{	// ���͂Ȃ��͊������~�߂�
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}

	m_vRotIner.x = GetMobUseY() * 0.00006f;
	m_vRotIner.z = GetMobUseX() * 0.00006f;
	m_vRot += m_vRotIner;

	// ���[����]���v�Z���A�s�b�`��]�p�̎������߂�
	QuaternionCalculate(&m_vX, &m_vZ, m_vRot.z, &m_vX);

	// �s�b�`��]���v�Z���A���[����]�p�̎������߂�
	QuaternionCalculate(&m_vZ, &m_vX, m_vRot.x, &m_vZ);

	// ���[�J��Y���̃x�N�g�����O�ςŋ��߂�
	CrossProduct(&m_vY, &m_vZ, &m_vX);

	// �ړ��ʂ��Z�o
	m_vMove = -m_vZ * m_fMoveSpeed;

	m_vUp = m_vY;

	// �X����������
	SlopeRestore();
}

//=============================================================================
// ���V����
//=============================================================================
void Player::Float(void)
{
	// ���V�A�j���[�V�������Z�b�g
	SetAnim(PLAYER_ANIM_FLOAT);
	// �E�B���O�̃A�j���[�V�������Z�b�g
	m_cWing->SetAnim(WING_ANIM_FLOAT);

	// Y�x�N�g����UpY�ɕ␳
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)) { /* ������������ */ }
	else if (InputPress(INPUT_UP_R))
	{
		m_vRotIner.x += 0.05f;
	}
	else if (InputPress(INPUT_DOWN_R))
	{
		m_vRotIner.x -= 0.05f;
	}

	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R)) { /* ������������ */ }
	else if (InputPress(INPUT_LEFT_R))
	{
		m_vRotIner.y += 0.05f;
	}
	else if (InputPress(INPUT_RIGHT_R))
	{
		m_vRotIner.y -= 0.05f;
	}

	// �}�E�X
	m_vRotIner.x -= GetMobUseY() * 0.001f;
	m_vRotIner.y -= GetMobUseX() * 0.001f;

	m_vRot.x = m_vRot.x + ((m_vRotIner.x - m_vRot.x) * 0.05f);
	m_vRot.y = m_vRot.y + ((m_vRotIner.y - m_vRot.y) * 0.05f);

	D3DXVECTOR3 vTa = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vAxis =
		D3DXVECTOR3(cosf(m_vRot.y), 0.0f, sinf(m_vRot.y));

	QuaternionCalculate(&vTa, &vAxis, m_vRot.x, &vTa);

	m_vZ = vTa;

	// ���[�J��X���̃x�N�g�����O�ςŋ��߂�
	CrossProduct(&m_vX, &m_vY, &m_vZ);
	// ���[�J��X���̃x�N�g���𐳋K��
	D3DXVec3Normalize(&m_vX, &m_vX);

	// �ړ�����
	MoveNormal();
}

//=============================================================================
// ���V����
//=============================================================================
void Player::Lockon(void)
{

	// ���V�A�j���[�V�������Z�b�g
	SetAnim(PLAYER_ANIM_FLOAT);
	// �E�B���O�̃A�j���[�V�������Z�b�g
	m_cWing->SetAnim(WING_ANIM_FLOAT);

	// �x�x�N�g������Ɍ�����
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	// �Ώۍ��W�Ǝ���̍��W�����Z
	D3DXVECTOR3 vLook = m_vPos - m_vTarget;
	//vLook *= -1.0f;

	D3DXVec3Normalize(&vLook, &vLook);
	m_vZ = m_vZ + (vLook - m_vZ) * 0.1f;


	// ���炪�����x�N�g���𐳋K������
	D3DXVec3Normalize(&m_vZ, &m_vZ);

	//m_vZ *= -1.0f;

	// ���[�J��X���̃x�N�g�����O�ςŋ��߂�
	CrossProduct(&m_vX, &m_vY, &m_vZ);

	// X���𐳋K������
	D3DXVec3Normalize(&m_vX, &m_vX);

	// �ړ�����
	MoveNormal();
}

//=============================================================================
// �ړ�����
//=============================================================================
void Player::Change(void)
{
	if (m_nCount < PLAYER_CHANGE_FRAME)
	{
		m_vY += m_vRotChange;
		m_nCount++;
	}
	else
	{
		MatrixRotationVecAndUp(&m_mtxWorld, m_vZ, m_vY);
		m_vX = D3DXVECTOR3(m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13);
		m_vZ = D3DXVECTOR3(m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33);
		m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_eMode = MODE_FLOAT;
	}

	// �J������At�����f���ɐݒ�
	CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	// �J����Eye�����f������ɃZ�b�g
	CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + (m_vY * 30) + m_vZ * 200);

	// �J����Up�����f���㕔�ɐݒ�
	CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);
	//Camera::SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}

//=============================================================================
// �s������
//=============================================================================
void Player::Action(void)
{
	if (m_bAttack || m_bGuard || m_bChargeAttack || m_bCharge || m_bStep)
	{
		m_bAction = true;
	}
	else
	{
		m_bAction = false;
	}

	if (m_bAction)
	{
		// �E�B���O�̃A�j���[�V�������Z�b�g
		m_cWing->SetAnim(WING_ANIM_CLOSE);
		SetLimit(&m_fMagicScl, m_fMagicScl + 2.0f, 20.0f, 0.0f);
	}
	else
	{
		SetLimit(&m_fMagicScl, m_fMagicScl - 2.0f, 20.0f, 0.0f);
	}
}


//=============================================================================
// �ړ�����
//=============================================================================
void Player::UpdateMove(void)
{
	if (GetKeyboardPress(DIK_Q) && GetKeyboardPress(DIK_E))
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}
	else if (GetKeyboardPress(DIK_Q))
	{
		m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED_CHANGE, PLAYER_MOVE_SPEED_MIN);
	}
	else if (GetKeyboardPress(DIK_E))
	{
		m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED_CHANGE, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}

	// �ړ��ʂ�K�p
	m_vPos += m_vMove;

	// �㏸�E���~��K�p
	m_vPos.y += m_fRiseSpeed;

	// �ړ���������
	if (SetLimit(&m_vPos.y, m_vPos.y, PLAYER_HEIGHT_MAX, PLAYER_HEIGHT_MIN))
		m_vMove.y = 0.0f;

#ifdef _DEBUG
	D3DXVECTOR3 vTemp = D3DXVECTOR3(m_pMtxTorso->_41, m_pMtxTorso->_42, m_pMtxTorso->_43);
	vTemp += m_vMove;
	vTemp.y += m_fRiseSpeed;
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, vTemp);
#endif

	// �ړ��ʂɊ�����������
	m_vMove.x += (0.0f - m_vMove.x) * PLAYER_MOVE_INERTIA;
	m_vMove.y += (0.0f - m_vMove.y) * PLAYER_MOVE_INERTIA;
	m_vMove.z += (0.0f - m_vMove.z) * PLAYER_MOVE_INERTIA;
}

//=============================================================================
// �X�e�b�v���͏���
//=============================================================================
void Player::Step(void)
{
	// �X�e�b�v���͓��͂��󂯕t���Ȃ�
	if (!m_bStep)
	{
		// �w��L�[�ŃX�e�b�v�ݒ�
		if (GetKeyboardTrigger(DIK_W))
			SetStep(STEP_FRONT);
		else if (GetKeyboardTrigger(DIK_S))
			SetStep(STEP_BACK);
		else if (GetKeyboardTrigger(DIK_A))
			SetStep(STEP_LEFT);
		else if (GetKeyboardTrigger(DIK_D))
			SetStep(STEP_RIGHT);
	}

	// �X�e�b�v�X�V����
	UpdateStep();
}

//=============================================================================
// �X�e�b�v�ݒ菈��
//=============================================================================
void Player::SetStep(PlayerStep eStep)
{
	// �X�e�b�v�X�e�[�g���Ⴄ�ꍇ
	if (m_eStep != eStep)
	{
		m_nStepCount = PLAYER_STEP_WAIT_TIME;
		m_eStep = eStep;
	}
	// �X�e�b�v�X�e�[�g�����l�̏ꍇ
	else
	{
		// �X�e�b�v
		if (m_nStepCount > 0)
		{
			m_bStep = true;
			// �X�e�b�v���Ԃ��J�E���g�ɑ��
			m_nStepCount = PLAYER_STEP_TIME;
		}
	}
}

//=============================================================================
// �X�e�b�v�X�V����
//=============================================================================
void Player::UpdateStep(void)
{
	if (m_eStep != STEP_NON)
	{
		// �X�e�b�v�� True
		if (m_bStep)
		{
			// �X�e�b�v���� FLY���[�h�̏ꍇ�� FLOAT���[�h�ɐ؂�ւ�
			if (m_eMode == MODE_FLY)
				m_eMode = MODE_FLOAT;

			// �X�e�b�v�A�j���[�V������ݒ�
			switch (m_eStep)
			{
			case STEP_FRONT:
				SetAnim(PLAYER_ANIM_STEP_FLONT);
				break;
			case STEP_BACK:
				SetAnim(PLAYER_ANIM_STEP_BACK);
				break;
			case STEP_LEFT:
				SetAnim(PLAYER_ANIM_STEP_LEFT);
				break;
			case STEP_RIGHT:
				SetAnim(PLAYER_ANIM_STEP_RIGHT);
				break;
			}
			// �X�e�b�v�J�E���g�����Z
			m_nStepCount--;
			// �X�e�b�v�J�E���g�� 0 �����������
			if (m_nStepCount < 0)
			{
				// �X�e�b�v�t���O�� false
				m_bStep = false;
				// �X�e�b�v���Ȃ�
				m_eStep = STEP_NON;
			}
		}
		// �X�e�b�v�� false
		else
		{
			// �X�e�b�v�J�E���g�����Z
			m_nStepCount--;
			// �X�e�b�v�J�E���g�� 0 �����������
			if (m_nStepCount < 0)
			{
				// �X�e�b�v���Ȃ�
				m_eStep = STEP_NON;
			}
		}
	}
}

//=============================================================================
// ���[�h�`�F���W����
//=============================================================================
void Player::ModeChange(void)
{
	if (GetKeyboardTrigger(DIK_1))
	{
		m_eMode = MODE_FLOAT;
	}

	if (GetKeyboardTrigger(DIK_TAB))
	{
		if (m_eMode == MODE_LOCKON)m_eMode = MODE_FLOAT;
		else m_eMode = MODE_LOCKON;
	}

	if (GetKeyboardTrigger(DIK_V))
	{
		if (m_eMode == MODE_FLY)m_eMode = MODE_FLOAT;
		else m_eMode = MODE_FLY;
		// �X�e�b�v������
		m_eStep = STEP_NON;
		m_bStep = false;
		m_nStepCount = 0;
	}

	if (m_eMode != m_eModeOld)
	{
		switch (m_eMode)
		{
		case MODE_FLOAT:
			SetSoundBgm(SOUND_BGM_TEST2);
			//m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X,0.0f,0.0f);
			m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X - atan2(m_vZ.y, 1.0f), -atan2(m_vZ.x, m_vZ.z), 0.0f);
			m_vRotIner = m_vRot;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(0.01f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_FLY:
			SetSoundBgm(SOUND_BGM_TEST1);
			m_vRot = ZERO_D3DXVECTOR3;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(0.05f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_LOCKON:
			SetSoundBgm(SOUND_BGM_TEST2);
			m_vRot = ZERO_D3DXVECTOR3;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(1.0f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_CHANGE:
			m_vRotChange = (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) / PLAYER_CHANGE_FRAME;
			m_nCount = 0;
			break;
		}
		m_eModeOld = m_eMode;
	}
}

//=============================================================================
// �ړ������֐�
//=============================================================================
void Player::MoveNormal(void)
{
	if (m_bStep)
	{
		// �X�e�b�v�ړ�����
		switch (m_eStep)
		{
		case STEP_FRONT:
			m_vMove = -m_vZ * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_BACK:
			m_vMove = m_vZ * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_LEFT:
			m_vMove = m_vX * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_RIGHT:
			m_vMove = -m_vX * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		}
	}
	else
	{
		// �ړ�����
		if (InputPress(INPUT_LEFT))
		{
			if (InputPress(INPUT_UP))
			{// ���O�ړ�
				m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_DOWN))
			{// ����ړ�
				m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_RIGHT)) {/* ���E���������͏����Ȃ� */ }
			else
			{// ���ړ�
				m_vMove = m_vX * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_RIGHT))
		{
			if (InputPress(INPUT_UP))
			{// �E�O�ړ�
				m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_DOWN))
			{// �E��ړ�
				m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else
			{// �E�ړ�
				m_vMove = -m_vX * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_UP))
		{// �O�ړ�
			if (InputPress(INPUT_DOWN)) {/* �O�㓯�������͏����Ȃ� */ }
			else
			{
				m_vMove = -m_vZ * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_DOWN))
		{// ��ړ�
			m_vMove = m_vZ * m_fMoveSpeed;
		}

		// �㏸�E���~����
		if (GetKeyboardPress(DIK_SPACE) && GetKeyboardPress(DIK_LSHIFT))
		{
			m_fRiseSpeed = max(m_fRiseSpeed - PLAYER_RISE_SPEED, -PLAYER_RISE_SPEED_MAX);
		}
		else if (GetKeyboardPress(DIK_SPACE))
		{
			m_fRiseSpeed = min(m_fRiseSpeed + PLAYER_RISE_SPEED, PLAYER_RISE_SPEED_MAX);
		}
		else
		{
			// �ړ��ʂɊ�����������
			m_fRiseSpeed += (0.0f - m_fRiseSpeed) * PLAYER_MOVE_INERTIA;

			//if (m_fRiseSpeed > 0.0f)
			//	m_fRiseSpeed = max(m_fRiseSpeed - PLAYER_RISE_SPEED_AUTO, 0.0f);
			//else if (m_fRiseSpeed < 0.0f)
			//	m_fRiseSpeed = min(m_fRiseSpeed + PLAYER_RISE_SPEED_AUTO, 0.0f);
		}

		//m_vMove.y += m_fRiseSpeed;
	}

	// �X������
	SlopeNormal();
}

//=============================================================================
// �X�������֐�
//=============================================================================
void Player::SlopeNormal(void)
{
	if (InputPress(INPUT_LEFT) && InputPress(INPUT_RIGHT))
	{
		if (m_vSlopeIner.z > 0.0f)
			m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.z < 0.0f)
			m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);
	}
	else if (InputPress(INPUT_LEFT))
	{// ���ړ�
		m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, -PLAYER_SLOPE_SPEED_MAX);
	}
	else if (InputPress(INPUT_RIGHT))
	{// �E�ړ�
		m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, PLAYER_SLOPE_SPEED_MAX);
	}
	else
	{
		if (m_vSlopeIner.z > 0.0f)
			m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.z < 0.0f)
			m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);
	}

	if (InputPress(INPUT_UP) && InputPress(INPUT_DOWN))
	{
		if (m_vSlopeIner.x > 0.0f)
			m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.x < 0.0f)
			m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);
	}
	if (InputPress(INPUT_UP))
	{// �O�ړ�
		m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, -PLAYER_SLOPE_SPEED_MAX);
	}
	else if (InputPress(INPUT_DOWN))
	{// ��ړ�
		m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, PLAYER_SLOPE_SPEED_MAX);
	}
	else
	{
		if (m_vSlopeIner.x > 0.0f)
			m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.x < 0.0f)
			m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);
	}

	m_vSlope = m_vSlope + ((m_vSlopeIner - m_vSlope) * PLAYER_SLOPE_INERTIA);
}

//=============================================================================
// �X�������֐�
//=============================================================================
void Player::SlopeRestore(void)
{
	if (m_vSlopeIner.z > 0.0f)
		m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
	else if (m_vSlopeIner.z < 0.0f)
		m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);

	if (m_vSlopeIner.x > 0.0f)
		m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
	else if (m_vSlopeIner.x < 0.0f)
		m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);

	m_vSlope = m_vSlope + ((m_vSlopeIner - m_vSlope) * PLAYER_SLOPE_INERTIA);
}

//=============================================================================
// �A�j���[�V�����m�F
//=============================================================================
void Player::CheckAnim(void)
{
	// �D�揇�ʂ��Ƃ� if ����
	if (PLAYER_ANIM_DUMMY & m_dwAnim)				ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_GUARD & m_dwAnim)			ChangeAnim(PLAYER_GUARD, PLAYER_ANIM_WEIGHT_DEF);

	else if (PLAYER_ANIM_STEP_FLONT & m_dwAnim)		ChangeAnim(PLAYER_STEP_FRONT, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_BACK & m_dwAnim)		ChangeAnim(PLAYER_STEP_BACK, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_LEFT & m_dwAnim)		ChangeAnim(PLAYER_STEP_LEFT, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_RIGHT & m_dwAnim)		ChangeAnim(PLAYER_STEP_RIGHT, PLAYER_ANIM_WEIGHT_ATK);

	else if (PLAYER_ANIM_ATK_RICARD & m_dwAnim)		ChangeAnim(PLAYER_ATK_RICARD, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_ATK_ROUNDUP & m_dwAnim)	ChangeAnim(PLAYER_ATK_ROUNDUP, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_THRUST & m_dwAnim)		ChangeAnim(PLAYER_ATK_THRUST, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_HORIZON & m_dwAnim)	ChangeAnim(PLAYER_ATK_HORIZON, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_FLOAT & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_FRY & m_dwAnim)			ChangeAnim(PLAYER_FLY, PLAYER_ANIM_WEIGHT_DEF);

	// �A�j���[�V�����r�b�g�p�^�[����������
	m_dwAnim = 0x00000000l;
}

//=============================================================================
// �t�@�C���m�F����
//=============================================================================
void Player::CheckFile(void)
{
	if (LoadFile()) { return; }
	else
	{
		WriteFile();
		LoadFile();
	}
}

//=============================================================================
// �t�@�C���ǂݍ��ݏ���
//=============================================================================
bool Player::LoadFile(void)
{
	FILE *fp;
	fopen_s(&fp, "data/file/player.dat", "rb");
	if (fp == NULL) { return false; }

	// �����o�ϐ��ɓǂݍ���
	fread(this, sizeof(Player), 1, fp);
	fclose(fp);
}

//=============================================================================
// �t�@�C���������ݏ���
//=============================================================================
void Player::WriteFile(void)
{
	FILE *fp;
	// �o�C�i���������݃��[�h�Ńt�@�C���I�[�v��
	fopen_s(&fp, "data/file/player.dat", "wb");

	// �����o�ϐ�����������
	fwrite(this, sizeof(Player), 1, fp);
	fclose(fp);
}

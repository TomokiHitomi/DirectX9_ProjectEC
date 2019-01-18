//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "calculate.h"
#include "input.h"
#include "camera.h"
#include "cube.h"
#include "sound.h"
#include "file.h"
#include "scene.h"


// �f�o�b�O�p
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_ANIM_SPEED_DEF		(60.0f / 4800.0f)	// �A�j���[�V�����X�s�[�h
#define PLAYER_ANIM_WEIGHT_DEF		(0.1f)				// �A�j���[�V�����̃u�����h�E�F�C�g
#define PLAYER_ANIM_WEIGHT_ATK		(0.3f)				// �A�j���[�V�����̃u�����h�E�F�C�g

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
extern float fGyroX;
extern float fGyroY;
extern float fGyroZ;
//D3DXVECTOR3 testpos,testrot;

Player* PlayerManager::m_pPlayer[PLAYER_MAX] = {};

int nTextIdx = 0;


//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
Player::Player(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//// �I�u�W�F�N�gID�ƃv���C�I���e�B�̐ݒ菈��
	//SetIdAndPriority(ObjectID::PLAYER, Priority::Middle, Priority::Middle);

	// �X�e�[�^�X�̏���������
	Init();

	// ���f���̏�����
	m_CSkinMesh = NULL;
	m_CSkinMesh = SceneManager::ModelMgr->GetCharData(ModelManager::PLAYER);
	//m_CSkinMesh = new CSkinMesh;
	//m_CSkinMesh->Init(pDevice, (LPSTR)PLAYER_MODEL);
	ChangeAnimSpeed(PLAYER_ANIM_SPEED_DEF);

	// �E�B���O�̏�����
	m_cWing = NULL;
	m_cWing = new Wing;
	// �L�����N�^�[�̃{�[���}�g���N�X�̃A�h���X���擾
	m_pMtxTorso = m_CSkinMesh->GetBoneMatrixAdr((LPSTR)PLAYER_MODEL_BONE_WING);
	m_cWing->SetMtx(m_pMtxTorso);

	// �\�[�h�̏�����
	m_cSword = NULL;
	m_cSword = new Sword;
	// �L�����N�^�[�̃{�[���}�g���N�X�̃A�h���X���擾
	m_cSword->SetMtx(m_CSkinMesh->GetBoneMatrixAdr((LPSTR)PLAYER_MODEL_BONE_RIGHT_HAND));
}

//=============================================================================
// ����������
//=============================================================================
void Player::Init(void)
{
	// �e�v���p�e�B�̏�����
	m_vPos = PLAYER_POS;
	m_vRot = PLAYER_ROT;
	m_vScl = D3DXVECTOR3(PLAYER_SCL, PLAYER_SCL, PLAYER_SCL);

	m_vMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotIner = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_eMode = MODE_FLOAT;
	m_eModeOld = MODE_MAX;	// 1�t���[���ڂ̓��[�h�`�F���W�ɓ��肽��

	m_fMoveSpeed = PLAYER_MOVE_SPEED_MIN;
	m_fRiseSpeed = 0.0f;
	m_bUse = true;

	// �A�^�b�N
	m_nComboTime = 0;
	m_nComboCount = 0;

	// �s��̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �J������At�����f���ɐݒ�
	CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	// �J����Up�����f���㕔�ɐݒ�
	CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);

	// �J����Eye�����f������ɃZ�b�g
	CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + m_vZ * 100);

	//WriteFile();

#ifdef _DEBUG
	m_cDebug.bUse = true;
	m_cDebug.fSize = PLAYER_COLLISION_SIZE;
	m_cDebug.nIdx = DebugObject::pSphere->Set(m_cDebug.fSize);
#endif
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

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
Player::~Player(void)
{
	// �X�L�����b�V���̉��
	//SAFE_RELEASE(m_CSkinMesh);
	//SAFE_DELETE(m_CSkinMesh);

	// �E�B���O�̉��
	SAFE_DELETE(m_cWing);
	// �\�[�h�̉��
	SAFE_DELETE(m_cSword);

#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif
}

//=============================================================================
// �X�V����
//=============================================================================
void Player::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Player");
	if (bGui)
	{
		ImGui::Text("Pos [%f,%f,%f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
		ImGui::Text("Rot [%f,%f,%f]\n", m_vRot.x, m_vRot.y, m_vRot.z);
		ImGui::Text("RotI[%f,%f,%f]\n", m_vRotIner.x, m_vRotIner.y, m_vRotIner.z);
		ImGui::Text("Move[%f,%f,%f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
		ImGui::Text("Spd [%f]\n", m_fMoveSpeed);
		ImGui::Text("vX  [%f,%f,%f]\n", m_vX.x, m_vX.y, m_vX.z);
		ImGui::Text("vY  [%f,%f,%f]\n", m_vY.x, m_vY.y, m_vY.z);
		ImGui::Text("vZ  [%f,%f,%f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
		ImGui::Text("combo[%d] time[%d]\n", m_nComboCount, m_nComboTime);
		ImGui::TreePop();
	}
#endif

	if (m_bUse)
	{

		Attack();	// �U������

		//// ���[�h�J�ڏ���
		//if (m_eMode == MODE_CHANGE)
		//	Change();

		switch (m_eMode)
		{
		case MODE_FLOAT:
			Float();	// ���V����
			break;
		case MODE_FLY:
			Fly();		// ��s����
			break;
		case MODE_LOCKON:
			Lockon();	// �Ə�����
			break;
		}

		Move();		// �ړ�����

		// �J�����ݒ�
		SetCamera();

		// ���[�h�`�F���W
		ModeChange();

		// �A�j���[�V�����`�F�b�N
		CheckAnim();

		// ���[���h�ϊ�
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		// �X�L�����b�V���̍X�V����
		m_CSkinMesh->Update();


		// �E�B���O�̍X�V����
		SAFE_UPDATE(m_cWing);

		// �\�[�h�̍X�V����
		SAFE_UPDATE(m_cSword);
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void Player::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		// ���[���h�ϊ�
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		// ���f����`��-
		m_CSkinMesh->Draw(pDevice, m_mtxWorld);

		// �E�B���O��`��
		//if(m_cWing != NULL) m_cWing->Draw();
		SAFE_DRAW(m_cWing);

		// �\�[�h��`��
		//if (m_cSword != NULL) m_cSword->Draw();
		SAFE_DRAW(m_cSword);

#ifdef _DEBUG
		//PrintDebugProc("Bone  [%d]\n", m_CSkinMesh[CHARACTER]->m_dwBoneCount);
		//PrintDebugProc("Bone  [%d]\n", m_CSkinMesh[WING]->m_dwBoneCount);
#endif
	}
}

//=============================================================================
// �U������
//=============================================================================
void Player::Attack(void)
{
	//m_eMode = MODE_LOCKON;

	if (IsMobUseLeftTriggered() && !IsMobUseRightPressed())
	{
		m_nComboCount++;
		if (m_nComboCount > PLAYER_ATK_COMBO_MAX) m_nComboCount = PLAYER_ATK_COMBO_MAX;
	}


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
			SetAnim(dwAnim);
		}
		else
		{
			m_nComboCount = 0;
			m_nComboTime = 0;
		}
	}

	if (m_cSword != NULL)
	{
		// �\�[�h����
		if (IsMobUseRightTriggered()) m_cSword->Shot();
		else if (IsMobUseRightPressed()) {if (IsMobUseLeftTriggered())m_cSword->RetrunAll();}

		// �e�X�g�p�\�[�h�ǉ�
		if (GetKeyboardTrigger(DIK_E)) m_cSword->Add(); 
		else if (GetKeyboardTrigger(DIK_Q))m_cSword->Sub();
	}

}

//=============================================================================
// �J�����ݒ菈��
//=============================================================================
void Player::SetCamera(void)
{
	Camera* pCamera = CameraManager::GetCameraNow();

	switch (m_eMode)
	{
	case MODE_FLOAT:
		// �J������At�����f���ɐݒ�
		pCamera->SetAt(m_vPos + (m_vY * 30));
		// �J����Eye�����f������ɃZ�b�g
		pCamera->SetEye(m_vPos + (m_vY * 30) + m_vZ * 100);
		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);

		// �J�������������炷
		pCamera->AddAtIner(0.02f);
		pCamera->AddEyeIner(0.02f);
		break;
	case MODE_FLY:
		// �J������At�����f���ɐݒ�
		pCamera->SetAt(m_vPos + (m_vY * 30));
		// �J����Eye�����f������ɃZ�b�g
		pCamera->SetEye(m_vPos + m_vZ * 100);
		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);

		// �J�������������炷
		pCamera->AddAtIner(0.02f);
		break;
	case MODE_LOCKON:
		// �J������At��Ώۂɐݒ�
		pCamera->SetAt(ZERO_D3DXVECTOR3);
		// �J����Eye�����f������ɃZ�b�g
		pCamera->SetEye(m_vPos + (m_vY * 30) + m_vZ * 100);
		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);

		// �J�������������炷
		//pCamera->AddEyeIner(0.05f);
		pCamera->AddAtIner(0.02f);
		break;
	}
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
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f)- m_vY) * 0.1f;

	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)){ /* ������������ */ }
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

	D3DXVECTOR3 vTa = D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXVECTOR3 vAxis =
		D3DXVECTOR3(cosf(m_vRot.y), 0.0f, sinf(m_vRot.y));

	QuaternionCalculate(&vTa, &vAxis, m_vRot.x, &vTa);
	
	m_vZ = vTa;

	// ���[�J��X���̃x�N�g�����O�ςŋ��߂�
	CrossProduct(&m_vX, &m_vY, &m_vZ);
	// ���[�J��X���̃x�N�g���𐳋K��
	D3DXVec3Normalize(&m_vX, &m_vX);

	// �ړ�����
	if (InputPress(INPUT_LEFT))
	{
		if (InputPress(INPUT_UP))
		{// ���O�ړ�
			//MoveFunc(m_fHAngle + D3DX_PI * 0.25f);
			m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// ����ړ�
			//MoveFunc(m_fHAngle + D3DX_PI * 0.75f);
			m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_RIGHT))
		{// ���E���������͏����Ȃ�
		}
		else
		{// ���ړ�
			//MoveFunc(m_fHAngle + D3DX_PI * 0.50f);
			m_vMove = m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_RIGHT))
	{
		if (InputPress(INPUT_UP))
		{// �E�O�ړ�
			//MoveFunc(m_fHAngle - D3DX_PI * 0.25f);
			m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// �E��ړ�
			//MoveFunc(m_fHAngle - D3DX_PI * 0.75f);
			m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else
		{// �E�ړ�
			//MoveFunc(m_fHAngle - D3DX_PI * 0.50f);
			m_vMove = -m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_UP))
	{// �O�ړ�
		if (InputPress(INPUT_DOWN))
		{// �O�㓯�������͏����Ȃ�
		}
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
		m_fRiseSpeed = max(m_fRiseSpeed - 0.5f, -10.0f);
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		m_fRiseSpeed = min(m_fRiseSpeed + 0.5f, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		if (m_fRiseSpeed > 0.0f)
			m_fRiseSpeed = max(m_fRiseSpeed - 0.2f, 0.0f);
		else if (m_fRiseSpeed < 0.0f)
			m_fRiseSpeed = min(m_fRiseSpeed + 0.2f, 0.0f);
	}

	m_vMove.y = m_fRiseSpeed;

	// �ړ���K�p
	//m_vPos += m_vMove;


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
	if ((InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)) || (fGyroY < PLAYER_GYRO_MARGIN && fGyroY > -PLAYER_GYRO_MARGIN))
	{	// ���������͊������~�߂�
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}
	else if (InputPress(INPUT_DOWN_R) || fGyroY > PLAYER_GYRO_MARGIN)
	{	// �s�b�`�p�x�������ŉ��Z
		m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, PLAYER_ROT_SPEED_MAX_X);
	}
	else if (InputPress(INPUT_UP_R) || fGyroY < -PLAYER_GYRO_MARGIN)
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
	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R) || (fGyroZ < PLAYER_GYRO_MARGIN && fGyroZ > -PLAYER_GYRO_MARGIN))
	{	// ���������͊������~�߂�
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}
	else if (InputPress(INPUT_RIGHT_R) || fGyroZ < -PLAYER_GYRO_MARGIN)
	{	// ���[���p�x�������ŉ��Z
		m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, PLAYER_ROT_SPEED_MAX_Z);
	}
	else if (InputPress(INPUT_LEFT_R) || fGyroZ > -PLAYER_GYRO_MARGIN)
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

	//// �ړ��ʂ�K�p
	//m_vPos += m_vMove;


}

//=============================================================================
// ���V����
//=============================================================================
void Player::Lockon(void)
{
	// ���V�A�j���[�V�������Z�b�g
	SetAnim(PLAYER_ANIM_FLOAT);
	// �E�B���O�̃A�j���[�V�������Z�b�g
	m_cWing->SetAnim(WING_ANIM_CLOSE);

	// �x�x�N�g������Ɍ�����
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	// �Ώۍ��W�Ǝ���̍��W�����Z
	D3DXVECTOR3 vLook = ZERO_D3DXVECTOR3 - m_vPos;
	vLook *= -1.0f;

	D3DXVec3Normalize(&vLook, &vLook);
	m_vZ = m_vZ + (vLook - m_vZ) * 0.1f;


	// ���炪�����x�N�g���𐳋K������
	D3DXVec3Normalize(&m_vZ, &m_vZ);

	//m_vZ *= -1.0f;

	// ���[�J��X���̃x�N�g�����O�ςŋ��߂�
	CrossProduct(&m_vX, &m_vY, &m_vZ);

	// X���𐳋K������
	D3DXVec3Normalize(&m_vX, &m_vX);

#ifdef _DEBUG
	PrintDebugProc("�y Float �z\n");
	PrintDebugProc("vX  [%f,%f,%f]\n", m_vX.x, m_vX.y, m_vX.z);
	PrintDebugProc("vZ  [%f,%f,%f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
#endif

	// �ړ�����
	if (InputPress(INPUT_LEFT))
	{
		if (InputPress(INPUT_UP))
		{// ���O�ړ�
			//m_vPos += (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// ����ړ�
			//m_vPos += (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_RIGHT)){/* ���E���������͏����Ȃ� */}
		else
		{// ���ړ�
			//m_vPos += m_vX * m_fMoveSpeed;
			m_vMove = m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_RIGHT))
	{
		if (InputPress(INPUT_UP))
		{// �E�O�ړ�
			//m_vPos -= (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// �E��ړ�
			//m_vPos -= (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else
		{// �E�ړ�
			//m_vPos -= m_vX * m_fMoveSpeed;
			m_vMove = -m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_UP))
	{// �O�ړ�
		if (InputPress(INPUT_DOWN)) {/* �O�㓯�������͏����Ȃ� */}
		else
		{
			//m_vPos += -m_vZ * m_fMoveSpeed;
			m_vMove = -m_vZ * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_DOWN))
	{// ��ړ�
		//m_vPos += m_vZ * m_fMoveSpeed;
		m_vMove = m_vZ * m_fMoveSpeed;
	}

	// �㏸�E���~����
	if (GetKeyboardPress(DIK_SPACE) && GetKeyboardPress(DIK_LSHIFT))
	{
		m_fRiseSpeed = max(m_fRiseSpeed - 0.5f, -10.0f);
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		m_fRiseSpeed = min(m_fRiseSpeed + 0.5f, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		if (m_fRiseSpeed > 0.0f)
			m_fRiseSpeed = max(m_fRiseSpeed - 0.2f, 0.0f);
		else if (m_fRiseSpeed < 0.0f)
			m_fRiseSpeed = min(m_fRiseSpeed + 0.2f, 0.0f);
	}

	m_vMove.y += m_fRiseSpeed;


	// �ړ���K�p
	//m_vPos += m_vMove;


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

	Cube::SetPos(m_vPos - m_vZ * 100);
}


//=============================================================================
// �ړ�����
//=============================================================================
void Player::Move(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 vTemp = D3DXVECTOR3(m_pMtxTorso->_41, m_pMtxTorso->_42, m_pMtxTorso->_43);
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, vTemp + m_vMove);
#endif
	if (GetKeyboardPress(DIK_Q) && GetKeyboardPress(DIK_E))
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}
	else if (GetKeyboardPress(DIK_Q))
	{
		m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, PLAYER_MOVE_SPEED_MIN);
	}
	else if (GetKeyboardPress(DIK_E))
	{
		m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, PLAYER_MOVE_SPEED_MAX);
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

	// �ړ���������
	m_vPos.y = SetLimit(m_vPos.y, PLAYER_HEIGHT_MAX, PLAYER_HEIGHT_MIN);

	// �ړ��ʂɊ�����������
	m_vMove.x += (0.0f - m_vMove.x) * PLAYER_MOVE_INERTIA;
	m_vMove.y += (0.0f - m_vMove.y) * PLAYER_MOVE_INERTIA;
	m_vMove.z += (0.0f - m_vMove.z) * PLAYER_MOVE_INERTIA;
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
	}

	if (m_eMode != m_eModeOld)
	{
		switch (m_eMode)
		{
		case MODE_FLOAT:
			SetSoundBgm(SOUND_BGM_TEST2);
			//m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X,0.0f,0.0f);
			m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X - atan2(m_vZ.y, 1.0f), -atan2(m_vZ.x, m_vZ.z),0.0f);
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
void Player::MoveFunc(float fAngle)
{
	m_vPos.x += cosf(fAngle) * m_fMoveSpeed;
	m_vPos.z += sinf(fAngle) * m_fMoveSpeed;
}

//=============================================================================
// ��]�����֐�
//=============================================================================
void Player::RotFunc(D3DXVECTOR3 vRot)
{
	//m_vPos.x += sinf(fAngle) * PLAYER_MOVE_SPEED;
	//m_vPos.z += cosf(fAngle) * PLAYER_MOVE_SPEED;
}

//=============================================================================
// �A�j���[�V�����m�F
//=============================================================================
void Player::CheckAnim(void)
{
	// �D�揇�ʂ��Ƃ� if ����
	if		(PLAYER_ANIM_DUMMY & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);

	else if	(PLAYER_ANIM_ATK_ROUNDUP & m_dwAnim)	ChangeAnim(PLAYER_ATK_ROUNDUP, PLAYER_ANIM_WEIGHT_ATK);
	else if	(PLAYER_ANIM_ATK_THRUST & m_dwAnim)		ChangeAnim(PLAYER_ATK_THRUST, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_HORIZON & m_dwAnim)	ChangeAnim(PLAYER_ATK_HORIZON, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_FLOAT & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_FRY & m_dwAnim)			ChangeAnim(PLAYER_FLY, PLAYER_ANIM_WEIGHT_DEF);


	// �A�j���[�V�����r�b�g�p�^�[����������
	m_dwAnim = 0x00000000l;
}

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
PlayerManager::PlayerManager(void)
{
	// �I�u�W�F�N�gID�ƃv���C�I���e�B�̐ݒ菈��
	SetIdAndPriority(ObjectID::PLAYERMANAGER, Priority::Middle, Priority::Middle);

	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		// �v���C���[��������
		m_pPlayer[i] = NULL;
		m_pPlayer[i] = new Player;
	}
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
PlayerManager::~PlayerManager(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		delete m_pPlayer[i];
		m_pPlayer[i] = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void PlayerManager::Update(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Update();
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void PlayerManager::Draw(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Draw();
		}
	}
}
//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "calculate.h"
#include "camera.h"
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
// �R���X�g���N�^�i�����������j
//=============================================================================
Player::Player(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

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


	// �G�t�F�N�g�}�l�[�W���[�̃|�C���^���擾
	pEffetMgr = NULL;
	pEffetMgr = ObjectManager::GetObjectPointer<EffectManager>(ObjectManager::EFFECT);
	// �G�t�F�N�g�n���h���̏�����
	m_handle = -1;

	// ���[�v�G�t�F�N�g���Ăяo��
	m_handle = pEffetMgr->Play(EffectManager::EFFECT_MAGICAREA);

	//pEffetMgr->m_manager->StopEffect(m_handle);
	//m_handle = -1;
}

//=============================================================================
// ����������
//=============================================================================
void Player::Init(void)
{
	// �e�v���p�e�B�̏�����
	m_vPos = PLAYER_POS;
	m_vRot = PLAYER_ROT;
	m_vSlope = ZERO_D3DXVECTOR3;
	m_vSlopeIner = m_vSlope;
	m_vScl = D3DXVECTOR3(PLAYER_SCL, PLAYER_SCL, PLAYER_SCL);

	m_vMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotIner = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_vTarget = D3DXVECTOR3(0.0f, 500.0f, 0.0f);

	m_eMode = MODE_FLOAT;
	m_eModeOld = MODE_MAX;	// 1�t���[���ڂ̓��[�h�`�F���W�ɓ��肽��

	m_fMoveSpeed = PLAYER_MOVE_SPEED;
	m_fRiseSpeed = 0.0f;
	m_bUse = true;

	// �A�^�b�N
	m_nComboTime = 0;
	m_nComboCount = 0;
	m_bAttack = false;
	m_nCharge = 0;
	m_bCharge = false;
	m_bChargeAttack = false;

	// �X�e�b�v
	m_eStep = STEP_NON;
	m_bStep = false;
	m_nStepCount = 0;

	// �K�[�h
	m_bGuard = false;

	// �A�N�V����
	m_bAction = false;

	// ���@�w
	m_fMagicScl = 0.0f;

	// �s��̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	//// �J������At�����f���ɐݒ�
	//CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	//// �J����Up�����f���㕔�ɐݒ�
	//CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);

	//// �J����Eye�����f������ɃZ�b�g
	//CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + m_vZ * 100);

	//WriteFile();

#ifdef _DEBUG
	m_cDebug.bUse = true;
	m_cDebug.fSize = PLAYER_COLLISION_SIZE;
	m_cDebug.nIdx = DebugObject::pSphere->Set(m_cDebug.fSize);
#endif
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
Player::~Player(void)
{
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
		ImGui::Text("Pos [%6.0f.%6.0f,%6.0f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
		ImGui::Text("Rot [%2.2f.%2.2f,%2.2f]\n", m_vRot.x, m_vRot.y, m_vRot.z);
		ImGui::Text("RotI[%2.2f.%2.2f,%2.2f]\n", m_vRotIner.x, m_vRotIner.y, m_vRotIner.z);
		ImGui::Text("Move[%2.2f.%2.2f,%2.2f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
		ImGui::Text("Spd [%f]  Rise[%f]\n", m_fMoveSpeed, m_fRiseSpeed);
		ImGui::Text("vX  [%.2f.%.2f,%.2f]\n", m_vX.x, m_vX.y, m_vX.z);
		ImGui::Text("vY  [%.2f.%.2f,%.2f]\n", m_vY.x, m_vY.y, m_vY.z);
		ImGui::Text("vZ  [%.2f.%.2f,%.2f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
		ImGui::Text("combo[%d] time[%d]\n", m_nComboCount, m_nComboTime);
		ImGui::InputFloat3("Target", &m_vTarget.x,2);
		ImGui::InputFloat3("Slope", &m_vSlope.x, 2);
		ImGui::InputFloat("scl", &m_fMagicScl);
		ImGui::TreePop();
	}
#endif

	if (m_bUse)
	{
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

		Action();		// �s������

		Attack();		// �U������
		Guard();		// �h�䏈��

		UpdateMove();	// �ړ�����
		Step();			// �X�e�b�v����

		// �J�����ݒ�
		SetCamera();

		// ���[�h�`�F���W
		ModeChange();

		// �A�j���[�V�����`�F�b�N
		CheckAnim();

		// ���[���h�ϊ�
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		D3DXMATRIX mtxSlope;

		// �X���i��]�j������ɓK�p
		D3DXMatrixRotationYawPitchRoll(&mtxSlope, m_vSlope.y, m_vSlope.x, m_vSlope.z);
		D3DXMatrixMultiply(&mtxSlope, &mtxSlope, &m_mtxWorld);


		if (m_handle >= 0)
		{
			pEffetMgr->SetMatrix(m_handle, mtxSlope);
			pEffetMgr->SetScale(m_handle, m_fMagicScl);
		}

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
		//WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		D3DXMATRIX mtxSlope;

		// �X���i��]�j������ɓK�p
		D3DXMatrixRotationYawPitchRoll(&mtxSlope, m_vSlope.y, m_vSlope.x, m_vSlope.z);
		D3DXMatrixMultiply(&mtxSlope, &mtxSlope, &m_mtxWorld);

		// ���f����`��-
		m_CSkinMesh->Draw(pDevice, mtxSlope);

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
// �J�����ݒ菈��
//=============================================================================
void Player::SetCamera(void)
{
	Camera* pCamera = CameraManager::GetCameraNow();
	D3DXVECTOR3 temp;

	D3DXMATRIX mtxTemp;
	D3DXVECTOR3 vX, vY, vZ;

	// �v���C���[�̍s����擾
	mtxTemp = m_mtxWorld;
	// �擾�����s��𐳋K��
	D3DXMatrixNormalize(&mtxTemp, &mtxTemp);

	// �s����̃x�N�g�����擾
	vX = D3DXVECTOR3(mtxTemp._11, mtxTemp._12, mtxTemp._13);
	vY = D3DXVECTOR3(mtxTemp._21, mtxTemp._22, mtxTemp._23);
	vZ = D3DXVECTOR3(mtxTemp._31, mtxTemp._32, mtxTemp._33);


	switch (m_eMode)
	{
	case MODE_FLOAT:
		// �J������At�����f���ɐݒ�
		pCamera->SetAt(m_vPos + (vY * 30) + (-vZ * 100));
		// �J����Eye�����f������ɃZ�b�g
		pCamera->SetEye(m_vPos + (vY * 30) + (vZ * 100));
		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);

		// �J�������������炷
		pCamera->AddAtIner(0.02f);
		pCamera->AddEyeIner(0.02f);
		break;
	case MODE_FLY:
		// �J������At�����f���ɐݒ�
		pCamera->SetAt(m_vPos + (m_vY * 30));
		//pCamera->SetAt(m_vTarget);
		//temp =  m_vPos - m_vTarget;
		//D3DXVec3Normalize(&temp, &temp);

		// �J����Eye�����f������ɃZ�b�g
		pCamera->SetEye(m_vPos + m_vZ * 100);
		//pCamera->SetEye(m_vPos + temp * 100);
		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);
		//pCamera->SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		// �J�������������炷
		pCamera->AddAtIner(0.02f);
		//pCamera->AddEyeIner(0.02f);
		break;
	case MODE_LOCKON:
		// �J������At��Ώۂɐݒ�
		pCamera->SetAt(m_vTarget);
		// �J����Eye�����f������ɃZ�b�g
		//pCamera->SetEye(m_vPos + (vX * 30) + (vY * 30) + (vZ * 100));
		pCamera->SetEye(m_vPos + (vY * 30) + (vZ * 100));

		// �J����Up�����f���㕔�ɐݒ�
		pCamera->SetUp(m_vY);

		// �J�������������炷
		//pCamera->AddEyeIner(0.05f);
		pCamera->AddAtIner(0.02f);
		break;
	}
}
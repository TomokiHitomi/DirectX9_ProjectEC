//=============================================================================
//
// �\�[�h���� [sword.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "sword.h"
#include "calculate.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "cube.h"
#include "collision.h"

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

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
Sword::Sword(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �\�[�h�̏�����
	m_CXModel = new CXModel;
	m_CXModel->Init(pDevice, (LPSTR)SWORD_MODEL, NULL);
	//m_CXModel->SetMaterialSpecular(0.5f, 1.0f);

	m_pSetMtx = NULL;

	m_eMode = Sword::SHOT;
	m_nCnt = 0;
	m_nCntRot = 0;
	m_fRot = 0.01f;
	m_fRotSpeed = SWORD_ROT_SPEED_MAX;
	m_fMorph = 0.0f;
	m_bUse = true;

	for (unsigned int i = 0; i < SWORD_MAX + 1; i++)
	{
		//m_cSword[i].vPos = D3DXVECTOR3(0.0f + i*100.0f, 0.0f + i*100.0f, 500.0f);
		m_cSword[i].vPos = ZERO_D3DXVECTOR3;
		m_cSword[i].vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_cSword[i].vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_cSword[i].vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		m_cSword[i].eMode = SwordData::STAY;
		m_cSword[i].fLength = SWORD_ROT_LENGTH;
		m_cSword[i].fSpeed = 0.0f;
		m_cSword[i].fAngle = 0.0f;
		if (i < SWORD_MAX_INIT)
		{
			m_cSword[i].bUse = true;
			m_nCnt++;
		}
		else
		{
			m_cSword[i].bUse = false;
		}
	}

	m_vScl = D3DXVECTOR3(SWORD_SCL, SWORD_SCL, SWORD_SCL);

	// ��������������
	m_cSword[SWORD_MAX].bUse = true;
	// �������̃��[�J�������쐬
	D3DXVECTOR3 pos, rot;
	pos = ZERO_D3DXVECTOR3;
	rot = SWORD_HAND_ROT;

	// �������̍s����쐬
	WorldConvert(&m_mtxSword, pos, rot, m_vScl);

	m_nCntMax = m_nCnt;
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
Sword::~Sword(void)
{
	if (m_CXModel != NULL)
	{
		m_CXModel->Release();
		delete m_CXModel;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void Sword::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Sword");
	if (bGui)
	{
		ImGui::Text("Cnt[%d]\n", m_nCnt);
	}
#endif

	if (m_bUse)
	{
		// �\�[�h�̍X�V����
		D3DXVECTOR3 temp,posTemp, posTarget,vUp;
		D3DXMATRIX mtxTemp;

		// �e�X�g�p�F�v���C���[�̃|�C���^���擾���āA�㔼�g�Q�̃}�g���N�X���擾
		//Player *pPlayer = ObjectManager::GetObjectPointer<Player>(ObjectManager::PLAYER);
		//mtxTemp = pPlayer->m_CSkinMesh[Player::CHARACTER]->GetBoneMatrix(PLAYER_MODEL_BONE_WING);
		//posTemp = D3DXVECTOR3(mtxTemp._41, mtxTemp._42, mtxTemp._43);

		//posTemp = pPlayer->GetPos();
		//posTemp += pPlayer->GetVecY() * SWORD_POS_Y;
		mtxTemp = PlayerManager::GetMtx(PlayerManager::PLAYER_1P);
		//mtxTemp = pPlayer->GetMtx();
		posTemp = D3DXVECTOR3(mtxTemp._21, mtxTemp._22, mtxTemp._23);
		D3DXVec3Normalize(&posTemp, &posTemp);

		posTemp *= SWORD_POS_Y;
		posTemp += PlayerManager::GetPos(PlayerManager::PLAYER_1P);
		//posTemp += pPlayer->GetPos();

		// �e�X�g�p�F�L���[�u�̃|�C���^���擾
		Cube *pCube = ObjectManager::GetObjectPointer<Cube>(ObjectManager::CUBE);
		posTarget = pCube->GetPos();

		 
		// �v���C���[���[�h�ɉ����ă��[�t�l��ϓ�
		//if (pPlayer->GetMode() == Player::MODE_FLY) m_fMorph += SWORD_MORPH_SPEED;
		if (PlayerManager::GetMode(PlayerManager::PLAYER_1P) == Player::MODE_FLY) m_fMorph += SWORD_MORPH_SPEED;
		else m_fMorph -= SWORD_MORPH_SPEED;

		// 0.0�`1.0f�͈̔͂ŃN�����v
		m_fMorph = saturate(m_fMorph);

		//// �e�X�g�p����
		//if (IsMobUseRightTriggered())
		//{
		//	Shot();
		//}
		//else if (IsMobUseRightPressed())
		//{
		//	if (IsMobUseLeftTriggered())
		//	{
		//		RetrunAll();
		//	}
		//}

		//// �e�X�g�p�\�[�h�ǉ�
		//if (GetKeyboardTrigger(DIK_E))
		//{
		//	Add();
		//}
		//else if (GetKeyboardTrigger(DIK_Q))
		//{
		//	Sub();
		//}

		//else
		//{
		//	//m_eMode = Sword::ROT;
		//	Stay();
		//}
		
		// �\�[�h��]����
		Rot();

		int nCnt = 0;
		m_fAngle = D3DX_PI * 2 / m_nCnt;
		//m_vAxis = pPlayer->GetVecY() * (1.0f - m_fMorph) + pPlayer->GetVecZ() * m_fMorph;
		//m_vAxis =pPlayer->GetVecZ();
		m_vAxis = PlayerManager::GetVecZ(PlayerManager::PLAYER_1P);

		m_vRadian = PlayerManager::GetVecX(PlayerManager::PLAYER_1P);
		CrossProduct(&vUp, &m_vAxis, &m_vRadian);

		// ���[�t���ɉ����ēW�J���a��ω�
		m_fLength = SWORD_ROT_LENGTH * (1.0f - m_fMorph) + SWORD_ROT_LENGTH_FLY * m_fMorph;

		for (unsigned int i = 0; i < m_nCntMax; i++)
		{
			
			switch (m_cSword[i].eMode)
			{
			case SwordData::RETURN:
				m_cSword[i].fAngle += ((m_fAngle * nCnt + m_fRot) - m_cSword[i].fAngle) / SWORD_ROT_ADJUST_TIME;
				QuaternionCalculate(&m_vRadian, &m_vAxis, m_cSword[i].fAngle, &temp);
				if (CheckHitBC(m_cSword[i].vPos, posTemp + temp * m_cSword[i].fLength, 100, 100))
				{
					m_cSword[i].eMode = SwordData::STAY;
					m_cSword[i].fSpeed = 0.0f;
					//break;
				}
				else
				{
					m_cSword[i].fSpeed += (m_fLength - m_cSword[i].fSpeed) / SWORD_SPEED_ADD;
					temp = (posTemp + temp * m_cSword[i].fLength) - m_cSword[i].vPos;
					D3DXVec3Normalize(&temp, &temp);
					m_cSword[i].vPos += temp * m_cSword[i].fSpeed;
				}
				m_cSword[i].vX = vUp;
				m_cSword[i].vZ = temp;
				nCnt++;
				break;
			case SwordData::STAY:
				m_cSword[i].fAngle += ((m_fAngle * nCnt + m_fRot) - m_cSword[i].fAngle) / SWORD_ROT_ADJUST_TIME;
				QuaternionCalculate(&m_vRadian, &m_vAxis, m_cSword[i].fAngle, &temp);

				m_cSword[i].vPos = posTemp + temp * m_cSword[i].fLength;
				m_cSword[i].fLength =
					max(m_fLength, m_cSword[i].fLength - SWORD_LENGTH_RETRUN_SUB);

				// �v���C���[�̃��[�h�Ō��̌�����ς���
				// ����������
				m_cSword[i].vX = ZERO_D3DXVECTOR3;
				m_cSword[i].vZ = ZERO_D3DXVECTOR3;

				// ���[�t�l�ɉ����ăx�N�g����ݒ�
				m_cSword[i].vX += vUp * (1.0f - m_fMorph);
				m_cSword[i].vZ += temp * (1.0f - m_fMorph);

				CrossProduct(&temp, &temp, &m_vAxis);
				m_cSword[i].vX += temp * m_fMorph;
				m_cSword[i].vZ += -m_vAxis * m_fMorph;

				nCnt++;
				break;
			case SwordData::SHOT:
				m_cSword[i].fSpeed += (SWORD_SPEED_MAX - m_cSword[i].fSpeed) / SWORD_SPEED_ADD;

				temp = posTarget - m_cSword[i].vPos;
				D3DXVec3Normalize(&temp, &temp);
				temp = temp * SWORD_TRUN + m_cSword[i].vZ;
				D3DXVec3Normalize(&temp, &temp);
				m_cSword[i].vPos += temp * m_cSword[i].fSpeed;
				m_cSword[i].vZ = temp;
				break;
			}

#ifdef _DEBUG
			if (bGui)
			{
				ImGui::Text("No[%d] Use[%d] Mode[%d] Len[%f] Spd[%f]\n"
					, i, m_cSword[i].bUse, m_cSword[i].eMode,
					m_cSword[i].fLength, m_cSword[i].fSpeed);
			}
#endif
		}
	}

#ifdef _DEBUG
	if(bGui) ImGui::TreePop();
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void Sword::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		D3DXMATRIX mtxWorld, mtxTemp;
		Player *pPlayer = ObjectManager::GetObjectPointer<Player>(ObjectManager::PLAYER);

		// ������`��
		for (unsigned int i = 0; i < m_nCntMax; i++)
		{
			WorldConvertAxis(&mtxWorld,
				m_cSword[i].vPos,
				-m_cSword[i].vZ,
				m_cSword[i].vX,
				m_vScl);

			m_CXModel->Draw(mtxWorld);
		}

		// ����������
		if (m_pSetMtx != NULL)
		{
			// �ݒu��}�g���N�X���R�s�[
			mtxTemp = *m_pSetMtx;

			// �}�g���N�X�𐳋K��
			D3DXMatrixNormalize(&mtxTemp, &mtxTemp);
		}
		else D3DXMatrixIdentity(&mtxTemp);

		// ���̍s��Ƀv���C���[�̃{�[���}�g���N�X��������
		D3DXMatrixMultiply(&mtxTemp, &m_mtxSword, &mtxTemp);

		m_CXModel->Draw(mtxTemp);
	}
}

//=============================================================================
// �ˏo����
//=============================================================================
void Sword::Shot(void)
{
	int nCntMax = m_nCntMax / 2;
	//if (m_nCntMax & 1)
	//{
	//	nCntMax++;
	//}
	for (unsigned int i = 0; i < nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::SHOT;
			m_cSword[i].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
		else if(m_cSword[i+ nCntMax].eMode != SwordData::SHOT)
		{
			m_cSword[i + nCntMax].eMode = SwordData::SHOT;
			m_cSword[i + nCntMax].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
	}
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::SHOT;
			m_cSword[i].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
	}
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::Stay(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode == SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::STAY;
			return;
		}
	}
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::StayAll(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::STAY)
		{
			m_cSword[i].eMode = SwordData::STAY;
			m_nCnt++;
		}
	}
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::RetrunAll(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode == SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::RETURN;
			m_nCnt++;
		}
	}
	m_eMode = Sword::RETURN;
	m_nCntRot = SWORD_ROT_SPEED_COUNT;
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::Add(void)
{
	for (unsigned int i = 0; i < SWORD_MAX; i++)
	{
		if (!m_cSword[i].bUse)
		{
			m_cSword[i].bUse = true;
			m_cSword[i].fAngle = m_fRot;
			m_nCnt++;
			m_nCntMax++;
			return;
		}
	}
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::Sub(void)
{
	for (unsigned int i = SWORD_MAX - 1; i >= 0; i--)
	{
		if (m_cSword[i].bUse)
		{
			m_cSword[i].bUse = false;
			if (m_cSword[i].eMode != SwordData::SHOT)
			{
				m_nCnt--;
			}
			m_nCntMax--;
			return;
		}
	}
}

//=============================================================================
// �i�[����
//=============================================================================
void Sword::Rot(void)
{
	//if (m_eMode == Sword::RETURN)
	//{
	//	//m_fRotSpeed += (SWORD_ROT_SPEED_MIN - m_fRotSpeed) / SWORD_ROT_SPEED_SUB;
	//	m_fRotSpeed = SWORD_ROT_SPEED_MIN;
	//	m_nCntRot--;
	//	if (m_nCntRot < 0)
	//	{
	//		m_eMode = Sword::ROT;
	//		m_nCntRot = 0;
	//	}
	//}
	//else
	//{
	//	m_fRotSpeed += (SWORD_ROT_SPEED_MAX - m_fRotSpeed) / SWORD_ROT_SPEED_ADD;
	//}
	m_fRot += m_fRotSpeed;
}
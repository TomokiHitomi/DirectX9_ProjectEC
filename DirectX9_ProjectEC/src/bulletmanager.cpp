//=============================================================================
//
// �o���b�g�}�l�[�W���[���� [bulletmanager.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "bulletmanager.h"
#include "calculate.h"

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
//#ifdef _DEBUG
//bool BulletManager::s_bDebug;
//#endif

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
BulletManager::BulletManager(void)
{
	//// �I�u�W�F�N�gID�ƃv���C�I���e�B�̐ݒ菈��
	//SetIdAndPriority(ObjectID::BULLETMANAGER, Priority::Low, Priority::Low);

	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		tProp[i].vPos = ZERO_D3DXVECTOR3;
		tProp[i].vMove = ZERO_D3DXVECTOR3;
		tProp[i].fCol = 0.0f;
		tProp[i].fMoveSpeed = 0.0f;
		tProp[i].fSize = 0.0f;
		tProp[i].nCount = 0;
		tProp[i].bUse = false;
	}

	m_pBullet = NULL;
	m_pBullet = new Bullet;

	m_pBulletEffect = NULL;
	m_pBulletEffect = new BulletEffect;
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
BulletManager::~BulletManager(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
#ifdef _DEBUG
		DebugObject::pSphere->Release(tProp[i].cDebug.nIdx);
#endif
	}
	SAFE_DELETE(m_pBullet);
	SAFE_DELETE(m_pBulletEffect);
}

//=============================================================================
// �J�n����
//=============================================================================
void BulletManager::Set(D3DXVECTOR3 vPos, D3DXVECTOR3 vMove, float fCol, float fMoveSpeed, float fSize)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (!tProp[i].bUse)
		{
			tProp[i].vPos = vPos;
			//tProp[i].vMove = vMove;
			D3DXVec3Normalize(&tProp[i].vMove, &vMove);
			tProp[i].fCol = fCol;
			tProp[i].fMoveSpeed = fMoveSpeed;
			tProp[i].fSize = fSize;
			tProp[i].nCount = 0;
			tProp[i].bUse = true;

			tProp[i].cBullet.bUse = true;
			tProp[i].cBullet.fSize = tProp[i].fSize;
			tProp[i].cBullet.nIdx = m_pBullet->Set(tProp[i].cBullet.fSize);
			m_pBullet->SetColor(tProp[i].cBullet.nIdx, tProp[i].fCol);
			
#ifdef _DEBUG
			tProp[i].cDebug.bUse = true;
			tProp[i].cDebug.fSize = tProp[i].fSize;
			tProp[i].cDebug.nIdx = DebugObject::pSphere->Set(tProp[i].cDebug.fSize);
#endif
			return;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void BulletManager::Update(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (tProp[i].bUse)
		{
			tProp[i].vPos += tProp[i].vMove * tProp[i].fMoveSpeed;

			tProp[i].nCount++;

			if (tProp[i].nCount > BULLET_COUNT_MAX) Delete(i);
			m_pBullet->SetPos(tProp[i].cBullet.nIdx, tProp[i].vPos);

			m_pBulletEffect->Set(tProp[i].vPos, tProp[i].fSize, tProp[i].fCol, 15);
#ifdef _DEBUG
			DebugObject::pSphere->SetPos(tProp[i].cDebug.nIdx, tProp[i].vPos);
#endif
		}
	}
	SAFE_UPDATE(m_pBullet);
	SAFE_UPDATE(m_pBulletEffect);
}

//=============================================================================
// �`�揈��
//=============================================================================
void BulletManager::Draw(void)
{

	SAFE_DRAW(m_pBulletEffect);
	SAFE_DRAW(m_pBullet);
}

//=============================================================================
// �폜����
//=============================================================================
void BulletManager::Delete(int nNum)
{
	tProp[nNum].bUse = false;
	m_pBullet->Release(tProp[nNum].cBullet.nIdx);
#ifdef _DEBUG
	DebugObject::pSphere->Release(tProp[nNum].cDebug.nIdx);
#endif
}

//=============================================================================
// �������
//=============================================================================
void BulletManager::Release(void)
{
	for (UINT i = 0; i < BULLET_MAX; i++)
	{
		if (tProp[i].bUse)
		{
			Delete(i);
		}
	}
}

////=============================================================================
//// �X�V����
////=============================================================================
//void EnemyNormal::Update(void)
//{
//
//}
//
////=============================================================================
//// �X�V����
////=============================================================================
//void EnemyNormal::Draw(void)
//{
//
//}


////=============================================================================
//// �R���X�g���N�^�i�����������j
////=============================================================================
//BulletManager::BulletManager(void)
//{
//	// �|�C���^��������
//	m_pBullet = NULL;
//	m_pBullet = new Bullet;
//}
//
////=============================================================================
//// �f�X�g���N�^�i�I�������j
////=============================================================================
//BulletManager::~BulletManager(void)
//{
//	Release();
//}
//
////=============================================================================
//// ����������
////=============================================================================
//void BulletManager::Init(void)
//{
//
//}
//
////=============================================================================
//// �I������
////=============================================================================
//void BulletManager::Release(void)
//{
//	SAFE_RELEASE(m_pBullet);
//	SAFE_DELETE(m_pBullet);
//}
//
////=============================================================================
//// �`�揈��
////=============================================================================
//void BulletManager::Draw(void)
//{
//	////Enemy* pList = m_pRoot;
//
//	////while (pList != NULL)
//	////{
//	////	pList->Draw();
//	////	pList = pList->m_pNext;
//	////}
//	//SAFE_DRAW(pOcta);
//}
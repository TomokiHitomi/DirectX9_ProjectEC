//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "enemy.h"
#include "game.h"
#include "calculate.h"
#include "tool.h"

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
#ifdef _DEBUG
bool EnemyManager::s_bDebug;
#endif

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
Enemy::Enemy(void)
{
	m_pPrev = NULL;
	m_pNext = NULL;
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
Enemy::~Enemy(void)
{
#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif
}

//=============================================================================
// �J�n����
//=============================================================================
void Enemy::Start(void)
{
	m_cProp.bUse = true;

	m_cOctaData.bUse = true;
	m_cOctaData.fSize = m_cProp.fSize;
	m_cOctaData.nIdx = pOcta->Set(m_cOctaData.fSize);
	pOcta->SetColor(m_cOctaData.nIdx, m_cProp.fCol);

#ifdef _DEBUG
	m_cDebug.bUse = true;
	m_cDebug.fSize = m_cProp.fSize;
	m_cDebug.nIdx = DebugObject::pSphere->Set(m_cDebug.fSize);
#endif
}

//=============================================================================
// �X�V����
//=============================================================================
void Enemy::Update(void)
{
	pOcta->SetPos(m_cOctaData.nIdx, m_cProp.vPos);
	pOcta->AddRot(m_cOctaData.nIdx, 0.02f);
#ifdef _DEBUG
	if (EnemyManager::s_bDebug)
	{
		ImGui::Text("Pos [%6.0f.%6.0f,%6.0f]\n",
			m_cProp.vPos.x, m_cProp.vPos.y, m_cProp.vPos.z);
	}
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, m_cProp.vPos);
#endif

	m_cProp.nCount++;
	if (m_cProp.nCount > ENEMY_BULLET_START)
	{
		if (m_cProp.nCount % ENEMY_BULLET_RELOAD == 0)
		{
			float fRot = pOcta->GetRot(m_cOctaData.nIdx);
			D3DXVECTOR3 vMove;
			vMove = D3DXVECTOR3(sinf(fRot), 0.0f, cosf(fRot));
			m_pBullet->Set(
				m_cProp.vPos + vMove * m_cProp.fSize,
				vMove,
				m_cProp.fCol,
				3.0f,
				10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI), 0.0f, cosf(fRot + D3DX_PI));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI * 0.5f), 0.0f, cosf(fRot + D3DX_PI * 0.5f));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);

			//vMove = D3DXVECTOR3(sinf(fRot + D3DX_PI * 1.5f), 0.0f, cosf(fRot + D3DX_PI * 1.5f));
			//m_pBullet->Set(
			//	m_cProp.vPos + vMove * m_cProp.fSize,
			//	vMove,
			//	m_cProp.fCol,
			//	3.0f,
			//	10.0f);
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void Enemy::Draw(void)
{

}

//=============================================================================
// �������
//=============================================================================
void Enemy::Release(void)
{
	pOcta->Release(m_cOctaData.nIdx);
#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif

	// �O�|�C���^��NULL�ł͂Ȃ�
	if ((m_pPrev) != NULL)
	{
		// �O�|�C���^���w�����|�C���^��
		// ���炪�i�[���Ă��鎟�|�C���^�ɍ����ւ���
		(m_pPrev)->m_pNext = m_pNext;
	}

	// ���|�C���^��NULL�ł͂Ȃ�
	if (m_pNext != NULL)
	{
		// ���|�C���^���w���O�|�C���^��
		// ���炪�i�[���Ă���O�|�C���^�ɍ����ւ���
		m_pNext->m_pPrev = m_pPrev;
	}

	delete this;
}

//=============================================================================
// �X�V����
//=============================================================================
void EnemyNormal::Update(void)
{

}

//=============================================================================
// �X�V����
//=============================================================================
void EnemyNormal::Draw(void)
{

}


//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
EnemyManager::EnemyManager(void)
{
	// �I�u�W�F�N�gID�ƃv���C�I���e�B�̐ݒ菈��
	SetIdAndPriority(ObjectID::ENEMYMANAGER, Priority::Middle, Priority::Middle);

	// ���[�g�|�C���^��������
	m_pRoot = NULL;

	pOcta = NULL;
	pOcta = new Octa;

	m_pBullet = NULL;
	m_pBullet = new BulletManager;
	//m_pBullet = ObjectManager::CreateObject<BulletManager>();

	// �Ǎ�����
	Load();
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
EnemyManager::~EnemyManager(void)
{
	Uninit();
	SAFE_DELETE(pOcta);
	SAFE_DELETE(m_pBullet);
}

//=============================================================================
// ����������
//=============================================================================
void EnemyManager::Init(void)
{

}

//=============================================================================
// �I������
//=============================================================================
void EnemyManager::Uninit(void)
{
	Enemy* pList = m_pRoot;
	Enemy* pTemp = NULL;

	while (pList != NULL)
	{
		pTemp = pList->m_pNext;
		pList->Release();
		pList = pTemp;
	}
}

//=============================================================================
// 
//=============================================================================
bool EnemyManager::Create(Enemy** ppEnemy, int* pData)
{
	if (pData[DATA_WAVE] < 0)
		return false;

	//EnemyNormal* pTest = new EnemyNormal;
	//delete pTest;

	// ����[1]���^�C�v�������Ƃ�����
	switch (pData[DATA_TYPE])
	{
	case ENEMY_NORMAL:
		*ppEnemy = new Enemy;
		break;
	case 1:
		*ppEnemy = new EnemyNormal;
		break;
	}

	(*ppEnemy)->m_cProp.nWave = pData[DATA_WAVE];
	(*ppEnemy)->m_cProp.vPos = D3DXVECTOR3(pData[DATA_POSX], pData[DATA_POSY], pData[DATA_POSZ]);
	(*ppEnemy)->m_cProp.fSize = pData[DATA_SIZE];
	(*ppEnemy)->m_cProp.fCol = pData[DATA_COL];

	(*ppEnemy)->pOcta = pOcta;
	(*ppEnemy)->m_pBullet = m_pBullet;
	return true;
}

//=============================================================================
// �t�@�C���Ǎ�����
//=============================================================================
void EnemyManager::Load(void)
{
	FILE *fp;

	// �t�@�C�����J��
	fopen_s(&fp,ENEMY_FILE, "r");

	// �t�@�C���I�[�v���G���[����
	if (fp == NULL)
		return;

	// �f�[�^�擾
	Read(fp);

	// �t�@�C�������
	fclose(fp);
}

//=============================================================================
// �t�@�C���Ǎ�����
//=============================================================================
void EnemyManager::Read(FILE* fp)
{
	int nData[8];

	Enemy** ppList = &m_pRoot;
	Enemy** ppPrev = NULL;

	if (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d",
		&nData[0], &nData[1], &nData[2], &nData[3], &nData[4], &nData[5], &nData[6], &nData[7]) != EOF)
	{
		if (!Create(ppList, &nData[0]))
			return;
		ppPrev = ppList;
		ppList = &(*ppList)->m_pNext;
	}
	else
		return;

	// �G�l�~�[�X�e�[�^�X�ݒ�
	while (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d",
		&nData[0], &nData[1], &nData[2], &nData[3], &nData[4], &nData[5], &nData[6], &nData[7]) != EOF)
	{
		if (!Create(ppList, &nData[0]))
			return;
		(*ppList)->m_pPrev = *ppPrev;
		ppPrev = ppList;
		ppList = &(*ppList)->m_pNext;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void EnemyManager::Update(void)
{
	Enemy* pList = m_pRoot;
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	s_bDebug = ImGui::TreeNode("Enemy");
#endif

	while (pList != NULL)
	{
		if (pList->m_cProp.nWave > GameScene::GetWave())
			break;

		if (pList->m_cProp.bUse)
			pList->Update();
		else
			pList->Start();

		pList = pList->m_pNext;
	}

#ifdef _DEBUG
	if (s_bDebug)
	{
		ImGui::TreePop();
	}
#endif

	SAFE_UPDATE(pOcta);
	SAFE_UPDATE(m_pBullet);
}

//=============================================================================
// �`�揈��
//=============================================================================
void EnemyManager::Draw(void)
{
	//Enemy* pList = m_pRoot;

	//while (pList != NULL)
	//{
	//	pList->Draw();
	//	pList = pList->m_pNext;
	//}
	SAFE_DRAW(pOcta);
	SAFE_DRAW(m_pBullet);
}
//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "object.h"
#include "debugobject.h"
#include "octa.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define MODEL_PLAYER			("data/model/haku/haku.X")
//#define MODEL_WING				("data/model/wing/wing.X")
#define ENEMY_FILE				("data/file/enemy.dat")

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Enemy
{
public:
	Enemy*	m_pPrev;		// ���X�g�̑O�|�C���^
	Enemy*	m_pNext;		// ���X�g�̎��|�C���^

	// �R���X�g���N�^�i�����������j
	Enemy(void);
	//�f�X�g���N�^�i�I�������j
	~Enemy(void);

	void Start(void);
	// �X�V����
	virtual void Update(void);
	// �`�揈��
	virtual void Draw(void);

	void Release(void);

	class Prop
	{
	public:
		int				nWave;	// �o���E�F�[�u
		D3DXVECTOR3		vPos;	// ���W
		float			fSize;	// �T�C�Y
		bool			bUse;	// �g�p�t���O
		Prop()
		{
			nWave = -1;
			vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			fSize = 0.0f;
			bUse = false;
		}
	};

	Prop		m_cProp;
	Octa*		pOcta;
	OctaData	m_cOctaData;
#ifdef _DEBUG
	DebugObjectData m_cDebug;
#endif
};

class EnemyNormal : public Enemy
{
public:
	int nTest;
	// �R���X�g���N�^�i�����������j
	EnemyNormal(void) { ; }
	//�f�X�g���N�^�i�I�������j
	~EnemyNormal(void) { ; }

	// �X�V����
	void Update(void);
	// �`�揈��
	void Draw(void);
private:


};



class EnemyManager : public ObjectManager
{
public:
	enum ENEMY
	{	
		ENEMY_NORMAL,
		ENEMY_MAX
	};

	Enemy* m_pRoot;		// ���X�g�̍X�V���[�g�|�C���^

	// �R���X�g���N�^�i�����������j
	EnemyManager(void);
	//�f�X�g���N�^�i�I�������j
	~EnemyManager(void);

	// ����������
	void Init(void);
	// �I������
	void Uninit(void);
	// �X�V����
	void Update(void);
	// �`�揈��
	void Draw(void);
	// �t�@�C���Ǎ�����
	void Load(void);
	// �t�@�C���Ǎ�����
	void Read(FILE* fp);
 
	bool Create(Enemy** ppEnemy, int* pData);
	Octa* pOcta;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

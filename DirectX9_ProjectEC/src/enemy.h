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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define MODEL_PLAYER			("data/model/haku/haku.X")
//#define MODEL_WING				("data/model/wing/wing.X")

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Enemy
{
public:

	// �R���X�g���N�^�i�����������j
	Enemy(void);
	//�f�X�g���N�^�i�I�������j
	~Enemy(void);

	// �X�V����
	virtual void Update(void);
	// �`�揈��
	virtual void Draw(void);
private:


};

class EnemyNormal : public Enemy
{
public:

	// �R���X�g���N�^�i�����������j
	EnemyNormal(void);
	//�f�X�g���N�^�i�I�������j
	~EnemyNormal(void);

	// �X�V����
	virtual void Update(void);
	// �`�揈��
	virtual void Draw(void);
private:


};



class EnemyManager
{
public:
	enum ENEMY
	{	
		ENEMY_MAX
	};

	// �R���X�g���N�^�i�����������j
	EnemyManager(void);
	//�f�X�g���N�^�i�I�������j
	~EnemyManager(void);


private:
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif
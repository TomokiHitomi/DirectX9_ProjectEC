//=============================================================================
//
// �o���b�g�}�l�[�W���[���� [bulletmanager.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "debugobject.h"
#include "bullet.h"
#include "bulleteffect.h"
//#include "object.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define BULLET_MAX			(8192)
#define BULLET_COUNT_MAX	(800)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class BulletManager/* : public ObjectManager*/
{
public:
	// �R���X�g���N�^�i�����������j
	BulletManager(void);
	//�f�X�g���N�^�i�I�������j
	~BulletManager(void);

	// �X�V����
	virtual void Update(void);
	// �`�揈��
	virtual void Draw(void);
	// �ݒu����
	virtual void Set(D3DXVECTOR3 vPos, D3DXVECTOR3 vMove, float fCol, float fMoveSpeed, float fSize);
	// �폜����
	virtual void Delete(int nNum);
	// �폜����
	virtual void Release(void);


	typedef struct BulletProp
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vMove;
		float		fCol;
		float		fMoveSpeed;
		float		fSize;
		int			nCount;
		bool		bUse;
		BulletData	cBullet;
#ifdef _DEBUG
		DebugObjectData cDebug;
#endif
	};

	BulletProp tProp[BULLET_MAX];

	Bullet*			m_pBullet;
	BulletEffect*	m_pBulletEffect;

	//typedef struct BulletData
	//{
	//	float fMoveSpeed;
	//};
	//
	//BulletData tData;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

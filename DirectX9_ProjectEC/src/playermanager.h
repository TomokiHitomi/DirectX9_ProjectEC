//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _PLAYERMANAGER_H_
#define _PLAYERMANAGER_H_

/*******************************************************************************
* �C���N���[�h
*******************************************************************************/
#include "object.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class PlayerManager : public ObjectManager
{
public:
	enum PLAYER
	{	// �v���C���[�Ǘ�
		PLAYER_1P,
		PLAYER_MAX
	};
	static Player		*m_pPlayer[PLAYER_MAX];

	// �R���X�g���N�^�i�����������j
	PlayerManager(void);
	//�f�X�g���N�^�i�I�������j
	~PlayerManager(void);

	// �X�V����
	void	Update(void);
	// �`�揈��
	void	Draw(void);

	static void Reset(void);
	static Player *GetPlayer(PLAYER player) { return m_pPlayer[player]; }
	static D3DXVECTOR3 GetPos(PLAYER player) { return m_pPlayer[player]->GetPos(); }
	static D3DXMATRIX GetMtx(PLAYER player) { return m_pPlayer[player]->GetMtx(); }
	static Player::MODE GetMode(PLAYER player) { return m_pPlayer[player]->GetMode(); }
	static D3DXVECTOR3 GetVecX(PLAYER player) { return m_pPlayer[player]->GetVecX(); }
	static D3DXVECTOR3 GetVecY(PLAYER player) { return m_pPlayer[player]->GetVecY(); }
	static D3DXVECTOR3 GetVecZ(PLAYER player) { return m_pPlayer[player]->GetVecZ(); }
	static D3DXVECTOR3 GetPosTaget(PLAYER player) { return m_pPlayer[player]->GetPosTaget(); }
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif

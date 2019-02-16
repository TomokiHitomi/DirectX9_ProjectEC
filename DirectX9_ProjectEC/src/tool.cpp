//=============================================================================
//
// �c�[������ [tool.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "tool.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
D3DXVECTOR4 Color::xColor[COLOR_PALLET_MAX];

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Color g_cColor;

//=============================================================================
// �J���[�p���b�g�Ăяo������
//=============================================================================
D3DXCOLOR SetColorPallet(int nColor)
{
	D3DXCOLOR temp;
	//temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	switch (nColor)
	{
	case COLOR_PALLET_WHITE:		// �z���C�g
		temp = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_RED:			// ���b�h
		temp = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_YELLOW:		// �C�G���[
		temp = D3DXCOLOR(0.9f, 0.9f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_GREEN:		// �O���[��
		temp = D3DXCOLOR(0.0f, 1.0f, 0.05f, 1.0f);
		break;
	case COLOR_PALLET_CYAN:			// �V�A��
		temp = D3DXCOLOR(0.0f, 0.9f, 0.9f, 1.0f);
		break;
	case COLOR_PALLET_BLUE:			// �u���[
		temp = D3DXCOLOR(0.1f, 0.1f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_MAGENTA:		// �}�[���_
		temp = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_BROWN:		// �u���E��
		temp = D3DXCOLOR(0.6f, 0.4f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTGREEN:	// ���C�g�O���[��
		temp = D3DXCOLOR(0.3f, 0.9f, 0.3f, 1.0f);
		break;
	case COLOR_PALLET_ORANGE:		// �I�����W
		temp = D3DXCOLOR(1.0f, 0.3f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_BLACK:		// �u���b�N
		temp = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case COLOR_PALLET_LIGHTBLUE:	// �u���[
		temp = D3DXCOLOR(0.4f, 0.7f, 1.0f, 1.0f);
		break;
	case COLOR_PALLET_GRAY:			// �O���C
		temp = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		break;
	}
	return temp;
}

//=============================================================================
// �J���[�p���b�g�Ăяo������
//=============================================================================
D3DXCOLOR SetColorPalletRandom(void)
{
	return SetColorPallet(rand() % COLOR_PALLET_MAX);
}

//=============================================================================
// �R���X�g���N�^
//=============================================================================
Color::Color(void)
{
	for (UINT i = 0; i < COLOR_PALLET_MAX; i++)
	{
		xColor[i] = (D3DXVECTOR4)SetColorPallet(i);
	}
}

//=============================================================================
// �֐����F
// ����  �F
// �߂�l�F
// ����  �F
//=============================================================================

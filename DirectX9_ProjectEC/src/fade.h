//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef ___FADE_H___
#define ___FADE_H___

#include "main.h"
#include "scene.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FADE_RATE		(0.02f)		// �t�F�[�h�W��
#define	FADE_HALF		(0.4f)		// �n�[�t�t�F�[�h

#define	FADE_COLOR_R	(0.0f)		// �t�F�[�h�J���[R
#define	FADE_COLOR_G	(0.0f)		// �t�F�[�h�J���[G
#define	FADE_COLOR_B	(0.0f)		// �t�F�[�h�J���[B

//*************************************
// �t�F�[�h�̏��
//*************************************
enum FADE
{
	FADE_NONE = 0,		// �����Ȃ����
	FADE_IN,			// �t�F�[�h�C������
	FADE_OUT,			// �t�F�[�h�A�E�g����
	FADE_OUT_HALF,		// �t�F�[�h�A�E�g����
	FADE_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade);
void SetFadeScene(SceneManager::SCENE eScene);
FADE GetFade(void);

#endif
//=============================================================================
//
// �V�[������ [scene.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _SCENE_H_
#define _SCENE_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "object.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCENE_UIOBJECT_MAX		(10)
#define SCENE_GAME_WIN			(50.0f)		// ��������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class BaseScene
{
public:
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual ~BaseScene() {};
};

class SceneManager
{
public:
	// �V�[�����X�g
	enum SCENE 	{TITLE, GAME, RESULT, MAX	};
	// �R���X�g���N�^
	SceneManager(void);
	// �f�X�g���N�^
	~SceneManager(void);
	// �V�[���������ɂ���ĕς���֐�
	static void			ChangeScene(SCENE scene);
	// �V�[���̏������֐�
	static void			Init(HINSTANCE hInst, HWND hWnd);
	// �V�[���̏I���֐�
	static void			Uninit(void);
	// �V�[���̍X�V�֐�
	static void			Update(void);
	// �V�[���̕`��֐�
	static void			Draw(void);

	// ���݂̃V�[���擾
	static SCENE		GetScene(void)				{ return m_eScene; }

	// SE�̏I���֐�
	static void			EndSeS(void);

private:
	static BaseScene	*m_pScene;			// ���݂̃V�[��
	static SCENE		m_eScene;			// ���݂̃V�[���ԍ�

	static ShaderManager	*ShaderMgr;
	static CameraManager	*CameraMgr;
public:
	static ModelManager		*ModelMgr;

#ifdef _DEBUG
	// �f�o�b�O�p�V�[���J��
	static void DebugScene(void);
#endif
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

#endif
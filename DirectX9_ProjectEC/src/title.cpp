//=============================================================================
//
// �^�C�g������ [title.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "title.h"

/* �S�̂ŕK�v�ȃC���N���[�h */
#include "input.h"
#include "camera.h"
#include "sound.h"

/* �^�C�g���ŕK�v�ȃC���N���[�h */
#include "object.h"
#include "SkinMeshX.h"
#include "light.h"
#include "player.h"
#include "skydome.h"
#include "stencil.h"
#include "effect.h"
#include "stage.h"
#include "debugobject.h"

/* Debug */
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
// �X�V����
//=============================================================================
void TitleScene::Update(void)
{
	ObjectManager::UpdateAll();
#ifdef _DEBUG
	//if (GetKeyboardTrigger(DIK_5))
	//{
	//	new Copyright;
	//}
	//if (GetKeyboardTrigger(DIK_6))
	//{
	//	Object *pTemp = Object::GetObjectPointer(Object::COPYRIGHT);
	//	if (pTemp != NULL)
	//	{
	//		pTemp->Release();
	//	}
	//}
	//if (GetKeyboardTrigger(DIK_7))
	//{
	//	new AirWaterFream;
	//}
	//if (GetKeyboardTrigger(DIK_8))
	//{
	//	Object *pTemp = Object::GetObjectPointer(Object::FRAME);
	//	if (pTemp != NULL)
	//	{
	//		pTemp->Release();
	//	}
	//}
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void TitleScene::Draw(void)
{
	ObjectManager::DrawAll();
}

//=============================================================================
// �R���X�g���N�^�����i�������j
//=============================================================================
TitleScene::TitleScene(void)
{
	ObjectManager::CreateObject<DebugObject>();
	ObjectManager::CreateObject<PlayerManager>();
	ObjectManager::CreateObject<Stage>();
	ObjectManager::CreateObject<EffectManager>();
}

//=============================================================================
// �f�X�g���N�^�����i�I���j
//=============================================================================
TitleScene::~TitleScene(void)
{
	ObjectManager::ReleaseAll();
}

//=============================================================================
// �^�C�g���f���̃��X�^�[�g���\�b�h
//=============================================================================
void TitleScene::DemoRestart(void)
{

}
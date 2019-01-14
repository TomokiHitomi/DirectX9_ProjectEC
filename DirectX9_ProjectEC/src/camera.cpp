//=============================================================================
//
// �J�������� [camera.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "camera.h"
#include "input.h"
#include "main.h"
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
Camera						*CameraManager::pCamera[TYPE_MAX];
CameraManager::CameraPort	CameraManager::eCameraPort;
CameraManager::CameraType	CameraManager::eCameraType;

//=============================================================================
// �R���X�g���N�^�i�������j
//=============================================================================
CameraManager::CameraManager(void)
{
	eCameraPort = CENTER;
	eCameraType = SINGLE;

	for (unsigned int i = 0; i < TYPE_MAX; i++)
	{
		pCamera[i] = NULL;
		pCamera[i] = new Camera(CameraType(i));
	}
}

//=============================================================================
// �f�X�g���N�^�i�I���j
//=============================================================================
CameraManager::~CameraManager(void)
{
	for (unsigned int i = 0; i < TYPE_MAX; i++)
	{
		if (pCamera[i] != NULL)
		{
			delete pCamera[i];
		}
	}
}

//=============================================================================
// ����������
//=============================================================================
void CameraManager::Init(void)
{
	for (unsigned int i = 0; i < TYPE_MAX; i++)
	{
		if (pCamera[i] != NULL)
		{
			pCamera[i]->Init(CameraType(i));
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void CameraManager::Update(void)
{
#ifdef _DEBUG
	PrintDebugProc("�y Camera �z\n");
#endif
	//if (GetKeyboardTrigger(DIK_B))
	//{
	//	SetExtension(EXTENSION_L);
	//}
	//if (GetKeyboardTrigger(DIK_N))
	//{
	//	SetExtension(CENTER);
	//}
	//if (GetKeyboardTrigger(DIK_M))
	//{
	//	SetExtension(EXTENSION_R);
	//}
	//Extension();

	for (unsigned int i = 0; i < TYPE_MAX; i++)
	{
		if (pCamera[i] != NULL)
		{
			pCamera[i]->Update();
#ifdef _DEBUG
			PrintDebugProc("Multi%d  PortX:%d PortW:%d PortExt:%d\n",
				i + 1, pCamera[i]->dvPort.X, pCamera[i]->dvPort.Width
				, pCamera[i]->bExtension);
			PrintDebugProc("\n");
#endif
		}
	}
#ifdef _DEBUG
#endif
}

//=============================================================================
// �ݒ菈��
//=============================================================================
void CameraManager::Set(CameraType eCT)
{
	eCameraType = eCT;
	if (pCamera[eCT] != NULL)
	{
		pCamera[eCT]->Set();
	}
}

////=============================================================================
//// ��ʊg���ݒ菈��
////=============================================================================
//void CameraManager::SetExtension(CameraPort eCP)
//{
//	eCameraPort = eCP;
//	for (unsigned int i = 0; i < MULTI_MAX; i++)
//	{
//		if (pCamera[i] != NULL)
//		{
//			pCamera[i]->bExtension = true;
//		}
//	}
//}
//
////=============================================================================
//// ��ʊg������
////=============================================================================
//void CameraManager::Extension(void)
//{
//	int PortWidth, PortX;
//
//	for (unsigned int i = 0; i < MULTI_MAX; i++)
//	{
//		if (pCamera[i] != NULL && pCamera[i]->bExtension)
//		{
//			PortWidth = int(pCamera[i]->dvPort.Width);
//			PortX = int(pCamera[i]->dvPort.X);
//			switch (CameraType(i))
//			{
//			case MULTI1:
//				switch (eCameraPort)
//				{
//				case CENTER:
//					if (PortWidth > int(CAMERA_MULTI_WIDTH))
//					{
//						PortWidth -= int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth < int(CAMERA_MULTI_WIDTH))
//						{
//							PortWidth = int(CAMERA_MULTI_WIDTH);
//							pCamera[i]->bExtension = false;
//						}
//					}
//					else if (PortWidth < int(CAMERA_MULTI_WIDTH))
//					{
//						PortWidth += int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth > int(CAMERA_MULTI_WIDTH))
//						{
//							PortWidth = int(CAMERA_MULTI_WIDTH);
//							pCamera[i]->bExtension = false;
//						}
//					}
//					break;
//				case EXTENSION_L:
//					if (PortWidth < int(SCREEN_WIDTH))
//					{
//						PortWidth += int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth > int(SCREEN_WIDTH))
//						{
//							PortWidth = int(SCREEN_WIDTH);
//							pCamera[i]->bExtension = false;
//						}
//					}
//					break;
//				case EXTENSION_R:
//					if (PortWidth > 0)
//					{
//						PortWidth -= int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth < 0)
//						{
//							PortWidth = 0;
//							pCamera[i]->bExtension = false;
//						}
//					}
//					break;
//				}
//				break;
//			case MULTI2:
//				switch (eCameraPort)
//				{
//				case CENTER:
//					if (PortWidth > int(CAMERA_MULTI_WIDTH) || PortX < int(CAMERA_MULTI_2_X))
//					{
//						if (PortX == 0)
//						{
//							PortX += CAMERA_MULTI_MARGIN * 2;
//						}
//						PortX += int(CAMERA_EXTENSION_SPEED);
//						if (PortX > int(CAMERA_MULTI_2_X))
//						{
//							PortX = int(CAMERA_MULTI_2_X);
//						}
//						PortWidth -= int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth < int(CAMERA_MULTI_WIDTH))
//						{
//							PortWidth = int(CAMERA_MULTI_WIDTH);
//							pCamera[i]->bExtension = false;
//						}
//					}
//					else if (PortWidth < int(CAMERA_MULTI_WIDTH) || PortX > int(CAMERA_MULTI_2_X))
//					{
//						if (PortX == int(SCREEN_WIDTH))
//						{
//							PortX += CAMERA_MULTI_MARGIN * 2;
//						}
//						PortX -= int(CAMERA_EXTENSION_SPEED);
//						if (PortX < int(CAMERA_MULTI_2_X))
//						{
//							PortX = int(CAMERA_MULTI_2_X);
//							pCamera[i]->bExtension = false;
//						}
//						PortWidth += int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth > int(CAMERA_MULTI_WIDTH))
//						{
//							PortWidth = int(CAMERA_MULTI_WIDTH);
//						}
//					}
//					break;
//				case EXTENSION_L:
//					if (PortWidth > int(0))
//					{
//						if (PortX == 0)
//						{
//							PortX += CAMERA_MULTI_MARGIN * 2;
//						}
//						PortX += int(CAMERA_EXTENSION_SPEED);
//						if (PortX > int(SCREEN_WIDTH))
//						{
//							PortX = int(SCREEN_WIDTH);
//						}
//						PortWidth -= int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth < 0)
//						{
//							PortWidth = 0;
//							pCamera[i]->bExtension = false;
//						}
//					}
//					break;
//				case EXTENSION_R:
//					if (PortWidth < int(SCREEN_WIDTH))
//					{
//						if (PortX == int(SCREEN_WIDTH))
//						{
//							PortX += CAMERA_MULTI_MARGIN * 2;
//						}
//						PortX -= int(CAMERA_EXTENSION_SPEED);
//						if (PortX < 0)
//						{
//							PortX = 0;
//						}
//						PortWidth += int(CAMERA_EXTENSION_SPEED);
//						if (PortWidth > int(SCREEN_WIDTH))
//						{
//							PortWidth = int(SCREEN_WIDTH);
//							pCamera[i]->bExtension = false;
//						}
//					}
//					break;
//				}
//				break;
//			}
//			pCamera[i]->dvPort.Width = DWORD(PortWidth);
//			pCamera[i]->dvPort.X = DWORD(PortX);
//			pCamera[i]->fAspect = float(pCamera[i]->dvPort.Width) / float(SCREEN_HEIGHT);
//		}
//	}
//}


//=============================================================================
// �R���X�g���N�^�i�������j
//=============================================================================
Camera::Camera(CameraManager::CameraType eCameraType)
{
	Init(eCameraType);
}

//=============================================================================
// �f�X�g���N�^�i�I���j
//=============================================================================
Camera::~Camera(void)
{

}

//=============================================================================
// ����������
//=============================================================================
void Camera::Init(CameraManager::CameraType eCameraType)
{
	vEye = D3DXVECTOR3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM);
	vAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	vEyeNext = vEye;
	vAtNext = vAt;
	vUpNext = vUp;

	fEyeIner = 1.0f;
	fAtIner = 1.0f;
	fUpIner = 1.0f;

	bExtension = false;

	switch (eCameraType)
	{
	//case CameraManager::CameraType::MULTI1:
	//	dvPort = D3DVIEWPORT9{ 0, 0, DWORD(CAMERA_MULTI_WIDTH), DWORD(CAMERA_MULTI_HEIGHT), 0.0f, DWORD(1.0f) };
	//	//fAspect = ((float)SCREEN_WIDTH / 2 - SCREEN_MARGIN) / ((float)SCREEN_HEIGHT);
	//	break;
	//case CameraManager::CameraType::MULTI2:
	//	dvPort = D3DVIEWPORT9{ DWORD(CAMERA_MULTI_2_X), 0, DWORD(CAMERA_MULTI_WIDTH), DWORD(CAMERA_MULTI_HEIGHT),0.0f,DWORD(1.0f) };
	//	//fAspect = ((float)SCREEN_WIDTH / 2 - SCREEN_MARGIN) / ((float)SCREEN_HEIGHT);
	//	break;
	case CameraManager::CameraType::SINGLE:
		dvPort = D3DVIEWPORT9{ 0, 0, DWORD(SCREEN_WIDTH), DWORD(SCREEN_HEIGHT), 0.0f, DWORD(1.0f) };
		//fAspect = ((float)SCREEN_WIDTH) / ((float)SCREEN_HEIGHT);
		break;
	}
	fAspect = float(dvPort.Width) / float(SCREEN_HEIGHT);
}

//=============================================================================
// �I������
//=============================================================================
void Camera::Uninit(void)
{

}

//=============================================================================
// �X�V����
//=============================================================================
void Camera::Update(void)
{
	vEye = vEye + ((vEyeNext - vEye) * fEyeIner);
	vAt = vAt + ((vAtNext - vAt) * fAtIner);
	vUp = vUp + ((vUpNext - vUp) * fUpIner);
}

//=============================================================================
// �J�����̒����_�ݒ菈��
//=============================================================================
void Camera::SetAt(D3DXVECTOR3 vAt)
{
	//camera->posCameraAt = vAt;
	vAtNext = vAt;
}

//=============================================================================
// �J�����̎��_�ݒ菈��
//=============================================================================
void Camera::SetEye(D3DXVECTOR3 vEye)
{
	//camera->posCameraEye = vEye;
	vEyeNext = vEye;
}

//=============================================================================
// �J�����̏㕔�ݒ菈��
//=============================================================================
void Camera::SetUp(D3DXVECTOR3 vUp)
{
	vUpNext = vUp;
	//camera->vUpNext = vUp;
}

//=============================================================================
// �J�����̒����_�̊����ݒ菈��
//=============================================================================
void Camera::SetAtIner(float fIner)
{
	fAtIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// �J�����̎��_�̊����ݒ菈��
//=============================================================================
void Camera::SetEyeIner(float fIner)
{
	fEyeIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// �J�����̏㕔�̊����ݒ菈��
//=============================================================================
void Camera::SetUpIner(float fIner)
{
	fUpIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// �J�����̒����_�̊�����������
//=============================================================================
void Camera::AddAtIner(float fIner)
{
	fAtIner = SetLimit(fAtIner + fIner, 1.0f, 0.0f);
}

//=============================================================================
// �J�����̎��_�̊�����������
//=============================================================================
void Camera::AddEyeIner(float fIner)
{
	fEyeIner = SetLimit(fEyeIner + fIner, 1.0f, 0.0f);
}

//=============================================================================
// �J�����̏㕔�̊�����������
//=============================================================================
void Camera::AddUpIner(float fIner)
{
	fUpIner = SetLimit(fUpIner + fIner, 1.0f, 0.0f);
}


//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================
void Camera::Set()
{
	//vEye = D3DXVECTOR3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM);
	//vAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	/******************** �r���[�C���O�ϊ� ********************/
	// �r���[�}�g���N�X�̏�����
	D3DXMatrixIdentity(&mtxView);

	// �r���[�}�g���N�X�̍쐬
	D3DXMatrixLookAtLH(&mtxView,
		&vEye,	// ���_
		&vAt,	// �����_
		&vUp);	// �����

	pDevice->SetViewport(&dvPort);

	// �r���[�}�g���N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);

	//pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(g_color[i].r, g_color[i].b, g_color[i].g), 1.0f, 0);
	//pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	/******************** �v���W�F�N�V�����ϊ� ********************/
	// �v���W�F�N�V�����}�g���N�X�̏�����
	D3DXMatrixIdentity(&mtxProjection);

	// �v���W�F�N�V�����}�g���N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		VIEW_ANGLE,			// �r���[���ʂ̎���p
		//VIEW_ASPECT,		// �r���[���ʂ̃A�X�y�N�g��
		fAspect,
		VIEW_NEAR_Z,		// �r���[���ʂ�NearZ�l�i�߂��ƕ`�悵�Ȃ��j
		VIEW_FAR_Z);		// �r���[���ʂ�FarZ�l�i�����ƕ`�悵�Ȃ��j

	// �v���W�F�N�V�����}�g���N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);


	/******************** UpY�̃r���[�C���O�ϊ� ********************/
	D3DXVECTOR3 vUpY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	// �r���[�}�g���N�X�̏�����
	D3DXMatrixIdentity(&mtxViewUpY);

	// �r���[�}�g���N�X�̍쐬
	D3DXMatrixLookAtLH(&mtxViewUpY,
		&vEye,	// ���_
		&vAt,	// �����_
		&vUpY);	// �����}
}

////=============================================================================
//// �J�����Ƒ΂ɂȂ��]�s����擾
////=============================================================================
//D3DXMATRIX Camera::GetInvRotateMat(D3DXVECTOR3 pos)
//{
//	D3DXMATRIX Inv;
//	D3DXMatrixIdentity(&Inv);
//	D3DXMatrixLookAtLH(&Inv, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &GetLookAtVct(), &camera->vecCameraUp);
//	D3DXMatrixInverse(&Inv, NULL, &Inv);
//
//	return Inv;
//}

////=============================================================================
//// �����_�Ǝ��_�̐��K�����ꂽ�x�N�g�����擾����֐�
////=============================================================================
//D3DXVECTOR3 Camera::GetLookAtVct(void)
//{
//	CAMERA *camera = GetCamera();
//	D3DXVECTOR3	vecLookAtVct;
//	vecLookAtVct = camera->posCameraEye - camera->posCameraAt;
//	return vecLookAtVct;
//}

//=============================================================================
// �r���[�}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX Camera::GetMtxView(void)
{
	return mtxView;
}

//=============================================================================
// �r���[�}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX Camera::GetMtxViewUpY(void)
{
	return mtxViewUpY;
}

//=============================================================================
// �v���W�F�N�V�����}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX Camera::GetMtxProjection(void)
{
	return mtxProjection;
}
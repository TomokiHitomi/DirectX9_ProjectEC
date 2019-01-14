//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "camera.h"
#include "input.h"
#include "main.h"
#include "calculate.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Camera						*CameraManager::pCamera[TYPE_MAX];
CameraManager::CameraPort	CameraManager::eCameraPort;
CameraManager::CameraType	CameraManager::eCameraType;

//=============================================================================
// コンストラクタ（初期化）
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
// デストラクタ（終了）
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
// 初期化処理
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
// 更新処理
//=============================================================================
void CameraManager::Update(void)
{
#ifdef _DEBUG
	PrintDebugProc("【 Camera 】\n");
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
// 設定処理
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
//// 画面拡張設定処理
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
//// 画面拡張処理
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
// コンストラクタ（初期化）
//=============================================================================
Camera::Camera(CameraManager::CameraType eCameraType)
{
	Init(eCameraType);
}

//=============================================================================
// デストラクタ（終了）
//=============================================================================
Camera::~Camera(void)
{

}

//=============================================================================
// 初期化処理
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
// 終了処理
//=============================================================================
void Camera::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void Camera::Update(void)
{
	vEye = vEye + ((vEyeNext - vEye) * fEyeIner);
	vAt = vAt + ((vAtNext - vAt) * fAtIner);
	vUp = vUp + ((vUpNext - vUp) * fUpIner);
}

//=============================================================================
// カメラの注視点設定処理
//=============================================================================
void Camera::SetAt(D3DXVECTOR3 vAt)
{
	//camera->posCameraAt = vAt;
	vAtNext = vAt;
}

//=============================================================================
// カメラの視点設定処理
//=============================================================================
void Camera::SetEye(D3DXVECTOR3 vEye)
{
	//camera->posCameraEye = vEye;
	vEyeNext = vEye;
}

//=============================================================================
// カメラの上部設定処理
//=============================================================================
void Camera::SetUp(D3DXVECTOR3 vUp)
{
	vUpNext = vUp;
	//camera->vUpNext = vUp;
}

//=============================================================================
// カメラの注視点の慣性設定処理
//=============================================================================
void Camera::SetAtIner(float fIner)
{
	fAtIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// カメラの視点の慣性設定処理
//=============================================================================
void Camera::SetEyeIner(float fIner)
{
	fEyeIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// カメラの上部の慣性設定処理
//=============================================================================
void Camera::SetUpIner(float fIner)
{
	fUpIner = SetLimit(fIner, 1.0f, 0.0f);
}

//=============================================================================
// カメラの注視点の慣性増減処理
//=============================================================================
void Camera::AddAtIner(float fIner)
{
	fAtIner = SetLimit(fAtIner + fIner, 1.0f, 0.0f);
}

//=============================================================================
// カメラの視点の慣性増減処理
//=============================================================================
void Camera::AddEyeIner(float fIner)
{
	fEyeIner = SetLimit(fEyeIner + fIner, 1.0f, 0.0f);
}

//=============================================================================
// カメラの上部の慣性増減処理
//=============================================================================
void Camera::AddUpIner(float fIner)
{
	fUpIner = SetLimit(fUpIner + fIner, 1.0f, 0.0f);
}


//=============================================================================
// カメラの設定処理
//=============================================================================
void Camera::Set()
{
	//vEye = D3DXVECTOR3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM);
	//vAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	/******************** ビューイング変換 ********************/
	// ビューマトリクスの初期化
	D3DXMatrixIdentity(&mtxView);

	// ビューマトリクスの作成
	D3DXMatrixLookAtLH(&mtxView,
		&vEye,	// 視点
		&vAt,	// 注視点
		&vUp);	// 上方向

	pDevice->SetViewport(&dvPort);

	// ビューマトリクスの設定
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);

	//pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(g_color[i].r, g_color[i].b, g_color[i].g), 1.0f, 0);
	//pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	/******************** プロジェクション変換 ********************/
	// プロジェクションマトリクスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	// プロジェクションマトリクスの作成
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		VIEW_ANGLE,			// ビュー平面の視野角
		//VIEW_ASPECT,		// ビュー平面のアスペクト比
		fAspect,
		VIEW_NEAR_Z,		// ビュー平面のNearZ値（近いと描画しない）
		VIEW_FAR_Z);		// ビュー平面のFarZ値（遠いと描画しない）

	// プロジェクションマトリクスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);


	/******************** UpYのビューイング変換 ********************/
	D3DXVECTOR3 vUpY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	// ビューマトリクスの初期化
	D3DXMatrixIdentity(&mtxViewUpY);

	// ビューマトリクスの作成
	D3DXMatrixLookAtLH(&mtxViewUpY,
		&vEye,	// 視点
		&vAt,	// 注視点
		&vUpY);	// 上方向}
}

////=============================================================================
//// カメラと対になる回転行列を取得
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
//// 注視点と視点の正規化されたベクトルを取得する関数
////=============================================================================
//D3DXVECTOR3 Camera::GetLookAtVct(void)
//{
//	CAMERA *camera = GetCamera();
//	D3DXVECTOR3	vecLookAtVct;
//	vecLookAtVct = camera->posCameraEye - camera->posCameraAt;
//	return vecLookAtVct;
//}

//=============================================================================
// ビューマトリックスの取得
//=============================================================================
D3DXMATRIX Camera::GetMtxView(void)
{
	return mtxView;
}

//=============================================================================
// ビューマトリックスの取得
//=============================================================================
D3DXMATRIX Camera::GetMtxViewUpY(void)
{
	return mtxViewUpY;
}

//=============================================================================
// プロジェクションマトリックスの取得
//=============================================================================
D3DXMATRIX Camera::GetMtxProjection(void)
{
	return mtxProjection;
}
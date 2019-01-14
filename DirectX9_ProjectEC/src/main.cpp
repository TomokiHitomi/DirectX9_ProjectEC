//=============================================================================
//
// ���C������ [main.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include <thread>

/* System */
#include "scene.h"
#include "input.h"
#include "joycon.h"

///* imgui */
//#include "imgui.h"
//#include "imgui_impl_dx9.h"
//#include "imgui_internal.h"

/* Debug */
#ifdef _DEBUG
#include "debugproc.h"
#endif


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void MainLoop(HWND hWnd);
void SubLoop(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3D�I�u�W�F�N�g
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// �f�o�C�X�I�u�W�F�N�g(�`��ɕK�v)
int					g_nCountFPS = 0;		// FPS�J�E���^
bool				g_bContinue = true;		// �Q�[���p���t���O

bool show_another_window = false;

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),									// �\���̂̃T�C�Y
		CS_CLASSDC,											// �N���X�̃X�^�C��
		WndProc,											// �E�B���h�v���V�[�W��
		0,													// �⏕������
		0,													// �⏕������
		hInstance,											// �C���X�^���X�n���h��
		NULL,												// �A�C�R���摜
		LoadCursor(NULL, IDC_ARROW),						// �J�[�\���摜
		(HBRUSH)(COLOR_WINDOW + 1),							// �w�i�F
		NULL,												// ���j���[��
		CLASS_NAME,											// �N���X��
		NULL												// �A�C�R��16x16
	};

	HWND hWnd;
	MSG msg;

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL,
		NULL,
		hInstance,
		NULL);

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	bool flag = true; // �t���X�N���[�����[�h�ŏ�����
	if (MessageBox(hWnd, START_UP_COMMENT, "�N���m�F", MB_YESNO) == IDYES)
	{
		flag = false;
	}

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, flag)))
	{
		return -1;
	}

	//�����_���̏�����
	srand((unsigned)time(NULL));

	//�t���[���J�E���g������
	timeBeginPeriod(1);				// ����\��ݒ�
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();
	dwCurrentTime =
		dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	std::thread t1(SubLoop);

	// ���b�Z�[�W���[�v
	while (g_bContinue)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{	// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
#ifdef _DEBUG
				PrintDebugProc("�y FPS : %d �z\n\n", g_nCountFPS);
				DebugHowTo();
#endif
				// �E�B���h�E���L���ȏꍇ
				if (hWnd == GetActiveWindow())
				{
					// �E�B���h�E�̍��W���擾
					RECT	lpScreen;
					GetWindowRect(hWnd, &lpScreen);

					// �}�E�X�J�[�\���̈ړ��͈͂��w��
					lpScreen.left = lpScreen.left + (long)MOUSE_MARGIN;
					lpScreen.top = lpScreen.bottom - (long)SCREEN_HEIGHT + (long)MOUSE_MARGIN;
					lpScreen.right = lpScreen.right - (long)MOUSE_MARGIN;
					lpScreen.bottom = lpScreen.bottom - (long)MOUSE_MARGIN;

					// �}�E�X�J�[�\���̈ړ�����
					ClipCursor(&lpScreen);

					// �}�E�X�J�[�\����\��
					ShowCursor(false);
				}
				else
				{
					// �}�E�X�J�[�\���\��
					ShowCursor(true);
				}

				dwExecLastTime = dwCurrentTime;

				// �X�V����
				JoyconUpdate();
				Update();
				//pollLoop();

				// �`�揈��
				Draw();

				dwFrameCount++;
			}
		}
	}

	// �Q�[���I���t���O
	EndGame();

	// �X���b�h1�̏������I���܂őҋ@
	t1.join();

	DestroyWindow(hWnd);

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	timeEndPeriod(1);				// ����\��߂�

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	//case WM_KEYDOWN:
	//	switch (wParam)
	//	{
	//	case VK_ESCAPE:
	//		DestroyWindow(hWnd);
	//		break;
	//	}
	//	break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// Direct3D�I�u�W�F�N�g�̍쐬
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));			// ���[�N���[���N���A
	d3dpp.BackBufferCount = 1;					// �o�b�N�o�b�t�@�̐�
	d3dpp.BackBufferWidth = SCREEN_WIDTH;		// �Q�[����ʃT�C�Y(��)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		// �Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferFormat = d3ddm.Format;		// �o�b�N�o�b�t�@�t�H�[�}�b�g�̓f�B�X�v���C���[�h�ɍ��킹�Ďg��
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// �f���M���ɓ������ăt���b�v����
	d3dpp.Windowed = bWindow;					// �E�B���h�E���[�h
	d3dpp.EnableAutoDepthStencil = TRUE;		// �f�v�X�o�b�t�@�i�y�o�b�t�@�j�ƃX�e���V���o�b�t�@���쐬
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;// �f�v�X�o�b�t�@�Ƃ���24�r�b�gZ�o�b�t�@8�r�b�g�X�e���V���o�b�t�@�쐬

	if (bWindow)
	{// �E�B���h�E���[�h
		d3dpp.FullScreen_RefreshRateInHz = 0;							// ���t���b�V�����[�g
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// �C���^�[�o��
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// �C���^�[�o��
	}
	else
	{// �t���X�N���[�����[�h
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// ���t���b�V�����[�g
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// �C���^�[�o��
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;			// �C���^�[�o��
	}

	// �f�o�C�X�I�u�W�F�N�g�̐���
	// [�f�o�C�X�쐬����]<�`��>��<���_����>���n�[�h�E�F�A�ōs�Ȃ�
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pD3DDevice)))
	{
		// ��L�̐ݒ肪���s������
		// [�f�o�C�X�쐬����]<�`��>���n�[�h�E�F�A�ōs���A<���_����>��CPU�ōs�Ȃ�
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pD3DDevice)))
		{
			// ��L�̐ݒ肪���s������
			// [�f�o�C�X�쐬����]<�`��>��<���_����>��CPU�ōs�Ȃ�
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &g_pD3DDevice)))
			{
				// ���������s
				return E_FAIL;
			}
		}
	}

	// �����_�[�X�e�[�g�p�����[�^�̐ݒ�
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// ���ʂ��J�����O
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Z�o�b�t�@���g�p

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// ���u�����h���s��
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// ���f�X�e�B�l�[�V�����J���[�̎w��

	// �T���v���[�X�e�[�g�p�����[�^�̐ݒ�
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// �e�N�X�`���A�h���b�V���O���@(U�l)��ݒ�
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// �e�N�X�`���A�h���b�V���O���@(V�l)��ݒ�
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// �e�N�X�`���k���t�B���^���[�h��ݒ�
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// �e�N�X�`���g��t�B���^���[�h��ݒ�

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// �A���t�@�u�����f�B���O������ݒ�
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// �ŏ��̃A���t�@����
	//g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// �Q�Ԗڂ̃A���t�@����
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);	// �Q�Ԗڂ̃A���t�@����


	// imgui�̏�����
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui_ImplDX9_Init(g_pD3DDevice);
	//ImGui::StyleColorsDark();

	// �V�[���̏���������
	SceneManager::Init(hInstance, hWnd);

	start();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// imgui�̉��
	//ImGui_ImplDX9_Shutdown();

	if (g_pD3DDevice != NULL)
	{// �f�o�C�X�I�u�W�F�N�g�̊J��
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if (g_pD3D != NULL)
	{// Direct3D�I�u�W�F�N�g�̊J��
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// �V�[���̏I������
	SceneManager::Uninit();
	actuallyQuit();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// imgui�̍X�V
	//ImGui::NewFrame();
	//ImGui_ImplDX9_NewFrame();
	//ImGui::Begin("Another Window", &show_another_window);
	//ImGui::Text("Hello from another window!");
	//ImGui::End();

	// �X�V����
	{	
		//Debugtimer timer2;
		SceneManager::Update();
		//PrintDebugProc("�yUpdateAt�z\n[%f]\n", timer2.End());
	}

	// imgui�̍X�V�I��
	//ImGui::EndFrame();
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �X�e���V���o�b�t�@���o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// �`��̊J�n
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		//Debugtimer timer;
		// �`�揈��
		SceneManager::Draw();
		//PrintDebugProc("�yDrawAt�z\n[%f]\n", timer.End());

		// �`��̏I��
		g_pD3DDevice->EndScene();
	}

	// imgui�̕`��
	//ImGui::Render();

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//=============================================================================
// �Q�[�������֐�
//=============================================================================
void MainLoop(HWND hWnd)
{
}

//=============================================================================
// �Q�[���O�����֐�
//=============================================================================
void SubLoop(void)
{
	while (g_bContinue)
	{
		pollLoop();
	}
}

//=============================================================================
// �Q�[���I���֐�
//=============================================================================
void EndGame(void)
{
	g_bContinue = false;
}

//=============================================================================
// �f�o�C�X�̎擾
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}
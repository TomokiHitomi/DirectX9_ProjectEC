//=============================================================================
//
// �f�o�b�O�\������ [debugproc.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include <stdio.h>
#include "debugproc.h"
#include "input.h"
#include "scene.h"
#include "calculate.h"
#include "debugobject.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPD3DXFONT	g_pD3DXFont = NULL;			// �t�H���g�ւ̃|�C���^
char		g_aStrDebug[4096 * 10] = {"\0"};	// �f�o�b�O���
long		g_lDrawPos = 0;
bool		g_bDispDebug = true;
int			g_nDispColor = 0;
D3DCOLOR	g_xColor = SetColorPallet(COLOR_PALLET_RED);

extern SceneManager		g_cScene;					// Scene�}�l�[�W��

//=============================================================================
// �f�o�b�O�\�������̏�����
//=============================================================================
HRESULT InitDebugProc(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	HRESULT hr;

	// ���\���p�t�H���g��ݒ�
	hr = D3DXCreateFont(pDevice,
		20,					// �����̍���
		0,					// ������
		FW_MEDIUM, 			// �t�H���g�̑���
		2,					// MIPMAP�̃��x��
		FALSE,				// �C�^���b�N�t���O
		SHIFTJIS_CHARSET,	// �����Z�b�g
		OUT_DEFAULT_PRECIS,	// �o�͐��x
		DEFAULT_QUALITY,	// �o�͕i��
		DEFAULT_PITCH, 		// �t�H���g�s�b�`�ƃt�@�~��
		"Terminal",			// �t�H���g��
		&g_pD3DXFont);		// �|�C���^

	// ���N���A
	memset(g_aStrDebug, 0, sizeof g_aStrDebug);

	return hr;
}

//=============================================================================
// �f�o�b�O�\�������̏I������
//=============================================================================
void UninitDebugProc(void)
{
	if(g_pD3DXFont != NULL)
	{// ���\���p�t�H���g�̊J��
		g_pD3DXFont->Release();
		g_pD3DXFont = NULL;
	}
}

//=============================================================================
// �f�o�b�O�\�������̍X�V����
//=============================================================================
void UpdateDebugProc(void)
{
	if (GetKeyboardTrigger(DIK_F1))
	{// �f�o�b�O�\��ON/OFF
		g_bDispDebug = g_bDispDebug ? false : true;
		DebugObject::SetUse(g_bDispDebug);
	}
	if (GetKeyboardPress(DIK_LSHIFT))
	{
		if		(GetKeyboardTrigger(DIK_1)) g_xColor = SetColorPallet(COLOR_PALLET_WHITE);
		else if (GetKeyboardTrigger(DIK_2)) g_xColor = SetColorPallet(COLOR_PALLET_BLACK);
		else if (GetKeyboardTrigger(DIK_3)) g_xColor = SetColorPallet(COLOR_PALLET_GRAY);
		else if (GetKeyboardTrigger(DIK_4)) g_xColor = SetColorPallet(COLOR_PALLET_RED);
		else if (GetKeyboardTrigger(DIK_5)) g_xColor = SetColorPallet(COLOR_PALLET_GREEN);
		else if (GetKeyboardTrigger(DIK_6)) g_xColor = SetColorPallet(COLOR_PALLET_BLUE);
		else if (GetKeyboardTrigger(DIK_7)) g_xColor = SetColorPallet(COLOR_PALLET_YELLOW);
		else if (GetKeyboardTrigger(DIK_8)) g_xColor = SetColorPallet(COLOR_PALLET_MAGENTA);
		else if (GetKeyboardTrigger(DIK_9)) g_xColor = SetColorPallet(COLOR_PALLET_CYAN);
	}
	if (g_bDispDebug)
	{
		// �f�o�b�O�t�H���g�̃X�N���[��
		if (GetKeyboardPress(DIK_9) && GetKeyboardPress(DIK_0))
		{	// ���������͕s��
		}
		else if (GetKeyboardPress(DIK_9))
		{
			g_lDrawPos += (long)DEBUG_SCROLL_SPEED;
		}
		else if (GetKeyboardPress(DIK_0))
		{
			g_lDrawPos -= (long)DEBUG_SCROLL_SPEED;
		}

		// �}�E�X�z�C�[��
		long ModUseZ = GetMobUseZ();
		if (ModUseZ != 0)
		{
			g_lDrawPos += ModUseZ;
		}
	}
}

//=============================================================================
// �f�o�b�O�\�������̕`�揈��
//=============================================================================
void DrawDebugProc(void)
{
	if (g_bDispDebug)
	{
		RECT rect = { 0, g_lDrawPos, SCREEN_WIDTH, SCREEN_HEIGHT };

		// ���݂̃V�[�����擾
		SceneManager::SCENE eScene = g_cScene.GetScene();

		// ���\���i�������j
		g_pD3DXFont->DrawText(NULL, g_aStrDebug, -1, &rect, DT_LEFT, g_xColor);

		// ���N���A
		memset(g_aStrDebug, 0, sizeof g_aStrDebug);
	}
}

//=============================================================================
// �f�o�b�O����\��
//=============================================================================
void DebugHowTo(void)
{
	PrintDebugProc("�y Debug������@ �z\n");
	PrintDebugProc("F1:�f�o�b�O�\��/��\��  F2:TITLE  F3:GAME  F4:RESULT  ");
	PrintDebugProc("9/0/�}�E�X�z�C�[��:�f�o�b�O�X�N���[��  ");
	PrintDebugProc("LSHIFT������  WASD:�J�����ړ�  ����:�g��k��  Enter:�J�������Z�b�g\n");
	PrintDebugProc("\n");
}

//=============================================================================
// �f�o�b�O�\���̓o�^
//=============================================================================
void PrintDebugProc(const char *fmt,...)
{
#if 0
	long *pParam;
	static char aBuf[256];

	pParam = (long*)&fmt;
	sprintf(aBuf, fmt, pParam[1], pParam[2], pParam[3], pParam[4],
									pParam[5], pParam[6], pParam[7], pParam[8],
									pParam[9], pParam[10], pParam[11], pParam[12]);
#else
	va_list list;			// �ψ�������������ׂɎg�p����ϐ�
	char *pCur;
	char aBuf[256]={"\0"};
	char aWk[128];

	// �ψ����ɃA�N�Z�X����O�̏�������
	va_start(list, fmt);

	pCur = (char*)fmt;
	for( ; *pCur; ++pCur)
	{
		if(*pCur != '%')
		{
			sprintf(aWk, "%c", *pCur);
		}
		else
		{
			pCur++;

			switch(*pCur)
			{
			case 'd':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%d", va_arg(list, int));
				break;

			case 'u':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%u", va_arg(list, int));
				break;

			case 'f':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%.2f", va_arg(list, double));		// double�^�Ŏw��
				break;

			case 'l':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%.2ld", va_arg(list, long));		// long�^�Ŏw��
				break;

			case 's':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%s", va_arg(list, char*));
				break;

			case 'c':
				// �ψ����ɃA�N�Z�X���Ă��̕ϐ������o������
				sprintf(aWk, "%c", va_arg(list, char));
				break;

			default:
				sprintf(aWk, "%c", *pCur);
				break;
			}
		}
		strcat(aBuf, aWk);
	}

	// �ψ����ɃA�N�Z�X������̏I������
	va_end(list);

	// �A��
	if((strlen(g_aStrDebug) + strlen(aBuf)) < sizeof g_aStrDebug - 1)
	{
		strcat(g_aStrDebug, aBuf);
	}
#endif
}



//=============================================================================
// �f�o�b�O�^�C�}�[�J�n/�ĊJ����
//=============================================================================
void Debugtimer::Restart(void)
{
	QueryPerformanceFrequency(&m_liFreq);	// �P�ʂ��K��
	QueryPerformanceCounter(&m_liStart);	// �J�n���Ԃ��擾
}

//=============================================================================
// �f�o�b�O�^�C�}�[�I������
//=============================================================================
double Debugtimer::End(void)
{
	LARGE_INTEGER liEnd;
	QueryPerformanceCounter(&liEnd);		// �I�����Ԃ��擾

	// �I�� - �J�n / �P�ʂ��v�Z�����l��Ԃ�
	return (double)(liEnd.QuadPart - m_liStart.QuadPart) / m_liFreq.QuadPart;
}
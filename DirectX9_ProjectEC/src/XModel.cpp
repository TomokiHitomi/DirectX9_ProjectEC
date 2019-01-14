//=============================================================================
//
// ���f������ [XModel.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "XModel.h"
#include "shader.h"
#include "light.h"
#include "camera.h"

// �f�o�b�O�p
#ifdef _DEBUG
#include "debugproc.h"
#endif

CHAR						g_cDirPassX[256];

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
CXModel::CXModel(void)
{
	// ���f���֌W�̏�����
	ppTexture = NULL;
	//ppMat = NULL;
	pMesh = NULL;
	pBuffMat = NULL;
	dwNumMat = 0;
	bLight = true;

	// �V�F�[�_�|�C���^�̏�����
	pEffect = NULL;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT CXModel::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pMeshPass, LPSTR pTexPass)
{
	// ���f���f�[�^�ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(
		pMeshPass,				// ���f���f�[�^
		D3DXMESH_SYSTEMMEM,		// �g�p���郁�����̃I�v�V����
		pDevice,				// �f�o�C�X
		NULL,					// ���g�p
		&pBuffMat,				// �}�e���A���f�[�^
		NULL,					// ���g�p
		&dwNumMat,				// D3DXMATERIAL�\���̂̐�
		&pMesh)))				// ���b�V���f�[�^�ւ̃|�C���^
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", pMeshPass, MB_OK);
		return E_FAIL;
	}

	if (pTexPass == NULL)
	{
		int nSearch = 0;
		for (int i = 256 - 1; i > 0; i--)
		{
			if (pMeshPass[i] == '/')
			{
				nSearch = i;
				for (i = 0; i < nSearch + 1; i++)
				{
					g_cDirPassX[i] = pMeshPass[i];
				}
				g_cDirPassX[nSearch + 1] = NULL;
				break;
			}
		}
		// �}�e���A���������o��
		D3DXMATERIAL*	d3Mat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();
		//ppMat = new D3DMATERIAL9[dwNumMat];				// ���b�V�������m��
		ppTexture = new LPDIRECT3DTEXTURE9[dwNumMat];	// �e�N�X�`�����m��
		for (int i = 0; i < dwNumMat; i++)
		{
			//ppMat[i] = d3Mat[i].MatD3D;			// �}�e���A�����Z�b�g
			//ppMat[i].Ambient = ppMat[i].Diffuse;// ����������
			ppTexture[i] = NULL;	// �e�N�X�`��������

			// �g�p���Ă���e�N�X�`��������Γǂݍ���
			if (d3Mat[i].pTextureFilename != NULL &&
				lstrlen(d3Mat[i].pTextureFilename) > 0)
			{
				CHAR TexMeshPass[255];
				strcpy_s(TexMeshPass, sizeof(TexMeshPass), g_cDirPassX);
				strcat_s(TexMeshPass, sizeof(TexMeshPass) - strlen(TexMeshPass) - strlen(d3Mat[i].pTextureFilename) - 1, d3Mat[i].pTextureFilename);
				// �e�N�X�`���ǂݍ���
				if (FAILED(D3DXCreateTextureFromFile(
					pDevice,
					TexMeshPass,
					&ppTexture[i])))
				{
					MessageBox(NULL, "X�t�@�C���̃e�N�X�`���ǂݍ��݂Ɏ��s���܂���", pTexPass, MB_OK);
					return E_FAIL;
				}
			}
		}
		strcpy_s(g_cDirPassX, sizeof(g_cDirPassX) - 1, "\0");
	}
	else
	{
		ppTexture = new LPDIRECT3DTEXTURE9[1];	// �e�N�X�`�����m��

		// �e�N�X�`���ǂݍ���
		if (FAILED(D3DXCreateTextureFromFile(
			pDevice,
			pTexPass,
			&ppTexture[0])))
		{
			MessageBox(NULL, "X�t�@�C���̃e�N�X�`���ǂݍ��݂Ɏ��s���܂���", pTexPass, MB_OK);
			return E_FAIL;
		}
	}

	// �V�F�[�_�̃A�h���X���擾
	if (pEffect == NULL) pEffect = ShaderManager::GetEffect(ShaderManager::XMODEL);

	// �}�e���A�����ɑ΂���|�C���^�̎擾
	D3DXMATERIAL *pMat;
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (unsigned int i = 0; i < (int)dwNumMat; i++)
	{
		// ����������
		//pMat[i].MatD3D.Ambient = pMat[i].MatD3D.Diffuse;
		pMat[i].MatD3D.Ambient.r = 0.5f;
		pMat[i].MatD3D.Ambient.g = 0.5f;
		pMat[i].MatD3D.Ambient.b = 0.5f;
		pMat[i].MatD3D.Ambient.a = 1.0f;
	}
}

//=============================================================================
// �}�e���A���ݒ菈���i�X�y�L�����j
//=============================================================================
void CXModel::SetMaterialSpecular(float speclar, float power)
{
	// �}�e���A�����ɑ΂���|�C���^�̎擾
	D3DXMATERIAL *pMat;
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (unsigned int i = 0; i < (int)dwNumMat; i++)
	{
		// ����������
		//pMat[i].MatD3D.Ambient = pMat[i].MatD3D.Diffuse;
		pMat[i].MatD3D.Specular.r = speclar;
		pMat[i].MatD3D.Specular.g = speclar;
		pMat[i].MatD3D.Specular.b = speclar;
		pMat[i].MatD3D.Specular.a = 1.0f;
		pMat[i].MatD3D.Power = power;
	}
}

//=============================================================================
// �������
//=============================================================================
void CXModel::Release(void)
{
	for (unsigned int i = 0; i < dwNumMat; i++)
	{
		SAFE_RELEASE(ppTexture[i]);
	}
	SAFE_DELETE_ARRAY(ppTexture);
	//SAFE_DELETE_ARRAY(ppMat);

	SAFE_RELEASE(pMesh);
	SAFE_RELEASE(pBuffMat);
}

//=============================================================================
// �X�V����
//=============================================================================
void CXModel::Update(void)
{
}

//=============================================================================
// �`�揈��
//=============================================================================
void CXModel::Draw(D3DXMATRIX mtxWorld)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATERIAL *pMat;
	D3DMATERIAL9 pMatDef;
	HRESULT hr;

	// �r���[�E�v���W�F�N�V�����s����擾
	D3DXMATRIX mtxView, mtxProjection;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	// �}�e���A�����ɑ΂���|�C���^�̎擾
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	bool bSetEffect = false;

	//���b�V���̕`��
	for (int i = 0; i < (int)dwNumMat; i++)
	{
		// �g�p����e�N�j�b�N���`
		if (bLight)
		{	// ���C�gON
			if (ppTexture[i]) hr = pEffect->SetTechnique("LIGHT_ON_TEX");
			else hr = pEffect->SetTechnique("LIGHT_ON");
		}
		else
		{	// ���C�gOFF
			if (ppTexture[i]) hr = pEffect->SetTechnique("LIGHT_OFF_TEX");
			else hr = pEffect->SetTechnique("LIGHT_OFF");
		}
		// �G���[
		if (FAILED(hr)) MessageBox(NULL, "�e�N�j�b�N�̒�`�Ɏ��s���܂����B", "XMODEL", MB_OK);

		if (!bSetEffect)
		{
			// �K�v�ȍs������Z�b�g
			pEffect->SetMatrix("proj", &mtxProjection);
			pEffect->SetMatrix("view", &mtxView);
			pEffect->SetMatrix("world", &mtxWorld);

			if (bLight)
			{	// ���C�gON
				Camera* pCamera = CameraManager::GetCameraNow();
				D3DXVECTOR4 eyeTmp = D3DXVECTOR4(pCamera->GetEye(), 0.0f);
				if (FAILED(pEffect->SetVector("eye", &eyeTmp)))
				{
					// �G���[
					MessageBox(NULL, "�J����Eye���̃Z�b�g�Ɏ��s���܂����B", "eye", MB_OK);
				}
				// ���C�g�����擾
				Light* pLight = LightManager::GetLightAdr(LightManager::Main);
				// ���C�g�����Z�b�g
				if (FAILED(pEffect->SetValue("lt", &pLight->value, sizeof(Light::LIGHTVALUE))))
				{
					// �G���[
					MessageBox(NULL, "���C�g���̃Z�b�g�Ɏ��s���܂����B", "lt", MB_OK);
				}
				//SetShaderLight(pEffect, GetLight(0));
			}
		}

		// �}�e���A�����Z�b�g
		pEffect->SetValue("mat", &pMat[i].MatD3D, sizeof(D3DMATERIAL9));

		// �e�N�X�`�����Z�b�g
		pEffect->SetTexture("tex", ppTexture[i]);

		// ���ʂ��m�肳����
		pEffect->CommitChanges();

		// �V�F�[�_�[�̊J�n�AnumPass�ɂ͎w�肵�Ă���e�N�j�b�N�ɒ�`���Ă���pass�̐����ς���
		UINT numPass = 0;
		pEffect->Begin(&numPass, 0);
		for (unsigned int j = 0; j < numPass; j++)
		{
			// �p�X���w�肵�ĊJ�n
			pEffect->BeginPass(0);

			// �`��
			pMesh->DrawSubset(i);

			// �V�F�[�_�[�p�X���I��
			pEffect->EndPass();
		}
		// �V�F�[�_�[���I��
		pEffect->End();
	}

	// �Œ�@�\�ɖ߂�
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}
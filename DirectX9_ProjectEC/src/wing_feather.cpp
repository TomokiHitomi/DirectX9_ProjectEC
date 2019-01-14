//=============================================================================
//
// �E�B���O�t�F�U�[���� [wing_feather.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "wing_feather.h"
#include "input.h"
#include "calculate.h"
#include "camera.h"
#include "shader.h"

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
LPCTSTR g_lpWingFeatherEH[] = {
	"tex",
	"proj",
	"view",
	"world",
	"g_fMove",
	"g_fMorph"
};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
WingFeather::WingFeather()
{
	// �e�|�C���^�̏�����
	m_pTexture = NULL;	// �e�N�X�`��
	m_pVtxBuff = NULL;	// ���_�o�b�t�@
	m_pInstBuff = NULL;	// �C���X�^���V���O�o�b�t�@
	m_pIdxBuff = NULL;	// �C���f�b�N�X�o�b�t�@
	m_pDecl = NULL;		// ���_�錾

	// �V�F�[�_�֘A�̏�����
	pEffect = NULL;		// �V�F�[�_

	// �J�E���^�̏�����
	m_nCount = 0;

	// ���[�t��������
	m_fMorph = 1.0f;

	// ����������
	Init();
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
WingFeather::~WingFeather()
{
	// �������
	Release();
}

//=============================================================================
// ����������
//=============================================================================
HRESULT WingFeather::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �V�F�[�_�̃A�h���X���擾
	pEffect = ShaderManager::GetEffect(ShaderManager::WING_FEATHER);

	for (UINT i = 0; i < EH_MAX; i++)
	{
		// �V�F�[�_�̃n���h����������
		hEffectHandle[i] = NULL;
		// �V�F�[�_�̃n���h�����擾
		hEffectHandle[i] = pEffect->GetParameterByName(NULL, g_lpWingFeatherEH[i]);
		// �V�F�[�_�̃n���h���擾�̃G���[�`�F�b�N
		if (hEffectHandle[i] == NULL) MessageBox(NULL, "�V�F�[�_�n���h���̎擾���s", g_lpWingFeatherEH[i], MB_OK);
	}


	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		WING_FEATHER_TEXTURE,
		&m_pTexture)))
	{
		// �G���[
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", WING_FEATHER_TEXTURE, MB_OK);
		return S_FALSE;
	}

	// ���_���̍쐬�i���[�J�����W�̐ݒ�j
	MakeVertex(pDevice);
	return S_OK;
}

//=============================================================================
// �������
//=============================================================================
void WingFeather::Release(void)
{
	// �������
	SAFE_RELEASE(m_pTexture);		// �e�N�X�`��
	SAFE_RELEASE(m_pVtxBuff);		// ���_�o�b�t�@
	SAFE_RELEASE(m_pInstBuff);	// �C���X�^���V���O�o�b�t�@
	SAFE_RELEASE(m_pIdxBuff);		// �C���f�b�N�X�o�b�t�@
	SAFE_RELEASE(m_pDecl);		// ���_�錾
}

//=============================================================================
// �X�V����
//=============================================================================
void WingFeather::Update(void)
{
#ifdef _DEBUG
	PrintDebugProc("�y WING_FEATHER �z\n");
#endif
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	m_fMorph += WING_FEATHER_MORPH_SPEED;

#ifdef _DEBUG
	PrintDebugProc("ParticleMax:%d\n", m_nCount);
	PrintDebugProc("\n");
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void WingFeather::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �r���[�E�v���W�F�N�V�����s����擾
	D3DXMATRIX mtxWorld, mtxView, mtxProjection;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	// ���[���h�}�g���N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �r���[�s��̋t�s����i�[�i�r���{�[�h���j
	mtxWorld._11 = mtxView._11;
	mtxWorld._12 = mtxView._21;
	mtxWorld._13 = mtxView._31;
	mtxWorld._21 = mtxView._12;
	mtxWorld._22 = mtxView._22;
	mtxWorld._23 = mtxView._32;
	mtxWorld._31 = mtxView._13;
	mtxWorld._32 = mtxView._23;
	mtxWorld._33 = mtxView._33;

	// �C���X�^���X�錾
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nCount);
	pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	// ���_�ƃC���f�b�N�X��ݒ�
	pDevice->SetVertexDeclaration(m_pDecl);
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX));		// ���_�o�b�t�@
	pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// �C���X�^���X�o�b�t�@
	pDevice->SetIndices(m_pIdxBuff);										// �C���f�b�N�X�o�b�t�@

	// �g�p����e�N�j�b�N���`
	if (FAILED(pEffect->SetTechnique("Tec01")))
	{
		// �G���[
		MessageBox(NULL, "�e�N�j�b�N�̒�`�Ɏ��s���܂���", "Tec01", MB_OK);
		//return S_FALSE;
	}

	// �e�N�X�`�����Z�b�g
	SHR(pEffect->SetTexture(hEffectHandle[EH_TEX], m_pTexture), g_lpWingFeatherEH[EH_TEX]);

	// �e�N�X�`���̕��������Z�b�g
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_X], (FLOAT)WING_FEATHER_TEXTURE_DIVIDE_X);
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_Y], (FLOAT)WING_FEATHER_TEXTURE_DIVIDE_Y);

	// �K�v�ȍs������Z�b�g
	SHR(pEffect->SetMatrix(hEffectHandle[EH_PROJ], &mtxProjection), g_lpWingFeatherEH[EH_PROJ]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_VIEW], &mtxView), g_lpWingFeatherEH[EH_VIEW]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_WORLD], &mtxWorld), g_lpWingFeatherEH[EH_WORLD]);

	// ���[�t�l��ݒ�
	SHR(pEffect->SetFloat(hEffectHandle[EH_MORPH], m_fMorph), g_lpWingFeatherEH[EH_MORPH]);

	// �ړ��ʂ��Z�b�g
	SHR(pEffect->SetFloat(hEffectHandle[EH_MOVE], WING_FEATHER_MOVE), g_lpWingFeatherEH[EH_MOVE]);


	// ���ʂ��m�肳����
	pEffect->CommitChanges();

	// �V�F�[�_�[�̊J�n�AnumPass�ɂ͎w�肵�Ă���e�N�j�b�N�ɒ�`���Ă���pass�̐����ς���
	UINT numPass = 0;
	pEffect->Begin(&numPass, D3DXFX_DONOTSAVESTATE);

	for (UINT iPass = 0; iPass < numPass; iPass++)
	{
		// �p�X���w�肵�ĊJ�n
		pEffect->BeginPass(iPass);

		// �|���S���̕`��
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		// �V�F�[�_�[�p�X���I��
		pEffect->EndPass();
	}

	// �V�F�[�_�[���I��
	pEffect->End();

	// �C���X�^���X�錾��W���ɖ߂�
	pDevice->SetStreamSourceFreq(0, 1);
	pDevice->SetStreamSourceFreq(1, 1);

	// �Œ�@�\�ɖ߂�
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
}


//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT WingFeather::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
{
	/***** ���_�o�b�t�@�ݒ� *****/

	// ���_�o�b�t�@����
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		0,									// ���_�o�b�t�@�̎g�p�@�@
		0,									// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&m_pVtxBuff,							// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].vtx = D3DXVECTOR4(-WING_FEATHER_SIZE_X, WING_FEATHER_SIZE_Y, 0.0f, 1.0f);
		pVtx[1].vtx = D3DXVECTOR4(WING_FEATHER_SIZE_X, WING_FEATHER_SIZE_Y, 0.0f, 1.0f);
		pVtx[2].vtx = D3DXVECTOR4(-WING_FEATHER_SIZE_X, -WING_FEATHER_SIZE_Y, 0.0f, 1.0f);
		pVtx[3].vtx = D3DXVECTOR4(WING_FEATHER_SIZE_X, -WING_FEATHER_SIZE_Y, 0.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		int x = 0 % WING_FEATHER_TEXTURE_DIVIDE_X;
		int y = 0 / WING_FEATHER_TEXTURE_DIVIDE_X;
		float sizeX = 1.0f / WING_FEATHER_TEXTURE_DIVIDE_X;
		float sizeY = 1.0f / WING_FEATHER_TEXTURE_DIVIDE_Y;

		pVtx[0].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
		pVtx[1].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
		pVtx[2].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
		pVtx[3].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

		// ���_�J���[�̐ݒ�
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ���_�f�[�^���A�����b�N����
		m_pVtxBuff->Unlock();
	}


	/***** �C���f�b�N�X�o�b�t�@�ݒ� *****/

	WORD wIndex[6] = { 0, 1, 2, 2, 1, 3 };

	// ���W�o�b�t�@����
	if (FAILED(pDevice->CreateIndexBuffer(
		sizeof(wIndex),						// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		0,									// ���_�o�b�t�@�̎g�p�@�@
		D3DFMT_INDEX16,						// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&m_pIdxBuff,							// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{// ���W�o�b�t�@�̒��g�𖄂߂�
		WORD *pIdx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		// ���_���W�̐ݒ�
		for (unsigned int i = 0; i < 6; i++, pIdx++)
		{
			*pIdx = wIndex[i];
		}

		// ���_�f�[�^���A�����b�N����
		m_pIdxBuff->Unlock();
	}


	/***** �C���X�^���V���O�p�̍��W�o�b�t�@�ݒ� *****/

	// ���W�o�b�t�@����
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * WING_FEATHER_MAX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		0,									// ���_�o�b�t�@�̎g�p�@�@
		0,									// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&m_pInstBuff,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{// �C���X�^���V���O�o�b�t�@�̒��g�𖄂߂�
		INSTANCE *pInst;


		// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

		// ���W�̐ݒ�
		for (unsigned int i = 0; i < WING_FEATHER_MAX; i++, pInst++)
		{
			pInst->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			pInst->vec = RandVector();
			pInst->morph = 0.0f;
			pInst->shiftuv.x = 1.0f / WING_FEATHER_TEXTURE_DIVIDE_X * (int)(i % WING_FEATHER_TEXTURE_DIVIDE_X);
			pInst->shiftuv.y = 1.0f / WING_FEATHER_TEXTURE_DIVIDE_Y * (int)((i % WING_FEATHER_TEXTURE_NUM) / WING_FEATHER_TEXTURE_DIVIDE_Y);
		}

		// ���W�f�[�^���A�����b�N����
		m_pInstBuff->Unlock();
	}


	D3DVERTEXELEMENT9 declElems[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },		// VTX
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// UV
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },		// �J���[

		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// ���[���h�ʒu
		{ 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// �ړ��x�N�g��
		{ 1, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// ���[�t
		{ 1, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },		// �ǉ�UV
		D3DDECL_END()
	};

	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);

	return S_OK;
}

//=============================================================================
// �ݒu����
//=============================================================================
void WingFeather::Set(D3DXVECTOR3 pos)
{
	{// ���W�o�b�t�@�̒��g�𖄂߂�
		INSTANCE *pInst;

		// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

		for (unsigned int i = 0; i < WING_FEATHER_MAX; i++, pInst++)
		{
			if (m_fMorph - pInst->morph > 1.0f)
			{
				// ���W�̐ݒ�
				pInst->pos = pos + RandVector() * (rand() % WING_FEATHER_SET_LENGTH);
				pInst->morph = m_fMorph;
				if (i >= m_nCount)
				{
					m_nCount = i + 1;
				}
				break;
			}
		}
		// ���W�f�[�^���A�����b�N����
		m_pInstBuff->Unlock();
	}
}
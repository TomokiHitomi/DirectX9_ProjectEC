//=============================================================================
//
// �N���E�h���� [cloud.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "cloud.h"
#include "input.h"
#include "calculate.h"
#include "camera.h"
#include "shader.h"
#include "player.h"

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
LPCTSTR g_lpCloudEH[] = {
	"tex",
	"proj",
	"view",
	"world",
	"g_vMove",
	"g_fMorph",
	"g_vPos"
};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
Cloud::Cloud()
{
	// �I�u�W�F�N�gID�ƃv���C�I���e�B�̐ݒ菈��
	SetIdAndPriority(ObjectID::CLOUD, Priority::Middle, Priority::Low);

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

	m_fMove = 0.0f;

	m_vPos = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	m_vMove = D3DXVECTOR4(0.0f, 0.0f, 0.0f, CLOUD_MAX_X * CLOUD_POS_MARGIN);

	// ����������
	Init();
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Cloud::~Cloud()
{
	// �������
	Release();
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Cloud::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �V�F�[�_�̃A�h���X���擾
	pEffect = ShaderManager::GetEffect(ShaderManager::CLOUD);

	for (UINT i = 0; i < EH_MAX; i++)
	{
		// �V�F�[�_�̃n���h����������
		hEffectHandle[i] = NULL;
		// �V�F�[�_�̃n���h�����擾
		hEffectHandle[i] = pEffect->GetParameterByName(NULL, g_lpCloudEH[i]);
		// �V�F�[�_�̃n���h���擾�̃G���[�`�F�b�N
		if (hEffectHandle[i] == NULL) MessageBox(NULL, "�V�F�[�_�n���h���̎擾���s", g_lpCloudEH[i], MB_OK);
	}

	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		CLOUD_TEXTURE,
		&m_pTexture)))
	{
		// �G���[
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", CLOUD_TEXTURE, MB_OK);
		return S_FALSE;
	}

	// ���_���̍쐬�i���[�J�����W�̐ݒ�j
	MakeVertex(pDevice);
	return S_OK;
}

//=============================================================================
// �������
//=============================================================================
void Cloud::Release(void)
{
	// �������
	SAFE_RELEASE(m_pTexture);		// �e�N�X�`��
	SAFE_RELEASE(m_pVtxBuff);		// ���_�o�b�t�@
	SAFE_RELEASE(m_pInstBuff);		// �C���X�^���V���O�o�b�t�@
	SAFE_RELEASE(m_pIdxBuff);		// �C���f�b�N�X�o�b�t�@
	SAFE_RELEASE(m_pDecl);			// ���_�錾
}

//=============================================================================
// �X�V����
//=============================================================================
void Cloud::Update(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//m_fMorph += CLOUD_MORPH_SPEED;
	//m_fMove += 0.2f;

	D3DXVECTOR4 vTemp = m_vPos;
	m_vPos = D3DXVECTOR4(PlayerManager::GetPos(PlayerManager::PLAYER_1P),0.0f);

	m_vMove.x += vTemp.x - m_vPos.x;
	if (m_vMove.x < 0.0f) m_vMove.x += m_vMove.w;
	m_vMove.z += vTemp.z - m_vPos.z;
	if (m_vMove.z < 0.0f) m_vMove.z += m_vMove.w;

	m_vPosSet = m_vPos;
	m_vPosSet.x -= m_vMove.w / 2;
	m_vPosSet.z -= m_vMove.w / 2;

#ifdef _DEBUG
	PrintDebugProc("�y CLOUD �z\n");
	PrintDebugProc("Pos [%f,%f,%f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
	PrintDebugProc("Move[%f,%f,%f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void Cloud::Draw(void)
{

	D3DXMATRIX mtxWorld, mtxView, mtxProjection;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// Up��Y�̃r���[�s����擾
	mtxView = CameraManager::GetCameraNow()->GetMtxViewUpY();
	// �v���W�F�N�V�����s����擾
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

	// �r���[�s����Ď擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �C���X�^���X�錾
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | CLOUD_MAX);
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
	SHR(pEffect->SetTexture(hEffectHandle[EH_TEX], m_pTexture), g_lpCloudEH[EH_TEX]);

	// �e�N�X�`���̕��������Z�b�g
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_X], (FLOAT)CLOUD_TEXTURE_DIVIDE_X);
	//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_Y], (FLOAT)CLOUD_TEXTURE_DIVIDE_Y);

	// �K�v�ȍs������Z�b�g
	SHR(pEffect->SetMatrix(hEffectHandle[EH_PROJ], &mtxProjection), g_lpCloudEH[EH_PROJ]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_VIEW], &mtxView), g_lpCloudEH[EH_VIEW]);
	SHR(pEffect->SetMatrix(hEffectHandle[EH_WORLD], &mtxWorld), g_lpCloudEH[EH_WORLD]);

	// ���[�t�l��ݒ�
	SHR(pEffect->SetFloat(hEffectHandle[EH_MORPH], m_fMorph), g_lpCloudEH[EH_MORPH]);

	// �ړ��ʂ��Z�b�g
	SHR(pEffect->SetVector(hEffectHandle[EH_DATA], &m_vMove), g_lpCloudEH[EH_DATA]);

	// �ݒu���W���Z�b�g
	SHR(pEffect->SetVector(hEffectHandle[EH_POS], &m_vPosSet), g_lpCloudEH[EH_POS]);

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
HRESULT Cloud::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
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
		pVtx[0].vtx = D3DXVECTOR4(-CLOUD_SIZE_X, CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[1].vtx = D3DXVECTOR4(CLOUD_SIZE_X, CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[2].vtx = D3DXVECTOR4(-CLOUD_SIZE_X, -CLOUD_SIZE_Y, 0.0f, 1.0f);
		pVtx[3].vtx = D3DXVECTOR4(CLOUD_SIZE_X, -CLOUD_SIZE_Y, 0.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		int x = 0 % CLOUD_TEXTURE_DIVIDE_X;
		int y = 0 / CLOUD_TEXTURE_DIVIDE_X;
		float sizeX = 1.0f / CLOUD_TEXTURE_DIVIDE_X;
		float sizeY = 1.0f / CLOUD_TEXTURE_DIVIDE_Y;

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
		sizeof(INSTANCE) * CLOUD_MAX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
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
		for (unsigned int i = 0; i < CLOUD_MAX_X; i++)
		{
			for (unsigned int j = 0; j < CLOUD_MAX_Y; j++)
			{
				for (unsigned int k = 0; k < CLOUD_MAX_Z; k++)
				{
					pInst->pos = D3DXVECTOR3(
						i * CLOUD_POS_MARGIN,
						j * CLOUD_POS_MARGIN_Y,
						k * CLOUD_POS_MARGIN);
					pInst->pos += RandVector() * CLOUD_POS_RANDOM_VECTOR;
					pInst->vec = RandVector();
					pInst->rot = (float)(rand() % 618) * 0.01f;
					pInst->shiftuv.x = 1.0f / CLOUD_TEXTURE_DIVIDE_X * (int)(rand() % CLOUD_TEXTURE_DIVIDE_X);
					pInst->shiftuv.y = 1.0f / CLOUD_TEXTURE_DIVIDE_Y * (int)(rand() % CLOUD_TEXTURE_DIVIDE_Y);
					pInst++;
				}
			}
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

////=============================================================================
//// �ݒu����
////=============================================================================
//void Cloud::Set(D3DXVECTOR3 pos)
//{
//	{// ���W�o�b�t�@�̒��g�𖄂߂�
//		INSTANCE *pInst;
//
//		// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
//		m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);
//
//		for (unsigned int i = 0; i < CLOUD_MAX; i++, pInst++)
//		{
//			if (m_fMorph - pInst->rot > 1.0f)
//			{
//				// ���W�̐ݒ�
//				pInst->pos = pos + RandVector() * (rand() % CLOUD_SET_LENGTH);
//				pInst->rot = m_fMorph;
//				if (i >= m_nCount)
//				{
//					m_nCount = i + 1;
//				}
//				break;
//			}
//		}
//
//		// ���W�f�[�^���A�����b�N����
//		m_pInstBuff->Unlock();
//	}
//}
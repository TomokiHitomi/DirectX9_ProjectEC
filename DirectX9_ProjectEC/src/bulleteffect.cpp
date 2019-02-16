//=============================================================================
//
// �o���b�g�G�t�F�N�g���� [bulleteffect.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "bulleteffect.h"
#include "shader.h"
#include "light.h"
#include "camera.h"
#include "calculate.h"
#include "tool.h"

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
LPCTSTR g_lpBulletEffectEH[] = {
	"tex",
	"proj",
	"view",
	"world",
	"colorpallet"
};


//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
BulletEffect::BulletEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �}�g���N�X�̏�����
	InitMatrix();

	// �C���X�^���V���O�p�f�[�^�̏�����
	InitInst();

	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		m_tData[i].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tData[i].fSize = 0.0f;
		m_tData[i].fUse = 0.0f;
		m_tData[i].fCol = 0.0f;

		m_tData[i].nCountMax = 0;
		m_tData[i].nCount = 0;
		m_tData[i].bUse = false;
	}

	// �g�p�t���O�̏�����
	m_bUse = true;
	m_nCount = 0;

	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		BULLETEFFECT_TEX,
		&m_pTexture)))
	{
		// �G���[
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", BULLETEFFECT_TEX, MB_OK);
		//return S_FALSE;
	}

	// �V�F�[�_�|�C���^�̏������E�擾
	pEffect = NULL;
	pEffect = ShaderManager::GetEffect(ShaderManager::BULLET);

	for (UINT i = 0; i < EH_MAX; i++)
	{
		// �V�F�[�_�̃n���h����������
		hEffectHandle[i] = NULL;
		// �V�F�[�_�̃n���h�����擾
		hEffectHandle[i] = pEffect->GetParameterByName(NULL, g_lpBulletEffectEH[i]);
		// �V�F�[�_�̃n���h���擾�̃G���[�`�F�b�N
		if (hEffectHandle[i] == NULL) MessageBox(NULL, "�V�F�[�_�n���h���̎擾���s", g_lpBulletEffectEH[i], MB_OK);
	}

	// ���b�V���̐���
	CreateMesh(pDevice);
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
BulletEffect::~BulletEffect(void)
{
	SAFE_RELEASE(m_pTexture);		// �e�N�X�`��
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pInstBuff);
	SAFE_RELEASE(m_pIdxBuff);
	SAFE_RELEASE(m_pDecl);

	//SAFE_DELETE_ARRAY(m_pAttrTable);
}

//=============================================================================
// �X�V����
//=============================================================================
void BulletEffect::Update(void)
{
	SetInst();
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("BulletEffect");
	if (bGui)
	{
		ImGui::Text("Count [%d]\n", m_nCount);

		//// �Ǘ��p�f�[�^�̏�����
		//for (UINT i = 0; i <= m_nCount; i++)
		//{
		//	ImGui::Text("No[%2d] Pos [%6.0f.%6.0f,%6.0f] Size[%3.0f] Rot[%3.2f] Use[%d]\n",
		//		i,m_tData[i].vPos.x, m_tData[i].vPos.y, m_tData[i].vPos.z,
		//		m_tData[i].fSize, m_tData[i].fRot, m_tData[i].bUse
		//	);
		//}
		ImGui::TreePop();
	}
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void BulletEffect::Draw(void)
{
	if (m_nCount > 0)
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
		pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nCount);
		pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

		// ���_�ƃC���f�b�N�X��ݒ�
		pDevice->SetVertexDeclaration(m_pDecl);
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX));			// ���_�o�b�t�@
		pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// �C���X�^���X�o�b�t�@
		pDevice->SetIndices(m_pIdxBuff);								// �C���f�b�N�X�o�b�t�@

			// �g�p����e�N�j�b�N���`
		if (FAILED(pEffect->SetTechnique("Tec01")))
		{
			// �G���[
			MessageBox(NULL, "�e�N�j�b�N�̒�`�Ɏ��s���܂���", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// �e�N�X�`�����Z�b�g
		SHR(pEffect->SetTexture(hEffectHandle[EH_TEX], m_pTexture), g_lpBulletEffectEH[EH_TEX]);

		// �e�N�X�`���̕��������Z�b�g
		//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_X], (FLOAT)CLOUD_TEXTURE_DIVIDE_X);
		//pEffect->SetFloat(hEffectHandle[EH_DIVIDE_Y], (FLOAT)CLOUD_TEXTURE_DIVIDE_Y);

		// �K�v�ȍs������Z�b�g
		SHR(pEffect->SetMatrix(hEffectHandle[EH_PROJ], &mtxProjection), g_lpBulletEffectEH[EH_PROJ]);
		SHR(pEffect->SetMatrix(hEffectHandle[EH_VIEW], &mtxView), g_lpBulletEffectEH[EH_VIEW]);
		SHR(pEffect->SetMatrix(hEffectHandle[EH_WORLD], &mtxWorld), g_lpBulletEffectEH[EH_WORLD]);

		// �J���[�p���b�g���Z�b�g
		SHR(pEffect->SetVectorArray(
			hEffectHandle[EH_COLORPALLET],
			Color::xColor,
			COLOR_PALLET_MAX),
			g_lpBulletEffectEH[EH_COLORPALLET]);

		// ���ʂ��m�肳����
		pEffect->CommitChanges();

		// �V�F�[�_�[�̊J�n�AnumPass�ɂ͎w�肵�Ă���e�N�j�b�N�ɒ�`���Ă���pass�̐����ς���
		UINT numPass = 0;
		pEffect->Begin(&numPass, 0);
		for (UINT i = 0; i < numPass; i++)
		{
			// �p�X���w�肵�ĊJ�n
			pEffect->BeginPass(i);

			// �`��
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
}

//=============================================================================
// �g�p��Ԃɂ���
//=============================================================================
int BulletEffect::Set(D3DXVECTOR3 vPos, float fSize, float fCol, int nCount)
{
	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		if (!m_tData[i].bUse)
		{
			m_tData[i].fSize = fSize;
			m_tData[i].fUse = 1.0f;
			m_tData[i].vPos = vPos;
			m_tData[i].fCol = fCol;

			m_tData[i].nCountMax = nCount;
			m_tData[i].nCount = nCount;
			m_tData[i].bUse = true;
			return i;
		}
	}
	return -1;
}

////=============================================================================
//// �J���[���Z�b�g
////=============================================================================
//void BulletEffect::SetColor(int nIdx, float fCol)
//{
//	if (nIdx < 0)return;
//	m_tData[nIdx].fCol = fCol;
//	return;
//}
//
////=============================================================================
//// ���W���Z�b�g
////=============================================================================
//void BulletEffect::SetPos(int nIdx, D3DXVECTOR3 vPos)
//{
//	if (nIdx < 0)return;
//	m_tData[nIdx].vPos = vPos;
//	return;
//}
//
//=============================================================================
// ���
//=============================================================================
void BulletEffect::Release(int nIdx)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_tData[nIdx].fSize = 0.0f;
	m_tData[nIdx].fCol = 0.0f;

	m_tData[nIdx].fUse = 0.0f;
	m_tData[nIdx].bUse = false;
	m_tData[nIdx].nCountMax = 0;
	m_tData[nIdx].nCount = 0;
	return;
}

//=============================================================================
// �C���X�^���V���O�p�f�[�^�̏�����
//=============================================================================
HRESULT BulletEffect::InitInst(void)
{
	m_pVtxBuff = NULL;	// ���_�o�b�t�@
	m_pInstBuff = NULL;	// �C���X�^���V���O�o�b�t�@
	m_pIdxBuff = NULL;	// �C���f�b�N�X�o�b�t�@
	m_pDecl = NULL;		// ���_�錾
	return S_OK;
}

//=============================================================================
// �}�g���N�X�̏�����
//=============================================================================
HRESULT BulletEffect::InitMatrix(void)
{
	// �}�g���N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);
	return S_OK;
}

//=============================================================================
// ���b�V���̐���
//=============================================================================
HRESULT BulletEffect::CreateMesh(LPDIRECT3DDEVICE9 pDevice)
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
		pVtx[0].vtx = D3DXVECTOR4(-1.0f, 1.0f, 0.0f, 1.0f);
		pVtx[1].vtx = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
		pVtx[2].vtx = D3DXVECTOR4(-1.0f, -1.0f, 0.0f, 1.0f);
		pVtx[3].vtx = D3DXVECTOR4(1.0f, -1.0f, 0.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		int x = 0 % BULLETEFFECT_TEXTURE_DIVIDE_X;
		int y = 0 / BULLETEFFECT_TEXTURE_DIVIDE_X;
		float sizeX = 1.0f / BULLETEFFECT_TEXTURE_DIVIDE_X;
		float sizeY = 1.0f / BULLETEFFECT_TEXTURE_DIVIDE_Y;

		pVtx[0].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
		pVtx[1].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
		pVtx[2].uv = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
		pVtx[3].uv = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

		//// ���_�J���[�̐ݒ�
		//pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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


	// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
	if (FAILED(CreateInst(pDevice)))
	{
		MessageBox(NULL, "�C���X�^���V���O�p�o�b�t�@�̐����Ɏ��s", "Bullet", MB_OK);
		//return E_FAIL;
	}

	// ���_�錾���쐬
	if (FAILED(CreateDecl(pDevice)))
	{
		MessageBox(NULL, "���_�錾�̍쐬�Ɏ��s", "Bullet", MB_OK);
		//return E_FAIL;
	}

	
	return S_OK;
}

//=============================================================================
// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
//=============================================================================
HRESULT BulletEffect::CreateInst(LPDIRECT3DDEVICE9 pDevice)
{
	// �C���X�^���V���O�o�b�t�@����
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * BULLETEFFECT_MAX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		0,									// ���_�o�b�t�@�̎g�p�@�@
		0,									// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&m_pInstBuff,						// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	// �C���X�^���V���O�o�b�t�@�Ƀf�[�^����
	SetInst();
	return S_OK;
}

//=============================================================================
// �C���X�^���V���O�o�b�t�@�Ƀf�[�^�̑��
//=============================================================================
HRESULT BulletEffect::SetInst(void)
{
	INSTANCE *pInst;

	// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

	// ���W�̐ݒ�
	for (UINT i = 0; i < BULLETEFFECT_MAX; i++)
	{
		pInst->vPos = m_tData[i].vPos;
		pInst->fSize = m_tData[i].fSize;
		pInst->fUse = m_tData[i].fUse;
		pInst->fCol = m_tData[i].fCol;

		if (m_tData[i].bUse)
		{
			m_tData[i].nCount--;
			if (m_tData[i].nCount > 0)
			{
				pInst->fSize = m_tData[i].fSize * ((float)m_tData[i].nCount / (float)m_tData[i].nCountMax);
				m_nCount = i + 1;
			}
			else
			{
				pInst->fSize = m_tData[i].fSize * ((float)m_tData[i].nCount / (float)m_tData[i].nCountMax);
				Release(i);
			}
		}
		pInst++;
	}

	// ���W�f�[�^���A�����b�N����
	m_pInstBuff->Unlock();
	return S_OK;
}

//=============================================================================
// ���_�V�F�[�_�錾�̍쐬
//=============================================================================
HRESULT BulletEffect::CreateDecl(LPDIRECT3DDEVICE9 pDevice)
{
	// �C���X�^���V���O�f�[�^�ƍ��킹�čĒ�`
	D3DVERTEXELEMENT9 declElems[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },		// VTX
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// UV

		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// ���[���h�ʒu
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// �T�C�Y
		{ 1, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// �g�p�t���O
		{ 1, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },		// �J���[�C���f�b�N�X
		D3DDECL_END()
	};

	// ���_�v�f���璸�_�V�F�[�_�錾���쐬
	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);
	return S_OK;
}
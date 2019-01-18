//=============================================================================
//
// ���ʑ̏��� [debugsphere.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "octa.h"
#include "shader.h"
#include "light.h"
#include "camera.h"
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

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
Octa::Octa(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �}�g���N�X�̏�����
	InitMatrix();

	// �C���X�^���V���O�p�f�[�^�̏�����
	InitInst();

	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		m_tData[i].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tData[i].fSize = 0.0f;
		m_tData[i].fUse = 0.0f;
		m_tData[i].fRot = ((rand() % 618) * 0.01f);
		m_tData[i].vColor = SetColorPalletRandom();

		m_tData[i].bUse = false;
	}

	// �g�p�t���O�̏�����
	m_bUse = true;
	m_nCount = 0;

	// �V�F�[�_�|�C���^�̏������E�擾
	pEffect = NULL;
	pEffect = ShaderManager::GetEffect(ShaderManager::OCTA);

	// ���b�V���̐���
	CreateMesh(pDevice);
}

//=============================================================================
// �X�V����
//=============================================================================
void Octa::Update(void)
{
	SetInst();
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Octa");
	if (bGui)
	{
		ImGui::Text("Count [%d]\n", m_nCount);

		// �Ǘ��p�f�[�^�̏�����
		for (UINT i = 0; i <= m_nCount; i++)
		{
			ImGui::Text("No[%2d] Pos [%6.0f.%6.0f,%6.0f] Size[%3.0f] Rot[%3.2] Use[%d]\n",
				i,m_tData[i].vPos.x, m_tData[i].vPos.y, m_tData[i].vPos.z,
				m_tData[i].fSize, m_tData[i].fRot, m_tData[i].bUse
			);
		}
		ImGui::TreePop();
	}
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void Octa::Draw(void)
{
	if (m_nCount > 0)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// �r���[�E�v���W�F�N�V�����s����擾
		D3DXMATRIX mtxView, mtxProjection;
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);
		pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

		// �C���X�^���X�錾
		pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nCount);
		pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

		// ���_�ƃC���f�b�N�X��ݒ�
		pDevice->SetVertexDeclaration(m_pDecl);
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, (UINT)m_dwVtxSize);			// ���_�o�b�t�@
		pDevice->SetStreamSource(1, m_pInstBuff, 0, sizeof(INSTANCE));	// �C���X�^���X�o�b�t�@
		pDevice->SetIndices(m_pIdxBuff);								// �C���f�b�N�X�o�b�t�@

			// �g�p����e�N�j�b�N���`
		if (FAILED(pEffect->SetTechnique("Tec01")))
		{
			// �G���[
			MessageBox(NULL, "�e�N�j�b�N�̒�`�Ɏ��s���܂���", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// �K�v�ȍs������Z�b�g
		pEffect->SetMatrix("proj", &mtxProjection);
		pEffect->SetMatrix("view", &mtxView);
		pEffect->SetMatrix("world", &m_mtxWorld);

		// �J���������擾
		Camera* pCamera = CameraManager::GetCameraNow();
		D3DXVECTOR4 eyeTmp = D3DXVECTOR4(pCamera->GetEye(), 0.0f);
		// �J�������_���Z�b�g
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
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVtx, 0, m_dwNumFace);

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
int Octa::Set(float fSize)
{
	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		if (!m_tData[i].bUse)
		{
			m_tData[i].fSize = fSize;
			m_tData[i].fUse = 1.0f;
			m_tData[i].bUse = true;
			return i;
		}
	}
	return -1;
}

//=============================================================================
// �J���[���Z�b�g
//=============================================================================
void Octa::SetColor(int nIdx, D3DCOLOR xColor)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vColor = xColor;
	return;
}

//=============================================================================
// ���W���Z�b�g
//=============================================================================
void Octa::SetPos(int nIdx, D3DXVECTOR3 vPos)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vPos = vPos;
	return;
}

//=============================================================================
// ��]�����Z
//=============================================================================
void Octa::AddRot(int nIdx, float fRot)
{
	if (nIdx < 0)return;
	m_tData[nIdx].fRot += fRot;
	return;
}

//=============================================================================
// ���
//=============================================================================
void Octa::Release(int nIdx)
{
	if (nIdx < 0)return;
	m_tData[nIdx].fUse = 0.0f;
	m_tData[nIdx].bUse = false;
	return;
}

//=============================================================================
// �C���X�^���V���O�p�f�[�^�̏�����
//=============================================================================
HRESULT Octa::InitInst(void)
{
	m_pVtxBuff = NULL;	// ���_�o�b�t�@
	m_pInstBuff = NULL;	// �C���X�^���V���O�o�b�t�@
	m_pIdxBuff = NULL;	// �C���f�b�N�X�o�b�t�@
	m_pDecl = NULL;		// ���_�錾

	m_dwVtxSize = 0;
	m_dwNumVtx = 0;
	m_dwNumFace = 0;

	//m_pAttrTable = NULL;
	//m_dwNumAttr = 0;
	return S_OK;
}

//=============================================================================
// �}�g���N�X�̏�����
//=============================================================================
HRESULT Octa::InitMatrix(void)
{
	// �}�g���N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);
	return S_OK;
}

//=============================================================================
// ���b�V���̐���
//=============================================================================
HRESULT Octa::CreateMesh(LPDIRECT3DDEVICE9 pDevice)
{
	// ���b�V�����ւ̃|�C���^
	LPD3DXMESH pMesh = NULL;
	LPD3DXMESH pNewMesh = NULL;

	// ���f���f�[�^�ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(
		OCTA_MODEL,				// ���f���f�[�^
		D3DXMESH_SYSTEMMEM,		// �g�p���郁�����̃I�v�V����
		pDevice,				// �f�o�C�X
		NULL,					// ���g�p
		NULL,				// �}�e���A���f�[�^
		NULL,					// ���g�p
		NULL,				// D3DXMATERIAL�\���̂̐�
		&pMesh)))				// ���b�V���f�[�^�ւ̃|�C���^
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", OCTA_MODEL, MB_OK);
		return E_FAIL;
	}

	// ���_���W�݂̂̃��b�V�����R�s�[�i�@�����s�v�j
	pMesh->CloneMeshFVF(pMesh->GetOptions(), D3DFVF_XYZ | D3DFVF_NORMAL, pDevice, &pNewMesh);
	// ���b�V�������
	pMesh->Release();

	// ���b�V�����̎擾
	GetMeshData(pNewMesh);

	// ���_�V�F�[�_�錾�̍쐬
	CreateDecl(pDevice, pNewMesh);

	// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
	if (FAILED(CreateInst(pDevice)))
	{
		MessageBox(NULL, "�C���X�^���V���O�p�o�b�t�@�̐����Ɏ��s", "Octa", MB_OK);
		//return E_FAIL;
	}
	return S_OK;
}

//=============================================================================
// ���b�V�����̎擾
//=============================================================================
HRESULT Octa::GetMeshData(LPD3DXMESH pMesh)
{
	// ���b�V���̒��_�o�b�t�@�E�C���f�b�N�X�o�b�t�@���擾
	pMesh->GetVertexBuffer(&m_pVtxBuff);
	pMesh->GetIndexBuffer(&m_pIdxBuff);

	// ���_�T�C�Y�E���_���E�ʐ����擾
	m_dwVtxSize = pMesh->GetNumBytesPerVertex();
	m_dwNumVtx = pMesh->GetNumVertices();
	m_dwNumFace = pMesh->GetNumFaces();

	///// ���b�V���̑����e�[�u�����擾
	//// ���b�V���̑����e�[�u�������擾
	//pNewMesh->GetAttributeTable(NULL, &m_dwNumAttr);
	//// �����e�[�u���̐���
	//m_pAttrTable = new D3DXATTRIBUTERANGE[m_dwNumAttr];
	//// ���b�V���̑����e�[�u�����擾
	//pNewMesh->GetAttributeTable(m_pAttrTable, &m_dwNumAttr);
	return S_OK;
}

//=============================================================================
// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
//=============================================================================
HRESULT Octa::CreateInst(LPDIRECT3DDEVICE9 pDevice)
{
	// �C���X�^���V���O�o�b�t�@����
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * OCTA_MAX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
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
HRESULT Octa::SetInst(void)
{
	INSTANCE *pInst;

	// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

	// ���W�̐ݒ�
	for (UINT i = 0; i < OCTA_MAX; i++)
	{
		pInst->vPos = m_tData[i].vPos;
		pInst->fSize = m_tData[i].fSize;
		pInst->fUse = m_tData[i].fUse;
		pInst->fRot = m_tData[i].fRot;
		pInst->vColor = m_tData[i].vColor;
		pInst++;
		if (m_tData[i].bUse) m_nCount = i + 1;
	}

	// ���W�f�[�^���A�����b�N����
	m_pInstBuff->Unlock();
	return S_OK;
}

//=============================================================================
// ���_�V�F�[�_�錾�̍쐬
//=============================================================================
HRESULT Octa::CreateDecl(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh)
{
	// ���b�V���̓��͒��_�f�[�^�̒�`���擾
	D3DVERTEXELEMENT9 declMesh[8];
	pMesh->GetDeclaration(declMesh);

	// �C���X�^���V���O�f�[�^�ƍ��킹�čĒ�`
	D3DVERTEXELEMENT9 declElems[] = {
		declMesh[0],		// POSITION
		declMesh[1],		// NORMAL
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// ���[���h�ʒu
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// �T�C�Y
		{ 1, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// �g�p�t���O
		{ 1, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },		// ���_�J���[
		{ 1, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },		// ���_�J���[
		D3DDECL_END()
	};

	// ���_�v�f���璸�_�V�F�[�_�錾���쐬
	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);
	return S_OK;
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
Octa::~Octa(void)
{
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pInstBuff);
	SAFE_RELEASE(m_pIdxBuff);
	SAFE_RELEASE(m_pDecl);

	//SAFE_DELETE_ARRAY(m_pAttrTable);
}
//=============================================================================
//
// �f�o�b�O�X�t�B�A���� [debugsphere.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#include "main.h"
#include "debugsphere.h"
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

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
DebugSphere::DebugSphere(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �}�g���N�X�̏�����
	InitMatrix();

	// �C���X�^���V���O�p�f�[�^�̏�����
	InitInst();

	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < DEBUGSPHERE_MAX; i++)
	{
		m_tData[i].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_tData[i].fSize = 0.0f;
		m_tData[i].fUse = 0.0f;
		m_tData[i].bUse = false;
	}

	// �J���[�̏�����
	m_vColor = DEBUGSPHERE_COLOR;

	// �g�p�t���O�̏�����
	m_bUse = true;

	// �V�F�[�_�|�C���^�̏������E�擾
	pEffect = NULL;
	pEffect = ShaderManager::GetEffect(ShaderManager::DEBUGSPHERE);

	// ���b�V���̐���
	CreateMesh(pDevice);
}

//=============================================================================
// �X�V����
//=============================================================================
void DebugSphere::Update(void)
{
	SetInst();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DebugSphere::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �r���[�E�v���W�F�N�V�����s����擾
	D3DXMATRIX mtxView, mtxProjection;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	// �C���X�^���X�錾
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | DEBUGSPHERE_MAX);
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

	// �x�N�g�����Z�b�g�i�J���[�j
	pEffect->SetVector("g_vColor", &m_vColor);

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

//=============================================================================
// �g�p��Ԃɂ���
//=============================================================================
int DebugSphere::Set(float fSize)
{
	// �Ǘ��p�f�[�^�̏�����
	for (UINT i = 0; i < DEBUGSPHERE_MAX; i++)
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
// ���W���Z�b�g
//=============================================================================
void DebugSphere::SetPos(int nIdx, D3DXVECTOR3 vPos)
{
	if (nIdx < 0)return;
	m_tData[nIdx].vPos = vPos;
	return;
}

//=============================================================================
// ���
//=============================================================================
void DebugSphere::Release(int nIdx)
{
	if (nIdx < 0)return;
	m_tData[nIdx].fUse = 0.0f;
	m_tData[nIdx].bUse = false;
	return;
}

//=============================================================================
// �C���X�^���V���O�p�f�[�^�̏�����
//=============================================================================
HRESULT DebugSphere::InitInst(void)
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
HRESULT DebugSphere::InitMatrix(void)
{
	// �}�g���N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �yR�z��]�𔽉f(YawPitchRoll��y,x,z)
	D3DXMATRIX mtxRot;
	D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, D3DX_PI / 2, 0.0f);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);
	return S_OK;
}

//=============================================================================
// ���b�V���̐���
//=============================================================================
HRESULT DebugSphere::CreateMesh(LPDIRECT3DDEVICE9 pDevice)
{
	// ���b�V�����ւ̃|�C���^
	LPD3DXMESH pMesh = NULL;
	LPD3DXMESH pNewMesh = NULL;

	// ���̂𐶐�
	if (FAILED(D3DXCreateSphere(pDevice,
		DEBUGSPHERE_RADIUS,		// ���̂̔��a
		DEBUGSPHERE_SLICE,		// �厲����]���Ƃ����X���C�X��
		DEBUGSPHERE_STACK,		// �厲�ɉ������X�^�b�N��
		&pMesh,				// ���b�V���̃|�C���^
		NULL					// �o�b�t�@�̃|�C���^
	)))
	{
		MessageBox(NULL, "DebugSphere�̐����Ɏ��s", "DebugSphere", MB_OK);
		return E_FAIL;
	}

	// ���_���W�݂̂̃��b�V�����R�s�[�i�@�����s�v�j
	pMesh->CloneMeshFVF(pMesh->GetOptions(), D3DFVF_XYZ, pDevice, &pNewMesh);
	// ���b�V�������
	pMesh->Release();

	// ���b�V�����̎擾
	GetMeshData(pNewMesh);

	// ���_�V�F�[�_�錾�̍쐬
	CreateDecl(pDevice, pNewMesh);

	// �C���X�^���V���O�o�b�t�@���� + �f�[�^�̑��
	if (FAILED(CreateInst(pDevice)))
	{
		MessageBox(NULL, "�C���X�^���V���O�p�o�b�t�@�̐����Ɏ��s", "DebugSphere", MB_OK);
		//return E_FAIL;
	}
	return S_OK;
}

//=============================================================================
// ���b�V�����̎擾
//=============================================================================
HRESULT DebugSphere::GetMeshData(LPD3DXMESH pMesh)
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
HRESULT DebugSphere::CreateInst(LPDIRECT3DDEVICE9 pDevice)
{
	// �C���X�^���V���O�o�b�t�@����
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(INSTANCE) * DEBUGSPHERE_MAX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
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
HRESULT DebugSphere::SetInst(void)
{
	INSTANCE *pInst;

	// ���W�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	m_pInstBuff->Lock(0, 0, (void**)&pInst, 0);

	// ���W�̐ݒ�
	for (UINT i = 0; i < DEBUGSPHERE_MAX; i++)
	{
		pInst->vPos = m_tData[i].vPos;
		pInst->fSize = m_tData[i].fSize;
		pInst->fUse = m_tData[i].fUse;
		pInst++;
	}

	// ���W�f�[�^���A�����b�N����
	m_pInstBuff->Unlock();
	return S_OK;
}

//=============================================================================
// ���_�V�F�[�_�錾�̍쐬
//=============================================================================
HRESULT DebugSphere::CreateDecl(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh)
{
	// ���b�V���̓��͒��_�f�[�^�̒�`���擾
	D3DVERTEXELEMENT9 declMesh[8];
	pMesh->GetDeclaration(declMesh);

	// �C���X�^���V���O�f�[�^�ƍ��킹�čĒ�`
	D3DVERTEXELEMENT9 declElems[] = {
		declMesh[0],		// POSITION
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },		// ���[���h�ʒu
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },		// �T�C�Y
		{ 1, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },		// �g�p�t���O
		D3DDECL_END()
	};

	// ���_�v�f���璸�_�V�F�[�_�錾���쐬
	pDevice->CreateVertexDeclaration(declElems, &m_pDecl);
	return S_OK;
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
DebugSphere::~DebugSphere(void)
{
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pInstBuff);
	SAFE_RELEASE(m_pIdxBuff);
	SAFE_RELEASE(m_pDecl);

	//SAFE_DELETE_ARRAY(m_pAttrTable);
}
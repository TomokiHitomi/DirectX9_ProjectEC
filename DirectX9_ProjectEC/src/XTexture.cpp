//=============================================================================
//
// 2D�e�N�X�`������ [XTexture.cpp]
// Author : GP12A295 25 �l���F��
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "XTexture.h"

// �f�o�b�O�p
#ifdef _DEBUG
#include "debugproc.h"
#endif

//=============================================================================
// �R���X�g���N�^�i�����������j
//=============================================================================
CXTexture::CXTexture(void)
{
	// �|�C���^�� NULL �ŏ�����
	m_pTexture = NULL;	// �e�N�X�`���o�b�t�@
	m_pVertex = NULL;	// ���_�o�b�t�@
	m_pData = NULL;		// �f�[�^

	// �g�p�t���O�� false �ŏ�����
	m_bUse = false;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT CXTexture::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPass, int nNum)
{
	// �g�p����0�ȉ��̏ꍇ�͏��������s
	if (nNum <= 0) return E_FAIL;

	// �g�p����ۊ�
	m_nNum = nNum;

	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,					// �f�o�C�X
		pTexPass,					// �t�@�C����
		&m_pTexture)))				// �ǂݍ��ރ������i�����Ȃ�z��Ɂj
	{
		return E_FAIL;
	}

	// �e�f�[�^�����̉��{������
	m_pData = new XTextureData[m_nNum];

	// ���_�̍쐬
	if (FAILED(MakeVertex(pDevice)))
	{
		return E_FAIL;
	}

	// �g�p�t���O�� true �ɂ���
	m_bUse = true;
}

//=============================================================================
// �������
//=============================================================================
void CXTexture::Release(void)
{
	SAFE_RELEASE(m_pTexture);	// �e�N�X�`���̊J��
	SAFE_RELEASE(m_pVertex);	// ���_�o�b�t�@�̊J��
}

//=============================================================================
// �X�V����
//=============================================================================
void CXTexture::Update(void)
{
	if (m_bUse)
	{
		UpdateVertex();
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void CXTexture::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();

		// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
		pDevice->SetStreamSource(0, m_pVertex, 0, sizeof(VERTEX_2D));

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, m_pTexture);

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_2D);

		for (UINT i = 0; i < m_nNum; i++)
		{
			if (m_pData[i].bUse)
			{
				// �|���S���̕`��
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i * NUM_VERTEX), NUM_POLYGON);
			}
		}
	}
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT CXTexture::MakeVertex(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * (NUM_VERTEX * m_nNum),		// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,								// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_2D,									// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,								// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&m_pVertex,										// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))											// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	return S_OK;
}


//=============================================================================
// ���_�̍X�V
//=============================================================================
HRESULT CXTexture::UpdateVertex(void)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_2D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		m_pVertex->Lock(0, 0, (void**)&pVtx, 0);

		for (UINT i = 0; i < m_nNum; i++, pVtx += 4)
		{
			if (m_pData[i].bUse)
			{
				// �p�x���v�Z
				m_pData[i].fBaseAngle = atan2f(m_pData[i].vSize.y, m_pData[i].vSize.x);
				// ���a���v�Z
				D3DXVECTOR2 temp = D3DXVECTOR2(m_pData[i].vSize.x, m_pData[i].vSize.y);
				m_pData[i].fRadius = D3DXVec2Length(&temp);
				// ���a�ɃX�P�[���l����Z
				m_pData[i].fRadius *= m_pData[i].fScl;

				// ���_���W�̐ݒ�i��]�Ή��j
				pVtx[0].vtx.x = m_pData[i].vPos.x - cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.y = m_pData[i].vPos.y - sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.z = m_pData[i].vPos.z;
				pVtx[1].vtx.x = m_pData[i].vPos.x + cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.y = m_pData[i].vPos.y - sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.z = m_pData[i].vPos.z;
				pVtx[2].vtx.x = m_pData[i].vPos.x - cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.y = m_pData[i].vPos.y + sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.z = m_pData[i].vPos.z;
				pVtx[3].vtx.x = m_pData[i].vPos.x + cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.y = m_pData[i].vPos.y + sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.z = m_pData[i].vPos.z;

				//pVtx[0].vtx = D3DXVECTOR3(m_pData[i].vPos.x - m_pData[i].vSize.x / 2, m_pData[i].vPos.y - m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[1].vtx = D3DXVECTOR3(m_pData[i].vPos.x + m_pData[i].vSize.x / 2, m_pData[i].vPos.y - m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[2].vtx = D3DXVECTOR3(m_pData[i].vPos.x - m_pData[i].vSize.x / 2, m_pData[i].vPos.y + m_pData[i].vSize.y / 2, 0.0f);
				//pVtx[3].vtx = D3DXVECTOR3(m_pData[i].vPos.x + m_pData[i].vSize.x / 2, m_pData[i].vPos.y + m_pData[i].vSize.y / 2, 0.0f);

				// rhw�̐ݒ�
				pVtx[0].rhw =
				pVtx[1].rhw =
				pVtx[2].rhw =
				pVtx[3].rhw = 1.0f;

				// ���ˌ��̐ݒ�
				pVtx[0].diffuse =
				pVtx[1].diffuse =
				pVtx[2].diffuse =
				pVtx[3].diffuse = m_pData[i].xColor;

				// �e�N�X�`�����W�̐ݒ�
				int x = m_pData[i].nTexNum % m_pData[i].tDivide.x;
				int y = m_pData[i].nTexNum / m_pData[i].tDivide.x;
				float sizeX = 1.0f / m_pData[i].tDivide.x;
				float sizeY = 1.0f / m_pData[i].tDivide.y;
				pVtx[0].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY);
				pVtx[1].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY);
				pVtx[2].tex = D3DXVECTOR2((float)(x)* sizeX, (float)(y)* sizeY + sizeY);
				pVtx[3].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX, (float)(y)* sizeY + sizeY);

				//pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
				//pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
				//pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
				//pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);
			}
		}

		// ���_�f�[�^���A�����b�N����
		m_pVertex->Unlock();
	}
	return S_OK;
}
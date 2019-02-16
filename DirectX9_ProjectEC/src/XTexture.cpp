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
#include "shader.h"
#include "file.h"

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

	// �V�F�[�_�̃A�h���X���擾
	m_pEffect = ShaderManager::GetEffect(ShaderManager::XTEXTURE);


	// �g�p�t���O�� false �ŏ�����
	m_bUse = false;

#ifdef _DEBUG
	m_bEdit = false;
#endif
}

//=============================================================================
// �f�X�g���N�^�i�I�������j
//=============================================================================
CXTexture::~CXTexture(void)
{
	Release();
}

//=============================================================================
// �������
//=============================================================================
void CXTexture::Release(void)
{
	SAFE_RELEASE(m_pTexture);	// �e�N�X�`���̊J��
	SAFE_RELEASE(m_pVertex);	// ���_�o�b�t�@�̊J��

	// �e��f�[�^�����
	SAFE_DELETE_ARRAY(m_pData);
#ifdef _DEBUG
	SAFE_DELETE_ARRAY(m_pDataInit);
#endif
}

//=============================================================================
// ����������
//=============================================================================
HRESULT CXTexture::Init(LPDIRECT3DDEVICE9 pDevice, LPSTR pTexPath, LPSTR pDataPath, int nNum, int nDivideX, int nDivideY)
{
	// �g�p����0�ȉ��̏ꍇ�͏��������s
	if (nNum <= 0) return E_FAIL;

	// �g�p����ۊ�
	m_nNum = nNum;

	// �e�N�X�`���̕�������ۊ�
	m_cDivide.x = nDivideX;
	m_cDivide.y = nDivideY;

	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(
		pDevice,					// �f�o�C�X
		pTexPath,					// �t�@�C����
		&m_pTexture)))				// �ǂݍ��ރ������i�����Ȃ�z��Ɂj
	{
		return E_FAIL;
	}

	// �e�f�[�^�����̉��{������
	m_pData = new XTextureData[m_nNum];

#ifdef _DEBUG
	m_pDataPath = pDataPath;
	m_pDataInit = new XTextureData[m_nNum];
#endif

	bool bResult = File::Load(m_pData, pDataPath, m_nNum);

	if (!bResult)
	{
#ifdef _RELEASE
		// �G���[
		MessageBox(NULL, "�������f�[�^������܂���B", pDataPath, MB_OK);
		return E_FAIL;
#endif

#ifdef _DEBUG
		for (UINT i = 0; i < m_nNum; i++)
		{
			// �e�f�[�^�̏�����
			m_pData[i].vPos = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
			m_pData[i].vSize = D3DXVECTOR2(XTEXTURE_INIT_SIZE, XTEXTURE_INIT_SIZE);
			m_pData[i].xColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_pData[i].nTexNum = 0;
			m_pData[i].fBaseAngle = 0.0f;
			m_pData[i].fRadius = 0.0f;
			m_pData[i].fScl = 1.0f;
			m_pData[i].fRot = 0.0f;
			m_pData[i].bUse = true; 
		}

		File::Save(m_pData, pDataPath, m_nNum);
#endif
	}

#ifdef _DEBUG
	for (UINT i = 0; i < m_nNum; i++)
	{
		m_pDataInit[i] = m_pData[i];
	}
#endif

	// ���_�̍쐬
	if (FAILED(MakeVertex(pDevice)))
	{
		return E_FAIL;
	}

	// �g�p�t���O�� true �ɂ���
	m_bUse = true;
}

//=============================================================================
// �X�V����
//=============================================================================
void CXTexture::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode(m_pDataPath);
	if (bGui)
	{
		if (ImGui::Button("Save"))
		{
			File::Save(m_pDataInit, m_pDataPath, m_nNum);
		}
		if (ImGui::Button("Load"))
		{
			File::Load(m_pDataInit, m_pDataPath, m_nNum);
		}

		ImGui::Checkbox("Edit", &m_bEdit);

		if (m_bEdit)
		{
			for (UINT i = 0; i < m_nNum; i++)
			{
				m_pData[i] = m_pDataInit[i];
			}
		}

		for (UINT i = 0; i < m_nNum; i++)
		{
			CHAR cNum[256];
			cNum[0] = 'N';
			cNum[1] = 'o';
			cNum[2] = '.';
			cNum[3] = (int)'0' + i;
			cNum[4] = ' ';
			cNum[5] = NULL;

			strcat_s(cNum, sizeof(cNum) - strlen(cNum) - strlen(m_pDataPath) - 1, m_pDataPath);

			ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
			bool bGui = ImGui::TreeNode(cNum);
			if (bGui)
			{
				ImGui::Text("Slider");
				ImGui::SliderFloat2("PosS", &m_pDataInit[i].vPos.x, 0.0f, 1.0f);
				ImGui::SliderFloat4("ColorS", &m_pDataInit[i].xColor.r, 0.0f, 1.0f);
				ImGui::SliderFloat("SclS", &m_pDataInit[i].fScl, 0.0f, 2.0f);
				ImGui::SliderFloat("RotS", &m_pDataInit[i].fRot, -D3DX_PI, D3DX_PI);

				ImGui::Text("Input");
				ImGui::InputFloat2("Pos", &m_pDataInit[i].vPos.x, 3);
				ImGui::InputFloat2("Size", &m_pDataInit[i].vSize.x, 3);
				ImGui::InputFloat4("Color", &m_pDataInit[i].xColor.r, 3);
				ImGui::InputInt("TexNum", &m_pDataInit[i].nTexNum);
				ImGui::InputFloat("Scl", &m_pDataInit[i].fScl, 3);
				ImGui::InputFloat("Rot", &m_pDataInit[i].fRot, 3);

				ImGui::Text("Flag");
				ImGui::Checkbox("Use", &m_pDataInit[i].bUse);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
#endif

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

		// �r���[�E�v���W�F�N�V�����s����擾
		D3DXMATRIX mtxProjection;
		pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

		// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
		pDevice->SetStreamSource(0, m_pVertex, 0, sizeof(VERTEX_2D));

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_2D);

		// �g�p����e�N�j�b�N���`
		if (FAILED(m_pEffect->SetTechnique("Tec01")))
		{
			// �G���[
			MessageBox(NULL, "�e�N�j�b�N�̒�`�Ɏ��s���܂���", "Tec01", MB_OK);
			//return S_FALSE;
		}

		// �K�v�ȍs������Z�b�g
		m_pEffect->SetMatrix("proj", &mtxProjection);

		// �e�N�X�`���̐ݒ�
		//pDevice->SetTexture(0, m_pTexture);
		m_pEffect->SetTexture("tex", m_pTexture);

		// ���ʂ��m�肳����
		m_pEffect->CommitChanges();

		// �V�F�[�_�[�̊J�n�AnumPass�ɂ͎w�肵�Ă���e�N�j�b�N�ɒ�`���Ă���pass�̐����ς���
		UINT numPass = 0;
		m_pEffect->Begin(&numPass, 0);
		for (UINT j = 0; j < numPass; j++)
		{
			// �p�X���w�肵�ĊJ�n
			m_pEffect->BeginPass(0);

			for (UINT i = 0; i < m_nNum; i++)
			{
				if (m_pData[i].bUse)
				{
					// �|���S���̕`��
					pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i * NUM_VERTEX), NUM_POLYGON);
				}
			}

			// �V�F�[�_�[�p�X���I��
			m_pEffect->EndPass();
		}
		// �V�F�[�_�[���I��
		m_pEffect->End();

		// �Œ�@�\�ɖ߂�
		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);
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
				m_pData[i].fBaseAngle = atan2f((m_pData[i].vSize.y * SCREEN_SCALE) / 2, (m_pData[i].vSize.x * SCREEN_SCALE) / 2);
				// ���a���v�Z
				D3DXVECTOR2 temp = D3DXVECTOR2((m_pData[i].vSize.x * SCREEN_SCALE) / 2, (m_pData[i].vSize.y * SCREEN_SCALE) / 2);
				m_pData[i].fRadius = D3DXVec2Length(&temp);
				// ���a�ɃX�P�[���l����Z
				m_pData[i].fRadius *= m_pData[i].fScl;

				// ���_���W�̐ݒ�i��]�Ή��j
				pVtx[0].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) - cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) - sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[0].vtx.z = m_pData[i].vPos.z;
				pVtx[1].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) + cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) - sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[1].vtx.z = m_pData[i].vPos.z;
				pVtx[2].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) - cosf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) + sinf(m_pData[i].fBaseAngle + m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[2].vtx.z = m_pData[i].vPos.z;
				pVtx[3].vtx.x = (m_pData[i].vPos.x * SCREEN_WIDTH) + cosf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
				pVtx[3].vtx.y = (m_pData[i].vPos.y * SCREEN_HEIGHT) + sinf(m_pData[i].fBaseAngle - m_pData[i].fRot) * m_pData[i].fRadius;
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
				int x = m_pData[i].nTexNum % m_cDivide.x;
				int y = m_pData[i].nTexNum / m_cDivide.x;
				float sizeX = 1.0f / m_cDivide.x;
				float sizeY = 1.0f / m_cDivide.y;
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
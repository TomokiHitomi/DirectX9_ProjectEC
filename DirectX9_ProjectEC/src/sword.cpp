//=============================================================================
//
// ソード処理 [sword.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "sword.h"
#include "calculate.h"
#include "input.h"
#include "camera.h"
#include "playermanager.h"
#include "collision.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Sword::Sword(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ソードの初期化
	m_CXModel = new CXModel;
	m_CXModel->Init(pDevice, (LPSTR)SWORD_MODEL, NULL);
	//m_CXModel->SetMaterialSpecular(0.5f, 1.0f);

	m_pSetMtx = NULL;

	m_eMode = Sword::SHOT;

	m_nCnt = 0;
	m_nCntMax = m_nCnt;

	m_nCntRot = 0;
	m_fRot = 0.01f;
	m_fRotSpeed = SWORD_ROT_SPEED_MAX;

	// モーフ
	m_fMorph = 0.0f;
	m_fMorphGuard = 0.0f;

	m_bUse = true;

	for (unsigned int i = 0; i < SWORD_MAX + 1; i++)
	{
		//m_cSword[i].vPos = D3DXVECTOR3(0.0f + i*100.0f, 0.0f + i*100.0f, 500.0f);
		m_cSword[i].vPos = ZERO_D3DXVECTOR3;
		m_cSword[i].vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_cSword[i].vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_cSword[i].vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		m_cSword[i].eMode = SwordData::STAY;
		m_cSword[i].fLength = SWORD_ROT_LENGTH;
		m_cSword[i].fSpeed = 0.0f;
		m_cSword[i].fAngle = 0.0f;
		m_cSword[i].bUse = false;
	}

	for (UINT i = 0; i < SWORD_MAX_INIT; i++)
	{
		Add();
	}

	m_vScl = D3DXVECTOR3(SWORD_SCL, SWORD_SCL, SWORD_SCL);

	// 持ち剣を初期化
	m_cSword[SWORD_MAX].bUse = true;
	// 持ち剣のローカル情報を作成
	D3DXVECTOR3 pos, rot;
	pos = ZERO_D3DXVECTOR3;
	rot = SWORD_HAND_ROT;

	// 持ち剣の行列を作成
	WorldConvert(&m_mtxSword, pos, rot, m_vScl);
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Sword::~Sword(void)
{
	for (UINT i = 0; i < m_nCntMax; i++)
	{
		Sub();
	}

	if (m_CXModel != NULL)
	{
		m_CXModel->Release();
		delete m_CXModel;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Sword::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Sword");
	if (bGui)
	{
		ImGui::Text("CntMax[%d] Cnt[%d]\n", m_nCntMax, m_nCnt);
	}
#endif

	if (m_bUse)
	{
		// プレイヤーのポインタを取得
		Player* pPlayer = PlayerManager::GetPlayer(PlayerManager::PLAYER_1P);

		// ソードの更新処理
		D3DXVECTOR3 temp,posTemp, posTarget,vUp;
		D3DXMATRIX mtxTemp;

		// クォータニオン用
		D3DXVECTOR3 vAxis, vRadian, vX, vY, vZ;
		float fLength;


		// プレイヤーの行列を取得
		mtxTemp = pPlayer->GetMtx();
		// 取得した行列を正規化
		D3DXMatrixNormalize(&mtxTemp, &mtxTemp);

		// 行列内のベクトルを取得
		vX = D3DXVECTOR3(mtxTemp._11, mtxTemp._12, mtxTemp._13);
		vY = D3DXVECTOR3(mtxTemp._21, mtxTemp._22, mtxTemp._23);
		vZ = D3DXVECTOR3(mtxTemp._31, mtxTemp._32, mtxTemp._33);

		//vX = pPlayer->GetVecX();
		//vY = pPlayer->GetVecY();
		//vZ = pPlayer->GetVecZ();


		// プレイヤーのガードに応じてモーフ値を変動
		bool bGuard = pPlayer->GetGuard();
		if (bGuard) m_fMorphGuard += SWORD_MORPH_SPEED;
		else m_fMorphGuard -= SWORD_MORPH_SPEED;

		// プレイヤーモードに応じてモーフ値を変動
		if (pPlayer->GetMode() == Player::MODE_FLY || bGuard)
			m_fMorph += SWORD_MORPH_SPEED;
		else m_fMorph -= SWORD_MORPH_SPEED;

		// 0.0〜1.0fの範囲でクランプ
		m_fMorphGuard = saturate(m_fMorphGuard);
		m_fMorph = saturate(m_fMorph);

		// 設置座標
		
		float fPosY = SWORD_POS_Y * (1.0f - m_fMorph) + SWORD_POS_Y_FLY * m_fMorph;
		fPosY = fPosY * (1.0f - m_fMorphGuard) + SWORD_POS_Y_GUARD * m_fMorphGuard;
		posTemp = vY * fPosY;
		posTemp += pPlayer->GetPos();

		// テスト用
		posTarget = PlayerManager::GetPosTaget(PlayerManager::PLAYER_1P);

		// ソード回転処理
		Rot();
		




		vAxis = vZ * (1.0f - m_fMorphGuard) + vY * (m_fMorphGuard);
		vRadian = vX * (1.0f - m_fMorphGuard) + vZ * (m_fMorphGuard);

		CrossProduct(&vUp, &vAxis, &vRadian);

		// モーフ情報に応じて展開半径を変化
		fLength = SWORD_ROT_LENGTH * (1.0f - m_fMorph) + SWORD_ROT_LENGTH_FLY * m_fMorph;


		int nCnt = 0;
		m_fAngle = D3DX_PI * 2 / m_nCnt;

		for (unsigned int i = 0; i < m_nCntMax; i++)
		{
			
			switch (m_cSword[i].eMode)
			{
			case SwordData::RETURN:
				m_cSword[i].fAngle += ((m_fAngle * nCnt + m_fRot) - m_cSword[i].fAngle) / SWORD_ROT_ADJUST_TIME;
				QuaternionCalculate(&vRadian, &vAxis, m_cSword[i].fAngle, &temp);
				if (CheckHitBC(m_cSword[i].vPos, posTemp + temp * m_cSword[i].fLength, 100, 100))
				{
					m_cSword[i].eMode = SwordData::STAY;
					m_cSword[i].fSpeed = 0.0f;
					//break;
				}
				else
				{
					m_cSword[i].fSpeed += (fLength - m_cSword[i].fSpeed) / SWORD_SPEED_ADD;
					temp = (posTemp + temp * m_cSword[i].fLength) - m_cSword[i].vPos;
					D3DXVec3Normalize(&temp, &temp);
					m_cSword[i].vPos += temp * m_cSword[i].fSpeed;
				}
				m_cSword[i].vX = vUp;
				m_cSword[i].vZ = temp;
				nCnt++;
				break;
			case SwordData::STAY:
				m_cSword[i].fAngle += ((m_fAngle * nCnt + m_fRot) - m_cSword[i].fAngle) / SWORD_ROT_ADJUST_TIME;
				QuaternionCalculate(&vRadian, &vAxis, m_cSword[i].fAngle, &temp);

				m_cSword[i].vPos = posTemp + temp * m_cSword[i].fLength;
				m_cSword[i].fLength =
					max(fLength, m_cSword[i].fLength - SWORD_LENGTH_RETRUN_SUB);

				// プレイヤーのモードで剣の向きを変える
				// 情報を初期化
				m_cSword[i].vX = ZERO_D3DXVECTOR3;
				m_cSword[i].vZ = ZERO_D3DXVECTOR3;

				// モーフ値に応じてベクトルを設定
				// Float
				m_cSword[i].vZ += temp * (1.0f - m_fMorph);
				CrossProduct(&temp, &temp, &vAxis);
				m_cSword[i].vX += temp * (1.0f - m_fMorph);

				// Fly
				m_cSword[i].vX += temp * m_fMorph;
				m_cSword[i].vZ += -vAxis * m_fMorph;

				nCnt++;
				break;
			case SwordData::SHOT:
				m_cSword[i].fSpeed += (SWORD_SPEED_MAX - m_cSword[i].fSpeed) / SWORD_SPEED_ADD;

				temp = posTarget - m_cSword[i].vPos;
				D3DXVec3Normalize(&temp, &temp);
				temp = temp * SWORD_TRUN + m_cSword[i].vZ;
				D3DXVec3Normalize(&temp, &temp);
				m_cSword[i].vPos += temp * m_cSword[i].fSpeed;
				m_cSword[i].vZ = temp;
				break;
			}

#ifdef _DEBUG
			DebugObject::pSphere->SetPos(m_cSword[i].m_cDebug.nIdx, m_cSword[i].vPos + m_cSword[i].vZ * SWORD_COLLISION_POS);
			if (bGui)
			{
				ImGui::Text("No[%d] Use[%d] Mode[%d] Len[%f] Spd[%f]\n"
					, i, m_cSword[i].bUse, m_cSword[i].eMode,
					m_cSword[i].fLength, m_cSword[i].fSpeed);
				ImGui::Text("vX[%.2f.%.2f,%.2f] ",
					m_cSword[i].vX.x, m_cSword[i].vX.y, m_cSword[i].vX.z);
				ImGui::Text("vY[%.2f.%.2f,%.2f] ",
					m_cSword[i].vY.x, m_cSword[i].vY.y, m_cSword[i].vY.z);
				ImGui::Text("vZ[%.2f.%.2f,%.2f]\n",
					m_cSword[i].vZ.x, m_cSword[i].vZ.y, m_cSword[i].vZ.z);
			}
#endif
		}
	}

#ifdef _DEBUG
	if(bGui) ImGui::TreePop();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void Sword::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		D3DXMATRIX mtxWorld, mtxTemp;
		Player *pPlayer = ObjectManager::GetObjectPointer<Player>(ObjectManager::PLAYER);

		// 舞剣を描画
		for (unsigned int i = 0; i < m_nCntMax; i++)
		{
			WorldConvertAxis(&mtxWorld,
				m_cSword[i].vPos,
				-m_cSword[i].vZ,
				m_cSword[i].vX,
				m_vScl);

			m_CXModel->Draw(mtxWorld);
		}

		// 持ち剣処理
		if (m_pSetMtx != NULL)
		{
			// 設置先マトリクスをコピー
			mtxTemp = *m_pSetMtx;

			// マトリクスを正規化
			D3DXMatrixNormalize(&mtxTemp, &mtxTemp);
		}
		else D3DXMatrixIdentity(&mtxTemp);

		// 剣の行列にプレイヤーのボーンマトリクスをかける
		D3DXMatrixMultiply(&mtxTemp, &m_mtxSword, &mtxTemp);

		m_CXModel->Draw(mtxTemp);
	}
}

//=============================================================================
// 射出処理
//=============================================================================
void Sword::Shot(void)
{
	int nCntMax = m_nCntMax / 2;
	//if (m_nCntMax & 1)
	//{
	//	nCntMax++;
	//}
	for (unsigned int i = 0; i < nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::SHOT;
			m_cSword[i].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
		else if(m_cSword[i+ nCntMax].eMode != SwordData::SHOT)
		{
			m_cSword[i + nCntMax].eMode = SwordData::SHOT;
			m_cSword[i + nCntMax].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
	}
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::SHOT;
			m_cSword[i].fLength = SWORD_LENGTH_RETRUN;
			m_nCnt--;
			m_eMode = Sword::SHOT;
			return;
		}
	}
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::Stay(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode == SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::STAY;
			return;
		}
	}
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::StayAll(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode != SwordData::STAY)
		{
			m_cSword[i].eMode = SwordData::STAY;
			m_nCnt++;
		}
	}
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::RetrunAll(void)
{
	for (unsigned int i = 0; i < m_nCntMax; i++)
	{
		if (m_cSword[i].eMode == SwordData::SHOT)
		{
			m_cSword[i].eMode = SwordData::RETURN;
			m_nCnt++;
		}
	}
	m_eMode = Sword::RETURN;
	m_nCntRot = SWORD_ROT_SPEED_COUNT;
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::Add(void)
{
	for (unsigned int i = 0; i < SWORD_MAX; i++)
	{
		if (!m_cSword[i].bUse)
		{
			m_cSword[i].bUse = true;
			m_cSword[i].fAngle = m_fRot - D3DX_PI * 0.5f;
			m_nCnt++;
			m_nCntMax++;
#ifdef _DEBUG
			m_cSword[i].m_cDebug.bUse = true;
			m_cSword[i].m_cDebug.fSize = SWORD_COLLISION_SIZE;
			m_cSword[i].m_cDebug.nIdx = DebugObject::pSphere->Set(m_cSword[i].m_cDebug.fSize);
#endif
			return;
		}
	}
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::Sub(void)
{
	for (int i = SWORD_MAX - 1; i >= 0; i--)
	{
		if (m_cSword[i].bUse)
		{
			m_cSword[i].bUse = false;
			if (m_cSword[i].eMode != SwordData::SHOT)
			{
				m_nCnt--;
			}
#ifdef _DEBUG
			DebugObject::pSphere->Release(m_cSword[i].m_cDebug.nIdx);
#endif
			m_nCntMax--;
			return;
		}
	}
}

//=============================================================================
// 格納処理
//=============================================================================
void Sword::Rot(void)
{
	//if (m_eMode == Sword::RETURN)
	//{
	//	//m_fRotSpeed += (SWORD_ROT_SPEED_MIN - m_fRotSpeed) / SWORD_ROT_SPEED_SUB;
	//	m_fRotSpeed = SWORD_ROT_SPEED_MIN;
	//	m_nCntRot--;
	//	if (m_nCntRot < 0)
	//	{
	//		m_eMode = Sword::ROT;
	//		m_nCntRot = 0;
	//	}
	//}
	//else
	//{
	//	m_fRotSpeed += (SWORD_ROT_SPEED_MAX - m_fRotSpeed) / SWORD_ROT_SPEED_ADD;
	//}
	m_fRot += m_fRotSpeed;
}
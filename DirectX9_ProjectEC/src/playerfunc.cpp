//=============================================================================
//
// プレイヤー機能処理 [playerfunc.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "calculate.h"
#include "input.h"
#include "camera.h"
#include "sound.h"
#include "file.h"
#include "scene.h"

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
// 攻撃処理
//=============================================================================
void Player::Attack(void)
{
	//m_eMode = MODE_LOCKON;
	if (!m_bGuard)
	{
		if (!m_bChargeAttack)
		{
			if (IsMobUseLeftPressed())
			{
				m_nCharge++;
				m_bCharge = true;
				if (m_nCharge == PLAYER_ATK_CHARGE)
				{
					// チャージ完了
				}
			}
			else
			{
				if (m_nCharge > PLAYER_ATK_CHARGE)
				{
					m_bChargeAttack = true;
				}
				m_nCharge = 0;
				m_bCharge = false;
			}

			if (IsMobUseLeftTriggered() && !IsMobUseRightPressed())
			{
				if (m_eMode == MODE_FLY)
					m_eMode = MODE_FLOAT;

				m_bAttack = true;
				m_nComboCount++;
				if (m_nComboCount > PLAYER_ATK_COMBO_MAX) m_nComboCount = PLAYER_ATK_COMBO_MAX;
			}

			if (m_cSword != NULL)
			{
				// ソード射出
				if (IsMobUseRightTriggered()) m_cSword->Shot();
			}
		}
		else
		{
			m_nCharge++;
			if(m_nCharge <= PLAYER_ATK_CHARGE_TIME)
			{
				SetAnim(PLAYER_ANIM_ATK_RICARD);
			}
			else
			{
				m_bChargeAttack = false;
				m_nCharge = 0;
			}
		}
	}
	else
	{
		m_nCharge = 0;
	}

	// ソード回収
	if (IsMobUseRightPressed()) { if (IsMobUseLeftTriggered())m_cSword->RetrunAll(); }


#ifdef _DEBUG
	if (m_cSword != NULL)
	{
		// テスト用ソード追加
		if (GetKeyboardTrigger(DIK_E)) m_cSword->Add();
		else if (GetKeyboardTrigger(DIK_Q))m_cSword->Sub();
	}
#endif

	if (m_nComboCount > 0)
	{
		m_nComboTime++;
		DWORD dwAnim = 0x00000000l;
		int nComboTimeMax = 0;
		for (UINT i = 0; i < m_nComboCount; i++)
		{
			switch (i)
			{
			case 0:
				dwAnim |= PLAYER_ANIM_ATK_HORIZON;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_HORIZON;
				break;
			case 1:
				if (m_nComboTime > nComboTimeMax)
				dwAnim |= PLAYER_ANIM_ATK_THRUST;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_THRUST;
				break;
			case 2:
				if (m_nComboTime > nComboTimeMax)
				dwAnim |= PLAYER_ANIM_ATK_ROUNDUP;
				nComboTimeMax += PLAYER_ATK_COMBOTIME_ROUNDUP;
				break;
			}
		}
		if (m_nComboTime <= nComboTimeMax)
		{
			switch (m_nComboTime)
			{
			case PLAYER_ATK_VOICE_COMBOTIME_HORIZON:
				SetVoice(VOICE_ATK1, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case PLAYER_ATK_VOICE_COMBOTIME_THRUST:
				SetVoice(VOICE_ATK2, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case PLAYER_ATK_VOICE_COMBOTIME_ROUNDUP:
				SetVoice(VOICE_ATK4, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			}
			SetAnim(dwAnim);
		}
		else
		{
			m_nComboCount = 0;
			m_nComboTime = 0;
			m_bAttack = false;
		}
	}
}

//=============================================================================
// 防御処理
//=============================================================================
void Player::Guard(void)
{
	if (!m_bAttack && !m_bChargeAttack)
	{
		if (GetKeyboardPress(DIK_G))
		{
			if (m_eMode == MODE_FLY)
				m_eMode = MODE_FLOAT;

			if (!m_bGuard)
			{
				SetVoice(VOICE_GUARD1, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				m_bGuard = true;
			}

			// 防御アニメーションをセット
			SetAnim(PLAYER_ANIM_GUARD);
		}
		else
		{
			m_bGuard = false;
		}
	}
}

//=============================================================================
// 飛行処理
//=============================================================================
void Player::Fly(void)
{
	// 飛行アニメーションをセット
	SetAnim(PLAYER_ANIM_FRY);
	// ウィングのアニメーションをセット
	m_cWing->SetAnim(WING_ANIM_FRY);

	// ピッチ
	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R))
	{	// 同時押しは慣性を止める
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}
	else if (InputPress(INPUT_DOWN_R))
	{	// ピッチ角度を慣性で加算
		m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, PLAYER_ROT_SPEED_MAX_X);
	}
	else if (InputPress(INPUT_UP_R))
	{	// ピッチ角度を慣性で減算
		m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, -PLAYER_ROT_SPEED_MAX_X);
	}
	else
	{	// 入力なしは慣性を止める
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}

	// ロール
	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R))
	{	// 同時押しは慣性を止める
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}
	else if (InputPress(INPUT_RIGHT_R))
	{	// ロール角度を慣性で加算
		m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, PLAYER_ROT_SPEED_MAX_Z);
	}
	else if (InputPress(INPUT_LEFT_R))
	{	// ロール角度を慣性で減算
		m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, -PLAYER_ROT_SPEED_MAX_Z);
	}
	else
	{	// 入力なしは慣性を止める
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}

	m_vRotIner.x = GetMobUseY() * 0.00006f;
	m_vRotIner.z = GetMobUseX() * 0.00006f;
	m_vRot += m_vRotIner;

	// ロール回転を計算し、ピッチ回転用の軸を求める
	QuaternionCalculate(&m_vX, &m_vZ, m_vRot.z, &m_vX);

	// ピッチ回転を計算し、ロール回転用の軸を求める
	QuaternionCalculate(&m_vZ, &m_vX, m_vRot.x, &m_vZ);

	// ローカルY軸のベクトルを外積で求める
	CrossProduct(&m_vY, &m_vZ, &m_vX);

	// 移動量を算出
	m_vMove = -m_vZ * m_fMoveSpeed;

	m_vUp = m_vY;

	// 傾き復元処理
	SlopeRestore();
}

//=============================================================================
// 浮遊処理
//=============================================================================
void Player::Float(void)
{
	// 浮遊アニメーションをセット
	SetAnim(PLAYER_ANIM_FLOAT);
	// ウィングのアニメーションをセット
	m_cWing->SetAnim(WING_ANIM_FLOAT);

	// YベクトルをUpYに補正
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)) { /* 同時押し無効 */ }
	else if (InputPress(INPUT_UP_R))
	{
		m_vRotIner.x += 0.05f;
	}
	else if (InputPress(INPUT_DOWN_R))
	{
		m_vRotIner.x -= 0.05f;
	}

	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R)) { /* 同時押し無効 */ }
	else if (InputPress(INPUT_LEFT_R))
	{
		m_vRotIner.y += 0.05f;
	}
	else if (InputPress(INPUT_RIGHT_R))
	{
		m_vRotIner.y -= 0.05f;
	}

	// マウス
	m_vRotIner.x -= GetMobUseY() * 0.001f;
	m_vRotIner.y -= GetMobUseX() * 0.001f;

	m_vRot.x = m_vRot.x + ((m_vRotIner.x - m_vRot.x) * 0.05f);
	m_vRot.y = m_vRot.y + ((m_vRotIner.y - m_vRot.y) * 0.05f);

	D3DXVECTOR3 vTa = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vAxis =
		D3DXVECTOR3(cosf(m_vRot.y), 0.0f, sinf(m_vRot.y));

	QuaternionCalculate(&vTa, &vAxis, m_vRot.x, &vTa);

	m_vZ = vTa;

	// ローカルX軸のベクトルを外積で求める
	CrossProduct(&m_vX, &m_vY, &m_vZ);
	// ローカルX軸のベクトルを正規化
	D3DXVec3Normalize(&m_vX, &m_vX);

	// 移動処理
	MoveNormal();
}

//=============================================================================
// 浮遊処理
//=============================================================================
void Player::Lockon(void)
{

	// 浮遊アニメーションをセット
	SetAnim(PLAYER_ANIM_FLOAT);
	// ウィングのアニメーションをセット
	m_cWing->SetAnim(WING_ANIM_FLOAT);

	// Ｙベクトルを上に向ける
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	// 対象座標と自らの座標を減算
	D3DXVECTOR3 vLook = m_vPos - m_vTarget;
	//vLook *= -1.0f;

	D3DXVec3Normalize(&vLook, &vLook);
	m_vZ = m_vZ + (vLook - m_vZ) * 0.1f;


	// 自らが向くベクトルを正規化する
	D3DXVec3Normalize(&m_vZ, &m_vZ);

	//m_vZ *= -1.0f;

	// ローカルX軸のベクトルを外積で求める
	CrossProduct(&m_vX, &m_vY, &m_vZ);

	// X軸を正規化する
	D3DXVec3Normalize(&m_vX, &m_vX);

	// 移動処理
	MoveNormal();
}

//=============================================================================
// 移動処理
//=============================================================================
void Player::Change(void)
{
	if (m_nCount < PLAYER_CHANGE_FRAME)
	{
		m_vY += m_vRotChange;
		m_nCount++;
	}
	else
	{
		MatrixRotationVecAndUp(&m_mtxWorld, m_vZ, m_vY);
		m_vX = D3DXVECTOR3(m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13);
		m_vZ = D3DXVECTOR3(m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33);
		m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_eMode = MODE_FLOAT;
	}

	// カメラをAtをモデルに設定
	CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	// カメラEyeをモデル後方にセット
	CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + (m_vY * 30) + m_vZ * 200);

	// カメラUpをモデル上部に設定
	CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);
	//Camera::SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}

//=============================================================================
// 行動処理
//=============================================================================
void Player::Action(void)
{
	if (m_bAttack || m_bGuard || m_bChargeAttack || m_bCharge || m_bStep)
	{
		m_bAction = true;
	}
	else
	{
		m_bAction = false;
	}

	if (m_bAction)
	{
		// ウィングのアニメーションをセット
		m_cWing->SetAnim(WING_ANIM_CLOSE);
		SetLimit(&m_fMagicScl, m_fMagicScl + 2.0f, 20.0f, 0.0f);
	}
	else
	{
		SetLimit(&m_fMagicScl, m_fMagicScl - 2.0f, 20.0f, 0.0f);
	}
}


//=============================================================================
// 移動処理
//=============================================================================
void Player::UpdateMove(void)
{
	if (GetKeyboardPress(DIK_Q) && GetKeyboardPress(DIK_E))
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}
	else if (GetKeyboardPress(DIK_Q))
	{
		m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED_CHANGE, PLAYER_MOVE_SPEED_MIN);
	}
	else if (GetKeyboardPress(DIK_E))
	{
		m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED_CHANGE, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}

	// 移動量を適用
	m_vPos += m_vMove;

	// 上昇・下降を適用
	m_vPos.y += m_fRiseSpeed;

	// 移動制限処理
	if (SetLimit(&m_vPos.y, m_vPos.y, PLAYER_HEIGHT_MAX, PLAYER_HEIGHT_MIN))
		m_vMove.y = 0.0f;

#ifdef _DEBUG
	D3DXVECTOR3 vTemp = D3DXVECTOR3(m_pMtxTorso->_41, m_pMtxTorso->_42, m_pMtxTorso->_43);
	vTemp += m_vMove;
	vTemp.y += m_fRiseSpeed;
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, vTemp);
#endif

	// 移動量に慣性をかける
	m_vMove.x += (0.0f - m_vMove.x) * PLAYER_MOVE_INERTIA;
	m_vMove.y += (0.0f - m_vMove.y) * PLAYER_MOVE_INERTIA;
	m_vMove.z += (0.0f - m_vMove.z) * PLAYER_MOVE_INERTIA;
}

//=============================================================================
// ステップ入力処理
//=============================================================================
void Player::Step(void)
{
	// ステップ中は入力を受け付けない
	if (!m_bStep)
	{
		// 指定キーでステップ設定
		if (GetKeyboardTrigger(DIK_W))
			SetStep(STEP_FRONT);
		else if (GetKeyboardTrigger(DIK_S))
			SetStep(STEP_BACK);
		else if (GetKeyboardTrigger(DIK_A))
			SetStep(STEP_LEFT);
		else if (GetKeyboardTrigger(DIK_D))
			SetStep(STEP_RIGHT);
	}

	// ステップ更新処理
	UpdateStep();
}

//=============================================================================
// ステップ設定処理
//=============================================================================
void Player::SetStep(PlayerStep eStep)
{
	// ステップステートが違う場合
	if (m_eStep != eStep)
	{
		m_nStepCount = PLAYER_STEP_WAIT_TIME;
		m_eStep = eStep;
	}
	// ステップステートが同様の場合
	else
	{
		// ステップ
		if (m_nStepCount > 0)
		{
			m_bStep = true;
			// ステップ時間をカウントに代入
			m_nStepCount = PLAYER_STEP_TIME;
		}
	}
}

//=============================================================================
// ステップ更新処理
//=============================================================================
void Player::UpdateStep(void)
{
	if (m_eStep != STEP_NON)
	{
		// ステップが True
		if (m_bStep)
		{
			// ステップ中に FLYモードの場合は FLOATモードに切り替え
			if (m_eMode == MODE_FLY)
				m_eMode = MODE_FLOAT;

			// ステップアニメーションを設定
			switch (m_eStep)
			{
			case STEP_FRONT:
				SetAnim(PLAYER_ANIM_STEP_FLONT);
				break;
			case STEP_BACK:
				SetAnim(PLAYER_ANIM_STEP_BACK);
				break;
			case STEP_LEFT:
				SetAnim(PLAYER_ANIM_STEP_LEFT);
				break;
			case STEP_RIGHT:
				SetAnim(PLAYER_ANIM_STEP_RIGHT);
				break;
			}
			// ステップカウントを減算
			m_nStepCount--;
			// ステップカウントが 0 を下回ったら
			if (m_nStepCount < 0)
			{
				// ステップフラグを false
				m_bStep = false;
				// ステップをなし
				m_eStep = STEP_NON;
			}
		}
		// ステップが false
		else
		{
			// ステップカウントを減算
			m_nStepCount--;
			// ステップカウントが 0 を下回ったら
			if (m_nStepCount < 0)
			{
				// ステップをなし
				m_eStep = STEP_NON;
			}
		}
	}
}

//=============================================================================
// モードチェンジ処理
//=============================================================================
void Player::ModeChange(void)
{
	if (GetKeyboardTrigger(DIK_1))
	{
		m_eMode = MODE_FLOAT;
	}

	if (GetKeyboardTrigger(DIK_TAB))
	{
		if (m_eMode == MODE_LOCKON)m_eMode = MODE_FLOAT;
		else m_eMode = MODE_LOCKON;
	}

	if (GetKeyboardTrigger(DIK_V))
	{
		if (m_eMode == MODE_FLY)m_eMode = MODE_FLOAT;
		else m_eMode = MODE_FLY;
		// ステップ初期化
		m_eStep = STEP_NON;
		m_bStep = false;
		m_nStepCount = 0;
	}

	if (m_eMode != m_eModeOld)
	{
		switch (m_eMode)
		{
		case MODE_FLOAT:
			SetSoundBgm(SOUND_BGM_TEST2);
			//m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X,0.0f,0.0f);
			m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X - atan2(m_vZ.y, 1.0f), -atan2(m_vZ.x, m_vZ.z), 0.0f);
			m_vRotIner = m_vRot;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(0.01f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_FLY:
			SetSoundBgm(SOUND_BGM_TEST1);
			m_vRot = ZERO_D3DXVECTOR3;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(0.05f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_LOCKON:
			SetSoundBgm(SOUND_BGM_TEST2);
			m_vRot = ZERO_D3DXVECTOR3;
			CameraManager::pCamera[CameraManager::CENTER]->SetEyeIner(1.0f);
			CameraManager::pCamera[CameraManager::CENTER]->SetAtIner(0.05f);
			break;
		case MODE_CHANGE:
			m_vRotChange = (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) / PLAYER_CHANGE_FRAME;
			m_nCount = 0;
			break;
		}
		m_eModeOld = m_eMode;
	}
}

//=============================================================================
// 移動処理関数
//=============================================================================
void Player::MoveNormal(void)
{
	if (m_bStep)
	{
		// ステップ移動処理
		switch (m_eStep)
		{
		case STEP_FRONT:
			m_vMove = -m_vZ * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_BACK:
			m_vMove = m_vZ * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_LEFT:
			m_vMove = m_vX * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		case STEP_RIGHT:
			m_vMove = -m_vX * m_fMoveSpeed * PLAYER_STEP_SPEED;
			break;
		}
	}
	else
	{
		// 移動処理
		if (InputPress(INPUT_LEFT))
		{
			if (InputPress(INPUT_UP))
			{// 左前移動
				m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_DOWN))
			{// 左後移動
				m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_RIGHT)) {/* 左右同時押しは処理なし */ }
			else
			{// 左移動
				m_vMove = m_vX * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_RIGHT))
		{
			if (InputPress(INPUT_UP))
			{// 右前移動
				m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else if (InputPress(INPUT_DOWN))
			{// 右後移動
				m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			}
			else
			{// 右移動
				m_vMove = -m_vX * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_UP))
		{// 前移動
			if (InputPress(INPUT_DOWN)) {/* 前後同時押しは処理なし */ }
			else
			{
				m_vMove = -m_vZ * m_fMoveSpeed;
			}
		}
		else if (InputPress(INPUT_DOWN))
		{// 後移動
			m_vMove = m_vZ * m_fMoveSpeed;
		}

		// 上昇・下降処理
		if (GetKeyboardPress(DIK_SPACE) && GetKeyboardPress(DIK_LSHIFT))
		{
			m_fRiseSpeed = max(m_fRiseSpeed - PLAYER_RISE_SPEED, -PLAYER_RISE_SPEED_MAX);
		}
		else if (GetKeyboardPress(DIK_SPACE))
		{
			m_fRiseSpeed = min(m_fRiseSpeed + PLAYER_RISE_SPEED, PLAYER_RISE_SPEED_MAX);
		}
		else
		{
			// 移動量に慣性をかける
			m_fRiseSpeed += (0.0f - m_fRiseSpeed) * PLAYER_MOVE_INERTIA;

			//if (m_fRiseSpeed > 0.0f)
			//	m_fRiseSpeed = max(m_fRiseSpeed - PLAYER_RISE_SPEED_AUTO, 0.0f);
			//else if (m_fRiseSpeed < 0.0f)
			//	m_fRiseSpeed = min(m_fRiseSpeed + PLAYER_RISE_SPEED_AUTO, 0.0f);
		}

		//m_vMove.y += m_fRiseSpeed;
	}

	// 傾き処理
	SlopeNormal();
}

//=============================================================================
// 傾き処理関数
//=============================================================================
void Player::SlopeNormal(void)
{
	if (InputPress(INPUT_LEFT) && InputPress(INPUT_RIGHT))
	{
		if (m_vSlopeIner.z > 0.0f)
			m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.z < 0.0f)
			m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);
	}
	else if (InputPress(INPUT_LEFT))
	{// 左移動
		m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, -PLAYER_SLOPE_SPEED_MAX);
	}
	else if (InputPress(INPUT_RIGHT))
	{// 右移動
		m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, PLAYER_SLOPE_SPEED_MAX);
	}
	else
	{
		if (m_vSlopeIner.z > 0.0f)
			m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.z < 0.0f)
			m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);
	}

	if (InputPress(INPUT_UP) && InputPress(INPUT_DOWN))
	{
		if (m_vSlopeIner.x > 0.0f)
			m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.x < 0.0f)
			m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);
	}
	if (InputPress(INPUT_UP))
	{// 前移動
		m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, -PLAYER_SLOPE_SPEED_MAX);
	}
	else if (InputPress(INPUT_DOWN))
	{// 後移動
		m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, PLAYER_SLOPE_SPEED_MAX);
	}
	else
	{
		if (m_vSlopeIner.x > 0.0f)
			m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
		else if (m_vSlopeIner.x < 0.0f)
			m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);
	}

	m_vSlope = m_vSlope + ((m_vSlopeIner - m_vSlope) * PLAYER_SLOPE_INERTIA);
}

//=============================================================================
// 傾き復元関数
//=============================================================================
void Player::SlopeRestore(void)
{
	if (m_vSlopeIner.z > 0.0f)
		m_vSlopeIner.z = max(m_vSlopeIner.z - PLAYER_SLOPE_SPEED, 0.0f);
	else if (m_vSlopeIner.z < 0.0f)
		m_vSlopeIner.z = min(m_vSlopeIner.z + PLAYER_SLOPE_SPEED, 0.0f);

	if (m_vSlopeIner.x > 0.0f)
		m_vSlopeIner.x = max(m_vSlopeIner.x - PLAYER_SLOPE_SPEED, 0.0f);
	else if (m_vSlopeIner.x < 0.0f)
		m_vSlopeIner.x = min(m_vSlopeIner.x + PLAYER_SLOPE_SPEED, 0.0f);

	m_vSlope = m_vSlope + ((m_vSlopeIner - m_vSlope) * PLAYER_SLOPE_INERTIA);
}

//=============================================================================
// アニメーション確認
//=============================================================================
void Player::CheckAnim(void)
{
	// 優先順位ごとに if 分岐
	if (PLAYER_ANIM_DUMMY & m_dwAnim)				ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_GUARD & m_dwAnim)			ChangeAnim(PLAYER_GUARD, PLAYER_ANIM_WEIGHT_DEF);

	else if (PLAYER_ANIM_STEP_FLONT & m_dwAnim)		ChangeAnim(PLAYER_STEP_FRONT, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_BACK & m_dwAnim)		ChangeAnim(PLAYER_STEP_BACK, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_LEFT & m_dwAnim)		ChangeAnim(PLAYER_STEP_LEFT, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_STEP_RIGHT & m_dwAnim)		ChangeAnim(PLAYER_STEP_RIGHT, PLAYER_ANIM_WEIGHT_ATK);

	else if (PLAYER_ANIM_ATK_RICARD & m_dwAnim)		ChangeAnim(PLAYER_ATK_RICARD, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_ATK_ROUNDUP & m_dwAnim)	ChangeAnim(PLAYER_ATK_ROUNDUP, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_THRUST & m_dwAnim)		ChangeAnim(PLAYER_ATK_THRUST, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_HORIZON & m_dwAnim)	ChangeAnim(PLAYER_ATK_HORIZON, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_FLOAT & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_FRY & m_dwAnim)			ChangeAnim(PLAYER_FLY, PLAYER_ANIM_WEIGHT_DEF);

	// アニメーションビットパターンを初期化
	m_dwAnim = 0x00000000l;
}

//=============================================================================
// ファイル確認処理
//=============================================================================
void Player::CheckFile(void)
{
	if (LoadFile()) { return; }
	else
	{
		WriteFile();
		LoadFile();
	}
}

//=============================================================================
// ファイル読み込み処理
//=============================================================================
bool Player::LoadFile(void)
{
	FILE *fp;
	fopen_s(&fp, "data/file/player.dat", "rb");
	if (fp == NULL) { return false; }

	// メンバ変数に読み込む
	fread(this, sizeof(Player), 1, fp);
	fclose(fp);
}

//=============================================================================
// ファイル書き込み処理
//=============================================================================
void Player::WriteFile(void)
{
	FILE *fp;
	// バイナリ書き込みモードでファイルオープン
	fopen_s(&fp, "data/file/player.dat", "wb");

	// メンバ変数を書き込む
	fwrite(this, sizeof(Player), 1, fp);
	fclose(fp);
}

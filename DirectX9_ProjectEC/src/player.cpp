//=============================================================================
//
// プレイヤー処理 [player.cpp]
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


// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_ANIM_SPEED_DEF		(60.0f / 4800.0f)	// アニメーションスピード
#define PLAYER_ANIM_WEIGHT_DEF		(0.1f)				// アニメーションのブレンドウェイト
#define PLAYER_ANIM_WEIGHT_ATK		(0.3f)				// アニメーションのブレンドウェイト

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player* PlayerManager::m_pPlayer[PLAYER_MAX] = {};

//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Player::Player(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ステータスの初期化処理
	Init();

	// モデルの初期化
	m_CSkinMesh = NULL;
	m_CSkinMesh = SceneManager::ModelMgr->GetCharData(ModelManager::PLAYER);
	//m_CSkinMesh = new CSkinMesh;
	//m_CSkinMesh->Init(pDevice, (LPSTR)PLAYER_MODEL);
	ChangeAnimSpeed(PLAYER_ANIM_SPEED_DEF);

	// ウィングの初期化
	m_cWing = NULL;
	m_cWing = new Wing;
	// キャラクターのボーンマトリクスのアドレスを取得
	m_pMtxTorso = m_CSkinMesh->GetBoneMatrixAdr((LPSTR)PLAYER_MODEL_BONE_WING);
	m_cWing->SetMtx(m_pMtxTorso);

	// ソードの初期化
	m_cSword = NULL;
	m_cSword = new Sword;
	// キャラクターのボーンマトリクスのアドレスを取得
	m_cSword->SetMtx(m_CSkinMesh->GetBoneMatrixAdr((LPSTR)PLAYER_MODEL_BONE_RIGHT_HAND));


	// エフェクトマネージャーのポインタを取得
	pEffetMgr = NULL;
	pEffetMgr = ObjectManager::GetObjectPointer<EffectManager>(ObjectManager::EFFECT);
	// エフェクトハンドルの初期化
	m_handle = -1;

	// ループエフェクトを呼び出し
	m_handle = pEffetMgr->Play(EffectManager::EFFECT_MAGICAREA);

	//pEffetMgr->m_manager->StopEffect(m_handle);
	//m_handle = -1;
}

//=============================================================================
// 初期化処理
//=============================================================================
void Player::Init(void)
{
	// 各プロパティの初期化
	m_vPos = PLAYER_POS;
	m_vRot = PLAYER_ROT;
	m_vScl = D3DXVECTOR3(PLAYER_SCL, PLAYER_SCL, PLAYER_SCL);

	m_vMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotIner = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_vTarget = D3DXVECTOR3(0.0f, 500.0f, 0.0f);

	m_eMode = MODE_FLOAT;
	m_eModeOld = MODE_MAX;	// 1フレーム目はモードチェンジに入りたい

	m_fMoveSpeed = PLAYER_MOVE_SPEED_MIN;
	m_fRiseSpeed = 0.0f;
	m_bUse = true;

	// アタック
	m_nComboTime = 0;
	m_nComboCount = 0;
	m_bAttack = false;
	m_nCharge = 0;
	m_bChargeAttack = false;

	// ガード
	m_bGuard = false;

	// 魔法陣
	m_fMagicScl = 0.0f;

	// 行列の初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//// カメラをAtをモデルに設定
	//CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	//// カメラUpをモデル上部に設定
	//CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);

	//// カメラEyeをモデル後方にセット
	//CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + m_vZ * 100);

	//WriteFile();

#ifdef _DEBUG
	m_cDebug.bUse = true;
	m_cDebug.fSize = PLAYER_COLLISION_SIZE;
	m_cDebug.nIdx = DebugObject::pSphere->Set(m_cDebug.fSize);
#endif
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

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
Player::~Player(void)
{
	// スキンメッシュの解放
	//SAFE_RELEASE(m_CSkinMesh);
	//SAFE_DELETE(m_CSkinMesh);

	// ウィングの解放
	SAFE_DELETE(m_cWing);
	// ソードの解放
	SAFE_DELETE(m_cSword);

#ifdef _DEBUG
	DebugObject::pSphere->Release(m_cDebug.nIdx);
#endif
}

//=============================================================================
// 更新処理
//=============================================================================
void Player::Update(void)
{
#ifdef _DEBUG
	ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
	bool bGui = ImGui::TreeNode("Player");
	if (bGui)
	{
		ImGui::Text("Pos [%6.0f.%6.0f,%6.0f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
		ImGui::Text("Rot [%2.2f.%2.2f,%2.2f]\n", m_vRot.x, m_vRot.y, m_vRot.z);
		ImGui::Text("RotI[%2.2f.%2.2f,%2.2f]\n", m_vRotIner.x, m_vRotIner.y, m_vRotIner.z);
		ImGui::Text("Move[%2.2f.%2.2f,%2.2f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
		ImGui::Text("Spd [%f]\n", m_fMoveSpeed);
		ImGui::Text("vX  [%.2f.%.2f,%.2f]\n", m_vX.x, m_vX.y, m_vX.z);
		ImGui::Text("vY  [%.2f.%.2f,%.2f]\n", m_vY.x, m_vY.y, m_vY.z);
		ImGui::Text("vZ  [%.2f.%.2f,%.2f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
		ImGui::Text("combo[%d] time[%d]\n", m_nComboCount, m_nComboTime);
		ImGui::InputFloat3("Target", &m_vTarget.x,2);
		ImGui::InputFloat("scl", &m_fMagicScl);
		ImGui::TreePop();
	}
#endif

	if (m_bUse)
	{



		//// モード遷移処理
		//if (m_eMode == MODE_CHANGE)
		//	Change();

		switch (m_eMode)
		{
		case MODE_FLOAT:
			Float();	// 浮遊処理
			break;
		case MODE_FLY:
			Fly();		// 飛行処理
			break;
		case MODE_LOCKON:
			Lockon();	// 照準処理
			break;
		}

		Attack();	// 攻撃処理
		Guard();	// 防御処理

		Move();		// 移動処理

		// カメラ設定
		SetCamera();

		// モードチェンジ
		ModeChange();

		// アニメーションチェック
		CheckAnim();

		// ワールド変換
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		if (m_handle >= 0)
		{
			pEffetMgr->SetMatrix(m_handle, m_mtxWorld);
			pEffetMgr->SetScale(m_handle, m_fMagicScl);
		}

		// スキンメッシュの更新処理
		m_CSkinMesh->Update();


		// ウィングの更新処理
		SAFE_UPDATE(m_cWing);

		// ソードの更新処理
		SAFE_UPDATE(m_cSword);
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Player::Draw(void)
{
	if (m_bUse)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		// ワールド変換
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		// モデルを描画-
		m_CSkinMesh->Draw(pDevice, m_mtxWorld);

		// ウィングを描画
		//if(m_cWing != NULL) m_cWing->Draw();
		SAFE_DRAW(m_cWing);

		// ソードを描画
		//if (m_cSword != NULL) m_cSword->Draw();
		SAFE_DRAW(m_cSword);

#ifdef _DEBUG
		//PrintDebugProc("Bone  [%d]\n", m_CSkinMesh[CHARACTER]->m_dwBoneCount);
		//PrintDebugProc("Bone  [%d]\n", m_CSkinMesh[WING]->m_dwBoneCount);
#endif
	}
}

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
			case 1:
				SetVoice(VOICE_ATK1, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case 30:
				SetVoice(VOICE_ATK2, E_DS8_FLAG_NONE, SOUND_OPTION_CONTINUE_ON, 0);
				break;
			case 50:
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
// カメラ設定処理
//=============================================================================
void Player::SetCamera(void)
{
	Camera* pCamera = CameraManager::GetCameraNow();
	D3DXVECTOR3 temp;

	D3DXMATRIX mtxTemp;
	D3DXVECTOR3 vX, vY, vZ;

	// プレイヤーの行列を取得
	mtxTemp = m_mtxWorld;
	// 取得した行列を正規化
	D3DXMatrixNormalize(&mtxTemp, &mtxTemp);

	// 行列内のベクトルを取得
	vX = D3DXVECTOR3(mtxTemp._11, mtxTemp._12, mtxTemp._13);
	vY = D3DXVECTOR3(mtxTemp._21, mtxTemp._22, mtxTemp._23);
	vZ = D3DXVECTOR3(mtxTemp._31, mtxTemp._32, mtxTemp._33);


	switch (m_eMode)
	{
	case MODE_FLOAT:
		// カメラをAtをモデルに設定
		pCamera->SetAt(m_vPos + (vY * 30) + (-vZ * 100));
		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + (vY * 30) + (vZ * 100));
		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);

		// カメラ慣性を減らす
		pCamera->AddAtIner(0.02f);
		pCamera->AddEyeIner(0.02f);
		break;
	case MODE_FLY:
		// カメラをAtをモデルに設定
		pCamera->SetAt(m_vPos + (m_vY * 30));
		//pCamera->SetAt(m_vTarget);
		//temp =  m_vPos - m_vTarget;
		//D3DXVec3Normalize(&temp, &temp);

		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + m_vZ * 100);
		//pCamera->SetEye(m_vPos + temp * 100);
		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);
		//pCamera->SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		// カメラ慣性を減らす
		pCamera->AddAtIner(0.02f);
		//pCamera->AddEyeIner(0.02f);
		break;
	case MODE_LOCKON:
		// カメラをAtを対象に設定
		pCamera->SetAt(m_vTarget);
		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + (vX * 30) + (vY * 30) + (vZ * 100));
		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);

		// カメラ慣性を減らす
		//pCamera->AddEyeIner(0.05f);
		pCamera->AddAtIner(0.02f);
		break;
	}
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
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f)- m_vY) * 0.1f;

	if (InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)){ /* 同時押し無効 */ }
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

	D3DXVECTOR3 vTa = D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXVECTOR3 vAxis =
		D3DXVECTOR3(cosf(m_vRot.y), 0.0f, sinf(m_vRot.y));

	QuaternionCalculate(&vTa, &vAxis, m_vRot.x, &vTa);
	
	m_vZ = vTa;

	// ローカルX軸のベクトルを外積で求める
	CrossProduct(&m_vX, &m_vY, &m_vZ);
	// ローカルX軸のベクトルを正規化
	D3DXVec3Normalize(&m_vX, &m_vX);

	// 移動処理
	if (InputPress(INPUT_LEFT))
	{
		if (InputPress(INPUT_UP))
		{// 左前移動
			//MoveFunc(m_fHAngle + D3DX_PI * 0.25f);
			m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// 左後移動
			//MoveFunc(m_fHAngle + D3DX_PI * 0.75f);
			m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_RIGHT))
		{// 左右同時押しは処理なし
		}
		else
		{// 左移動
			//MoveFunc(m_fHAngle + D3DX_PI * 0.50f);
			m_vMove = m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_RIGHT))
	{
		if (InputPress(INPUT_UP))
		{// 右前移動
			//MoveFunc(m_fHAngle - D3DX_PI * 0.25f);
			m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// 右後移動
			//MoveFunc(m_fHAngle - D3DX_PI * 0.75f);
			m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else
		{// 右移動
			//MoveFunc(m_fHAngle - D3DX_PI * 0.50f);
			m_vMove = -m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_UP))
	{// 前移動
		if (InputPress(INPUT_DOWN))
		{// 前後同時押しは処理なし
		}
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
		m_fRiseSpeed = max(m_fRiseSpeed - 0.5f, -10.0f);
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		m_fRiseSpeed = min(m_fRiseSpeed + 0.5f, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		if (m_fRiseSpeed > 0.0f)
			m_fRiseSpeed = max(m_fRiseSpeed - 0.2f, 0.0f);
		else if (m_fRiseSpeed < 0.0f)
			m_fRiseSpeed = min(m_fRiseSpeed + 0.2f, 0.0f);
	}

	m_vMove.y = m_fRiseSpeed;

	// 移動を適用
	//m_vPos += m_vMove;


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

	//// 移動量を適用
	//m_vPos += m_vMove;


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

#ifdef _DEBUG
	PrintDebugProc("【 Float 】\n");
	PrintDebugProc("vX  [%f,%f,%f]\n", m_vX.x, m_vX.y, m_vX.z);
	PrintDebugProc("vZ  [%f,%f,%f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
#endif

	// 移動処理
	if (InputPress(INPUT_LEFT))
	{
		if (InputPress(INPUT_UP))
		{// 左前移動
			//m_vPos += (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// 左後移動
			//m_vPos += (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_RIGHT)){/* 左右同時押しは処理なし */}
		else
		{// 左移動
			//m_vPos += m_vX * m_fMoveSpeed;
			m_vMove = m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_RIGHT))
	{
		if (InputPress(INPUT_UP))
		{// 右前移動
			//m_vPos -= (m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = -(m_vX + m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else if (InputPress(INPUT_DOWN))
		{// 右後移動
			//m_vPos -= (m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
			m_vMove = -(m_vX - m_vZ) * 0.71f * m_fMoveSpeed;
		}
		else
		{// 右移動
			//m_vPos -= m_vX * m_fMoveSpeed;
			m_vMove = -m_vX * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_UP))
	{// 前移動
		if (InputPress(INPUT_DOWN)) {/* 前後同時押しは処理なし */}
		else
		{
			//m_vPos += -m_vZ * m_fMoveSpeed;
			m_vMove = -m_vZ * m_fMoveSpeed;
		}
	}
	else if (InputPress(INPUT_DOWN))
	{// 後移動
		//m_vPos += m_vZ * m_fMoveSpeed;
		m_vMove = m_vZ * m_fMoveSpeed;
	}

	// 上昇・下降処理
	if (GetKeyboardPress(DIK_SPACE) && GetKeyboardPress(DIK_LSHIFT))
	{
		m_vMove.y = -m_fMoveSpeed;
		m_fRiseSpeed = max(m_fRiseSpeed - 0.5f, -10.0f);
	}
	else if (GetKeyboardPress(DIK_SPACE))
	{
		m_vMove.y = m_fMoveSpeed;
		m_fRiseSpeed = min(m_fRiseSpeed + 0.5f, PLAYER_MOVE_SPEED_MAX);
	}
	else
	{
		if (m_fRiseSpeed > 0.0f)
			m_fRiseSpeed = max(m_fRiseSpeed - 0.2f, 0.0f);
		else if (m_fRiseSpeed < 0.0f)
			m_fRiseSpeed = min(m_fRiseSpeed + 0.2f, 0.0f);
	}

	//m_vMove.y += m_fRiseSpeed;


	// 移動を適用
	//m_vPos += m_vMove;


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
// 移動処理
//=============================================================================
void Player::Move(void)
{
	if (m_bAttack || m_bGuard || m_bChargeAttack)
	{
		// ウィングのアニメーションをセット
		m_cWing->SetAnim(WING_ANIM_CLOSE);
		SetLimit(&m_fMagicScl, m_fMagicScl + 2.0f, 20.0f, 0.0f);
	}
	else
	{
		SetLimit(&m_fMagicScl, m_fMagicScl - 2.0f, 20.0f, 0.0f);
	}


	if (GetKeyboardPress(DIK_Q) && GetKeyboardPress(DIK_E))
	{
		//if (m_fMoveSpeed > 0.0f)
		//	m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, 0.0f);
		//else if (m_fMoveSpeed < 0.0f)
		//	m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, 0.0f);
	}
	else if (GetKeyboardPress(DIK_Q))
	{
		m_fMoveSpeed = max(m_fMoveSpeed - PLAYER_MOVE_SPEED, PLAYER_MOVE_SPEED_MIN);
	}
	else if (GetKeyboardPress(DIK_E))
	{
		m_fMoveSpeed = min(m_fMoveSpeed + PLAYER_MOVE_SPEED, PLAYER_MOVE_SPEED_MAX);
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

	// 移動制限処理
	if (SetLimit(&m_vPos.y, m_vPos.y, PLAYER_HEIGHT_MAX, PLAYER_HEIGHT_MIN))
		m_vMove.y = 0.0f;

#ifdef _DEBUG
	D3DXVECTOR3 vTemp = D3DXVECTOR3(m_pMtxTorso->_41, m_pMtxTorso->_42, m_pMtxTorso->_43);
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, vTemp + m_vMove);
#endif

	// 移動量に慣性をかける
	m_vMove.x += (0.0f - m_vMove.x) * PLAYER_MOVE_INERTIA;
	m_vMove.y += (0.0f - m_vMove.y) * PLAYER_MOVE_INERTIA;
	m_vMove.z += (0.0f - m_vMove.z) * PLAYER_MOVE_INERTIA;
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
	}

	if (m_eMode != m_eModeOld)
	{
		switch (m_eMode)
		{
		case MODE_FLOAT:
			SetSoundBgm(SOUND_BGM_TEST2);
			//m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X,0.0f,0.0f);
			m_vRot = D3DXVECTOR3(PLAYER_FLOAT_ROT_X - atan2(m_vZ.y, 1.0f), -atan2(m_vZ.x, m_vZ.z),0.0f);
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
void Player::MoveFunc(float fAngle)
{
	m_vPos.x += cosf(fAngle) * m_fMoveSpeed;
	m_vPos.z += sinf(fAngle) * m_fMoveSpeed;
}

//=============================================================================
// 回転処理関数
//=============================================================================
void Player::RotFunc(D3DXVECTOR3 vRot)
{
	//m_vPos.x += sinf(fAngle) * PLAYER_MOVE_SPEED;
	//m_vPos.z += cosf(fAngle) * PLAYER_MOVE_SPEED;
}

//=============================================================================
// アニメーション確認
//=============================================================================
void Player::CheckAnim(void)
{
	// 優先順位ごとに if 分岐
	if		(PLAYER_ANIM_DUMMY & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_GUARD & m_dwAnim)			ChangeAnim(PLAYER_GUARD, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_ATK_RICARD & m_dwAnim)		ChangeAnim(PLAYER_ATK_RICARD, PLAYER_ANIM_WEIGHT_DEF);
	else if	(PLAYER_ANIM_ATK_ROUNDUP & m_dwAnim)	ChangeAnim(PLAYER_ATK_ROUNDUP, PLAYER_ANIM_WEIGHT_ATK);
	else if	(PLAYER_ANIM_ATK_THRUST & m_dwAnim)		ChangeAnim(PLAYER_ATK_THRUST, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_ATK_HORIZON & m_dwAnim)	ChangeAnim(PLAYER_ATK_HORIZON, PLAYER_ANIM_WEIGHT_ATK);
	else if (PLAYER_ANIM_FLOAT & m_dwAnim)			ChangeAnim(PLAYER_FLOAT, PLAYER_ANIM_WEIGHT_DEF);
	else if (PLAYER_ANIM_FRY & m_dwAnim)			ChangeAnim(PLAYER_FLY, PLAYER_ANIM_WEIGHT_DEF);


	// アニメーションビットパターンを初期化
	m_dwAnim = 0x00000000l;
}


//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
PlayerManager::PlayerManager(void)
{
	// オブジェクトIDとプライオリティの設定処理
	SetIdAndPriority(ObjectID::PLAYERMANAGER, Priority::Middle, Priority::High);

	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーを初期化
		m_pPlayer[i] = NULL;
		m_pPlayer[i] = new Player;
	}
}

//=============================================================================
// デストラクタ（終了処理）
//=============================================================================
PlayerManager::~PlayerManager(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		delete m_pPlayer[i];
		m_pPlayer[i] = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void PlayerManager::Update(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Update();
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void PlayerManager::Draw(void)
{
	for (unsigned int i = 0; i < PLAYER_MAX; i++)
	{
		if (m_pPlayer[i] != NULL)
		{
			m_pPlayer[i]->Draw();
		}
	}
}
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
#include "cube.h"
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
extern float fGyroX;
extern float fGyroY;
extern float fGyroZ;
//D3DXVECTOR3 testpos,testrot;

Player* PlayerManager::m_pPlayer[PLAYER_MAX] = {};

int nTextIdx = 0;


//=============================================================================
// コンストラクタ（初期化処理）
//=============================================================================
Player::Player(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//// オブジェクトIDとプライオリティの設定処理
	//SetIdAndPriority(ObjectID::PLAYER, Priority::Middle, Priority::Middle);

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

	m_eMode = MODE_FLOAT;
	m_eModeOld = MODE_MAX;	// 1フレーム目はモードチェンジに入りたい

	m_fMoveSpeed = PLAYER_MOVE_SPEED_MIN;
	m_fRiseSpeed = 0.0f;
	m_bUse = true;

	// アタック
	m_nComboTime = 0;
	m_nComboCount = 0;

	// 行列の初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// カメラをAtをモデルに設定
	CameraManager::pCamera[CameraManager::CENTER]->SetAt(m_vPos + (m_vY * 30));

	// カメラUpをモデル上部に設定
	CameraManager::pCamera[CameraManager::CENTER]->SetUp(m_vY);

	// カメラEyeをモデル後方にセット
	CameraManager::pCamera[CameraManager::CENTER]->SetEye(m_vPos + m_vZ * 100);

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
		ImGui::Text("Pos [%f,%f,%f]\n", m_vPos.x, m_vPos.y, m_vPos.z);
		ImGui::Text("Rot [%f,%f,%f]\n", m_vRot.x, m_vRot.y, m_vRot.z);
		ImGui::Text("RotI[%f,%f,%f]\n", m_vRotIner.x, m_vRotIner.y, m_vRotIner.z);
		ImGui::Text("Move[%f,%f,%f]\n", m_vMove.x, m_vMove.y, m_vMove.z);
		ImGui::Text("Spd [%f]\n", m_fMoveSpeed);
		ImGui::Text("vX  [%f,%f,%f]\n", m_vX.x, m_vX.y, m_vX.z);
		ImGui::Text("vY  [%f,%f,%f]\n", m_vY.x, m_vY.y, m_vY.z);
		ImGui::Text("vZ  [%f,%f,%f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
		ImGui::Text("combo[%d] time[%d]\n", m_nComboCount, m_nComboTime);
		ImGui::TreePop();
	}
#endif

	if (m_bUse)
	{

		Attack();	// 攻撃処理

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

		Move();		// 移動処理

		// カメラ設定
		SetCamera();

		// モードチェンジ
		ModeChange();

		// アニメーションチェック
		CheckAnim();

		// ワールド変換
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

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

	if (IsMobUseLeftTriggered() && !IsMobUseRightPressed())
	{
		m_nComboCount++;
		if (m_nComboCount > PLAYER_ATK_COMBO_MAX) m_nComboCount = PLAYER_ATK_COMBO_MAX;
	}


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
			SetAnim(dwAnim);
		}
		else
		{
			m_nComboCount = 0;
			m_nComboTime = 0;
		}
	}

	if (m_cSword != NULL)
	{
		// ソード操作
		if (IsMobUseRightTriggered()) m_cSword->Shot();
		else if (IsMobUseRightPressed()) {if (IsMobUseLeftTriggered())m_cSword->RetrunAll();}

		// テスト用ソード追加
		if (GetKeyboardTrigger(DIK_E)) m_cSword->Add(); 
		else if (GetKeyboardTrigger(DIK_Q))m_cSword->Sub();
	}

}

//=============================================================================
// カメラ設定処理
//=============================================================================
void Player::SetCamera(void)
{
	Camera* pCamera = CameraManager::GetCameraNow();

	switch (m_eMode)
	{
	case MODE_FLOAT:
		// カメラをAtをモデルに設定
		pCamera->SetAt(m_vPos + (m_vY * 30));
		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + (m_vY * 30) + m_vZ * 100);
		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);

		// カメラ慣性を減らす
		pCamera->AddAtIner(0.02f);
		pCamera->AddEyeIner(0.02f);
		break;
	case MODE_FLY:
		// カメラをAtをモデルに設定
		pCamera->SetAt(m_vPos + (m_vY * 30));
		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + m_vZ * 100);
		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);

		// カメラ慣性を減らす
		pCamera->AddAtIner(0.02f);
		break;
	case MODE_LOCKON:
		// カメラをAtを対象に設定
		pCamera->SetAt(ZERO_D3DXVECTOR3);
		// カメラEyeをモデル後方にセット
		pCamera->SetEye(m_vPos + (m_vY * 30) + m_vZ * 100);
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
	if ((InputPress(INPUT_UP_R) && InputPress(INPUT_DOWN_R)) || (fGyroY < PLAYER_GYRO_MARGIN && fGyroY > -PLAYER_GYRO_MARGIN))
	{	// 同時押しは慣性を止める
		if (m_vRot.x > 0.0f)
			m_vRot.x = max(m_vRot.x - PLAYER_ROT_SPEED_X, 0.0f);
		else if (m_vRot.x < 0.0f)
			m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, 0.0f);
	}
	else if (InputPress(INPUT_DOWN_R) || fGyroY > PLAYER_GYRO_MARGIN)
	{	// ピッチ角度を慣性で加算
		m_vRot.x = min(m_vRot.x + PLAYER_ROT_SPEED_X, PLAYER_ROT_SPEED_MAX_X);
	}
	else if (InputPress(INPUT_UP_R) || fGyroY < -PLAYER_GYRO_MARGIN)
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
	if (InputPress(INPUT_LEFT_R) && InputPress(INPUT_RIGHT_R) || (fGyroZ < PLAYER_GYRO_MARGIN && fGyroZ > -PLAYER_GYRO_MARGIN))
	{	// 同時押しは慣性を止める
		if (m_vRot.z > 0.0f)
			m_vRot.z = max(m_vRot.z - PLAYER_ROT_SPEED_Z, 0.0f);
		else if (m_vRot.z < 0.0f)
			m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, 0.0f);
	}
	else if (InputPress(INPUT_RIGHT_R) || fGyroZ < -PLAYER_GYRO_MARGIN)
	{	// ロール角度を慣性で加算
		m_vRot.z = min(m_vRot.z + PLAYER_ROT_SPEED_Z, PLAYER_ROT_SPEED_MAX_Z);
	}
	else if (InputPress(INPUT_LEFT_R) || fGyroZ > -PLAYER_GYRO_MARGIN)
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
	m_cWing->SetAnim(WING_ANIM_CLOSE);

	// Ｙベクトルを上に向ける
	m_vY = m_vY + (D3DXVECTOR3(0.0f, 1.0f, 0.0f) - m_vY) * 0.1f;

	// 対象座標と自らの座標を減算
	D3DXVECTOR3 vLook = ZERO_D3DXVECTOR3 - m_vPos;
	vLook *= -1.0f;

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

	m_vMove.y += m_fRiseSpeed;


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

	Cube::SetPos(m_vPos - m_vZ * 100);
}


//=============================================================================
// 移動処理
//=============================================================================
void Player::Move(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 vTemp = D3DXVECTOR3(m_pMtxTorso->_41, m_pMtxTorso->_42, m_pMtxTorso->_43);
	DebugObject::pSphere->SetPos(m_cDebug.nIdx, vTemp + m_vMove);
#endif
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
	m_vPos.y = SetLimit(m_vPos.y, PLAYER_HEIGHT_MAX, PLAYER_HEIGHT_MIN);

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
	SetIdAndPriority(ObjectID::PLAYERMANAGER, Priority::Middle, Priority::Middle);

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
//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "calculate.h"
#include "camera.h"
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
	m_vSlope = ZERO_D3DXVECTOR3;
	m_vSlopeIner = m_vSlope;
	m_vScl = D3DXVECTOR3(PLAYER_SCL, PLAYER_SCL, PLAYER_SCL);

	m_vMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotIner = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_vTarget = D3DXVECTOR3(0.0f, 500.0f, 0.0f);

	m_eMode = MODE_FLOAT;
	m_eModeOld = MODE_MAX;	// 1フレーム目はモードチェンジに入りたい

	m_fMoveSpeed = PLAYER_MOVE_SPEED;
	m_fRiseSpeed = 0.0f;
	m_bUse = true;

	// アタック
	m_nComboTime = 0;
	m_nComboCount = 0;
	m_bAttack = false;
	m_nCharge = 0;
	m_bCharge = false;
	m_bChargeAttack = false;

	// ステップ
	m_eStep = STEP_NON;
	m_bStep = false;
	m_nStepCount = 0;

	// ガード
	m_bGuard = false;

	// アクション
	m_bAction = false;

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
// デストラクタ（終了処理）
//=============================================================================
Player::~Player(void)
{
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
		ImGui::Text("Spd [%f]  Rise[%f]\n", m_fMoveSpeed, m_fRiseSpeed);
		ImGui::Text("vX  [%.2f.%.2f,%.2f]\n", m_vX.x, m_vX.y, m_vX.z);
		ImGui::Text("vY  [%.2f.%.2f,%.2f]\n", m_vY.x, m_vY.y, m_vY.z);
		ImGui::Text("vZ  [%.2f.%.2f,%.2f]\n", m_vZ.x, m_vZ.y, m_vZ.z);
		ImGui::Text("combo[%d] time[%d]\n", m_nComboCount, m_nComboTime);
		ImGui::InputFloat3("Target", &m_vTarget.x,2);
		ImGui::InputFloat3("Slope", &m_vSlope.x, 2);
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

		Action();		// 行動処理

		Attack();		// 攻撃処理
		Guard();		// 防御処理

		UpdateMove();	// 移動処理
		Step();			// ステップ処理

		// カメラ設定
		SetCamera();

		// モードチェンジ
		ModeChange();

		// アニメーションチェック
		CheckAnim();

		// ワールド変換
		WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		D3DXMATRIX mtxSlope;

		// 傾き（回転）をさらに適用
		D3DXMatrixRotationYawPitchRoll(&mtxSlope, m_vSlope.y, m_vSlope.x, m_vSlope.z);
		D3DXMatrixMultiply(&mtxSlope, &mtxSlope, &m_mtxWorld);


		if (m_handle >= 0)
		{
			pEffetMgr->SetMatrix(m_handle, mtxSlope);
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
		//WorldConvertAxis(&m_mtxWorld, m_vPos, m_vZ, m_vY, m_vScl);

		D3DXMATRIX mtxSlope;

		// 傾き（回転）をさらに適用
		D3DXMatrixRotationYawPitchRoll(&mtxSlope, m_vSlope.y, m_vSlope.x, m_vSlope.z);
		D3DXMatrixMultiply(&mtxSlope, &mtxSlope, &m_mtxWorld);

		// モデルを描画-
		m_CSkinMesh->Draw(pDevice, mtxSlope);

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
		//pCamera->SetEye(m_vPos + (vX * 30) + (vY * 30) + (vZ * 100));
		pCamera->SetEye(m_vPos + (vY * 30) + (vZ * 100));

		// カメラUpをモデル上部に設定
		pCamera->SetUp(m_vY);

		// カメラ慣性を減らす
		//pCamera->AddEyeIner(0.05f);
		pCamera->AddAtIner(0.02f);
		break;
	}
}
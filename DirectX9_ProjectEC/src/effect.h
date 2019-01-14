//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : GP12A295 25 人見友基
//
//=============================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "object.h"
#include <tchar.h>

#include <Effekseer\Effekseer.h>
#include <Effekseer\EffekseerRendererDX9.h>
#include <Effekseer\EffekseerSoundXAudio2.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class EffectManager : public ObjectManager
{
private:
	static const WCHAR* c_filename[];

public:
	//struct Prop
	//{
	//	CXModel			*CXModel;			// モデル格納用
	//	D3DXVECTOR3		vPos;				// 座標情報
	//	D3DXVECTOR3		vRot;				// 回転情報
	//	D3DXVECTOR3		vScl;				// 拡縮情報
	//	D3DXMATRIX		mtxWorld;			// ワールドマトリクス
	//	bool			bUse;
	//}*prop;

	enum EFFECT
	{	// プレイヤー管理
		EFFECT1,
		//EFFECT2,
		//EFFECT3,
		//EFFECT4,
		//EFFECT5,
		EFFECT_MAX
	};
	//static const TCHAR* c_filename[EFFECT_MAX];

	::Effekseer::Manager*				m_manager;
	::EffekseerRendererDX9::Renderer*	m_renderer;
	::Effekseer::Effect*				m_effect[EFFECT_MAX];
	::Effekseer::Handle					m_handle;
	::Effekseer::Vector3D				m_position;
	::Effekseer::Vector3D				m_rotation;
	::Effekseer::Matrix44				mtx44View, mtx44Pro;

	//// サウンド
	//::EffekseerSound::Sound*			m_sound;
	//IXAudio2*						m_xa2 = NULL;
	//IXAudio2MasteringVoice*			m_xa2_master = NULL;

	// コンストラクタ（初期化処理）
	EffectManager(void);
	//デストラクタ（終了処理）
	~EffectManager(void);

	// 更新処理
	void	Update(void);
	// 描画処理
	void	Draw(void);
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

#endif
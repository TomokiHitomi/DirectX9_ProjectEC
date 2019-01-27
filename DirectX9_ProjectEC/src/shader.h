//=============================================================================
//
// �V�F�[�_���� [shader.h]
// Author : GP12A295 25 �l���F��
//
//=============================================================================
#ifndef _SHADER_H_
#define _SHADER_H_

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SHADER_FILE_XTEXTURE		("data/shader/xtexture.fx")
#define SHADER_FILE_XMODEL			("data/shader/xmodel.fx")
#define SHADER_FILE_SKINMESH		("data/shader/skinmesh.fx")
#define SHADER_FILE_WING_FEATHER	("data/shader/wing_feather.fx")
#define SHADER_FILE_CLOUD			("data/shader/cloud.fx")
#define SHADER_FILE_OCTA			("data/shader/octa.fx")
#define SHADER_FILE_DEBUGSPHERE		("data/shader/debugsphere.fx")

#define SHR(p,n) { if(FAILED(p)) { MessageBox(NULL, "�V�F�[�_�ւ̃f�[�^�]���Ɏ��s", n , MB_OK); } }

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Shader
{
public:
	// �V�F�[�_�[�֘A
	LPD3DXBUFFER					pErrorBuff;
	LPD3DXEFFECT					pEffect;

	Shader(const char* path);
	~Shader();

	HRESULT Init(const char* path);
	void Release(void);

	// �G�t�F�N�g�擾���\�b�h
	LPD3DXEFFECT GetEffect(void) { return pEffect; }
};

class ShaderManager
{
public:
	enum FILE
	{
		XTEXTURE,
		XMODEL,
		SKINMESH,
		WING_FEATHER,
		CLOUD,
		OCTA,
		DEBUGSPHERE,
		FILE_MAX
	};
private:
	static Shader* pShader[FILE_MAX];
	static LPD3DXBUFFER	pErrorBuff;
public:
	ShaderManager();
	~ShaderManager();
	static HRESULT Load(void);
	static void Delete(void);
	// �G�t�F�N�g�擾���\�b�h
	static HRESULT CreateEffect(LPD3DXEFFECT* pEffect,FILE eFile);
	static LPD3DXEFFECT GetEffect(FILE eFile) { return pShader[eFile]->GetEffect(); }

};
//
//class ShaderProp
//{
//public:
//	// �V�F�[�_�[�֘A
//	LPD3DXBUFFER					pErrorBuff;
//	LPD3DXEFFECT					pEffect;
//	UINT							numPass;
//
//	Shader(const char* path);
//	~Shader();
//
//	HRESULT Init(const char* path);
//	void Release(void);
//
//	// �G�t�F�N�g�擾���\�b�h
//	LPD3DXEFFECT GetEffect(void) { return pEffect; }
//};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetShaderMat(LPD3DXEFFECT pEffect, D3DMATERIAL9 pMat);
void SetShaderLight(LPD3DXEFFECT pEffect, D3DLIGHT9 pLight);

#endif
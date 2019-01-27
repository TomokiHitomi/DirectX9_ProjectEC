//=============================================================================
//
// アニメーションモデル処理 [SkinMeshX.cpp]
// Author : GP12A295 25 人見友基
//
// 参考：GESブログ
// http://ges.blog.shinobi.jp/directx/directx%209%E3%80%80%E3%82%B9%E3%82%AD%E3%83%B3%E3%83%A1%E3%83%83%E3%82%B7%E3%83%A5%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SkinMeshX.h"
#include "camera.h"
#include "light.h"

// デバッグ用
#ifdef _DEBUG
#include "debugproc.h"
#endif


HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer);

enum METHOD
{
	D3DNONINDEXED,
	D3DINDEXED,
	SOFTWARE,
	D3DINDEXEDVS,
	D3DINDEXEDHLSLVS,
	NONE
};

METHOD		g_SkinningMethod = D3DINDEXEDHLSLVS; // Current skinning method

UINT                        g_NumBoneMatricesMax = 0;
D3DXMATRIXA16*              g_pBoneMatrices = NULL;
DWORD                       g_dwBehaviorFlags;      // Behavior flags of the 3D device

CHAR						g_cDirPassSkin[256];


bool		g_bUseSoftwareVP;	// Flag to indicate whether software vp is

CHAR* HeapCopy(CHAR* sName)
{
	DWORD dwLen = (DWORD)strlen(sName) + 1;
	CHAR* sNewName = new CHAR[dwLen];
	if (sNewName)
		strcpy_s(sNewName, dwLen, sName);
	return sNewName;
}

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//--------------------------------------------------------------------------------------
HRESULT AllocateName(LPCSTR Name, LPSTR* pNewName)
{
	UINT cbLength;

	if (Name != NULL)
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if (*pNewName == NULL)
			return E_OUTOFMEMORY;
		memcpy(*pNewName, Name, cbLength * sizeof(CHAR));
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}


//=============================================================================
// フレーム格納関数
//=============================================================================
HRESULT MY_HIERARCHY::CreateFrame(LPCTSTR Name, LPD3DXFRAME *ppNewFrame)
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED *pFrame;
	//新しいフレームアドレス格納用変数を初期化
	*ppNewFrame = NULL;
	//フレームの領域確保
	pFrame = new D3DXFRAME_DERIVED;
	//領域確保の失敗時の処理
	if (pFrame == NULL)
	{
		return E_OUTOFMEMORY;
	}
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));


	//フレーム名格納用領域確保
	if (Name)
		pFrame->Name = (CHAR *)HeapCopy((CHAR *)Name);
	else
	{
		// TODO: Add a counter to append to the string below
		//       so that we are using a different name for
		//       each bone.
		pFrame->Name = NULL;
	}

	////フレーム名格納用領域確保
	//pFrame->Name = new TCHAR[lstrlen(Name) + 1];
	////領域確保の失敗時の処理
	//if (!pFrame->Name)
	//{
	//	return E_FAIL;
	//}
	// //strcpy(pFrame->Name,Name);
	////フレーム名格納
	//strcpy_s(pFrame->Name, lstrlen(Name) + 1, Name);

	//行列の初期化
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
	//追加：オフセット関係初期化
	//pFrame->OffsetID = 0xFFFFFFFF;
	D3DXMatrixIdentity(&(pFrame->CombinedTransformationMatrix));
	//新規フレームのメッシュコンテナ初期化
	pFrame->pMeshContainer = NULL;
	//新規フレームの兄弟フレームアドレス格納用変数初期化
	pFrame->pFrameSibling = NULL;
	//新規フレームの子フレームアドレス格納用変数初期化
	pFrame->pFrameFirstChild = NULL;
	//外部の新規フレームアドレス格納用変数に、作成したフレームのアドレスを格納
	*ppNewFrame = pFrame;
	return S_OK;
}

//=============================================================================
// メッシュコンテナー作成関数
//=============================================================================
//HRESULT MY_HIERARCHY::CreateMeshContainer
//メッシュコンテナーを作成する
HRESULT MY_HIERARCHY::CreateMeshContainer(
	LPCSTR Name,
	CONST D3DXMESHDATA* pMeshData,
	CONST D3DXMATERIAL* pMaterials,
	CONST D3DXEFFECTINSTANCE* pEffectInstances,
	DWORD NumMaterials,
	CONST DWORD *pAdjacency,
	LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER *ppMeshContainer)
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;
	UINT iMaterial;
	UINT iBone, cBones;
	LPDIRECT3DDEVICE9 pDevice = NULL;

	LPD3DXMESH pMesh = NULL;

	*ppMeshContainer = NULL;

	// this sample does not handle patch meshes, so fail when one is found
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// get the pMesh interface pointer out of the mesh data structure
	pMesh = pMeshData->pMesh;

	// this sample does not FVF compatible meshes, so fail when one is found
	if (pMesh->GetFVF() == 0)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// allocate the overloaded structure to return as a D3DXMESHCONTAINER
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if (pMeshContainer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
		goto e_Exit;

	pMesh->GetDevice(&pDevice);
	NumFaces = pMesh->GetNumFaces();

	// if no normals are in the mesh, add them
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// clone the mesh to make room for the normals
		hr = pMesh->CloneMeshFVF(pMesh->GetOptions(),
			pMesh->GetFVF() | D3DFVF_NORMAL,
			pDevice, &pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;

		// get the new pMesh pointer back out of the mesh container to use
		// NOTE: we do not release pMesh because we do not have a reference to it yet
		pMesh = pMeshContainer->MeshData.pMesh;

		// now generate the normals for the pmesh
		D3DXComputeNormals(pMesh, NULL);
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// allocate memory to contain the material information.  This sample uses
	//   the D3D9 materials and texture names instead of the EffectInstance style materials
	pMeshContainer->NumMaterials = max(1, NumMaterials);
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];
	if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);
	memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

	// if materials provided, copy them
	if (NumMaterials > 0)
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
		{
			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
			{
				//テクスチャのファイルパス保存用変数
				TCHAR strTexturePath[MAX_PATH];
				//テクスチャのファイルパスを保存(再読み込み時に必要)
				strcpy_s(strTexturePath, lstrlen(pMeshContainer->pMaterials[iMaterial].pTextureFilename) + 1, pMeshContainer->pMaterials[iMaterial].pTextureFilename);
				CHAR TexMeshPass[255];
				strcpy_s(TexMeshPass, sizeof(TexMeshPass), g_cDirPassSkin);
				strcat_s(TexMeshPass, sizeof(TexMeshPass) - strlen(TexMeshPass) - strlen(strTexturePath) - 1, strTexturePath);
				//テクスチャ情報の読み込み
				if (FAILED(hr = D3DXCreateTextureFromFile(
					pDevice,
					TexMeshPass,
					&pMeshContainer->ppTextures[iMaterial])))
				{
					////失敗時の処理
					////テクスチャファイル名格納用
					//CHAR TexMeshPass[255];
					////追記
					////もしなければ、Graphフォルダを調べる
					////注）ファイル名の結合時に、必ず両方にファイル名がある事を確認してから
					////  strcpy_sとstrcat_sを使うようにする(この場合は、上にある 
					////    テクスチャのファイルがあり、さらにそのファイル名の長さが0でなければ の所のif文)。
					////  TexMeshPassに、Xファイルがある場所と同じディレクトリと、テクスチャのファイル名を
					////  結合したものを格納
					//// strcpy_s( TexMeshPass, sizeof( TexMeshPass ) , "./../Source/Graph/" );
					//strcpy_s(TexMeshPass, sizeof(TexMeshPass), "./Graph/");
					//strcat_s(TexMeshPass, sizeof(TexMeshPass) - strlen(TexMeshPass) - strlen(strTexturePath) - 1, strTexturePath);
					////テクスチャ情報の読み込み
					//if (FAILED(D3DXCreateTextureFromFile(pDevice, TexMeshPass,
					//	&pMeshContainer->ppTextures[iMaterial])))
					//{
					//	//MessageBox(NULL, "アニメーションXファイルのテクスチャ読み込みに失敗しました", TexMeshPass, MB_OK);
					//	pMeshContainer->ppTextures[iMaterial] = NULL;
					//}
					//テクスチャのファイルパスをNULLにする
					pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
				}
			}
			pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse.r = 1.0f;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse.g = 1.0f;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse.b = 1.0f;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient.r = 0.9f;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient.g = 0.9f;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient.b = 0.9f;
		}
	}
	else // if no materials provided, use a default one
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// if there is skinning information, save off the required data and then setup for HW skinning
	if (pSkinInfo != NULL)
	{
		// first save off the SkinInfo and original mesh data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if (pMeshContainer->pBoneOffsetMatrices == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// get each of the bone offset matrices so that we don't need to get them later
		for (iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
		}

		// GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
		hr = GenerateSkinnedMesh(pDevice, pMeshContainer);
		if (FAILED(hr))
			goto e_Exit;
	}

	*ppMeshContainer = pMeshContainer;
	pMeshContainer = NULL;

e_Exit:
	SAFE_RELEASE(pDevice);

	// call Destroy function to properly clean up the memory allocated 
	if (pMeshContainer != NULL)
	{
		DestroyMeshContainer(pMeshContainer);
	}

	return hr;
}

//=============================================================================
// フレーム破棄関数
//=============================================================================
//HRESULT MY_HIERARCHY::DestroyFrame(LPD3DXFRAME pFrameToFree) 
//フレームを破棄する
HRESULT MY_HIERARCHY::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	// 2重解放防止
	// if (pFrameToFree == NULL)return S_FALSE;
	SAFE_DELETE_ARRAY(pFrameToFree->Name);
	if (pFrameToFree->pFrameFirstChild)
	{
		DestroyFrame(pFrameToFree->pFrameFirstChild);
	}
	if (pFrameToFree->pFrameSibling)
	{
		DestroyFrame(pFrameToFree->pFrameSibling);
	}
	SAFE_DELETE(pFrameToFree);
	return S_OK;
}

//=============================================================================
// メッシュコンテナー破棄関数
//=============================================================================
//HRESULT MY_HIERARCHY::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
//メッシュコンテナーを破棄する
HRESULT MY_HIERARCHY::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	int iMaterial;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	if (pMeshContainer->ppTextures != NULL)
	{
		for (iMaterial = 0; (DWORD)iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			//テクスチャ解放
			SAFE_RELEASE(pMeshContainer->ppTextures[iMaterial]);
		}
	}
	SAFE_DELETE_ARRAY(pMeshContainer->ppTextures);
	SAFE_RELEASE(pMeshContainer->pSkinInfo);


	SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);



	SAFE_RELEASE(pMeshContainer->pOrigMesh);
	if (pMeshContainer->pBoneCombinationBuf != NULL)
	{
		SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);
		SAFE_DELETE_ARRAY(pMeshContainer->pBoneOffsetMatrices);
	}
	SAFE_DELETE(pMeshContainer);
	pMeshContainerBase = NULL;
	return S_OK;
}

//=============================================================================
// 初期化関数
//=============================================================================
CSkinMesh::CSkinMesh()
{
	//マテリアル変更フラグ
	m_MaterialFlg = FALSE;
	//マテリアルデータ
	ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));
	//単位行列化
	D3DXMatrixIdentity(&(this->m_World));
	//アニメーション時間初期化
	m_AnimeTime = 0;
	//アニメーションスピード初期化
	m_AnimSpeed = SKIN_ANIME_SPEED; //固定
	//現在のアニメーショントラック初期化
	m_CurrentTrack = 0;
	//アニメーションデータトラック初期化
	//有効にする
	m_CurrentTrackDesc.Enable = TRUE;
	//影響度100%
	m_CurrentTrackDesc.Weight = 1;
	//開始位置初期化
	m_CurrentTrackDesc.Position = 0;
	//速度
	m_CurrentTrackDesc.Speed = 1;
	// 次のアニメーションへシフトするのにかかる時間
	m_fShiftTime = SKIN_ANIME_WEIGHT;

	// シェーダを初期化
	pEffect = NULL;
}

//=============================================================================
// ボーン解放関数
//=============================================================================
VOID CSkinMesh::Release()
{
	if (m_pFrameRoot != NULL) {
		//ボーンフレーム関係解放
		FreeAnim(m_pFrameRoot);
		//その他情報(テクスチャの参照データなど)の解放
		m_cHierarchy.DestroyFrame(m_pFrameRoot);
		m_pFrameRoot = NULL;
	}
	for (DWORD i = 0; i < m_pAnimController->GetNumAnimationSets(); i++)
	{
		//アニメーションセットの解放
		SAFE_RELEASE(m_pAnimSet[i]);
	}
	//アニメーションコントローラー解放
	SAFE_RELEASE(m_pAnimController);
	//すべてのフレーム参照変数の要素を削除
	m_FrameArray.clear();
	//メッシュコンテナありのフレーム参照変数の要素を削除
	m_IntoMeshFrameArray.clear();

	// シェーダの解放
	//SAFE_RELEASE(pEffect);
}

//=============================================================================
// ボーン行列取得関数
//=============================================================================
HRESULT CSkinMesh::AllocateBoneMatrix(LPD3DXFRAME pFrameRoot, LPD3DXMESHCONTAINER pMeshContainerBase)
{
	D3DXFRAME_DERIVED *pFrame = NULL;
	DWORD dwBoneNum = 0;
	//メッシュコンテナの型をオリジナルの型として扱う
	//(メッシュコンテナ生成時にオリジナルの型として作っているので問題はないが、
	//基本ダウンキャストは危険なので多用は避けるべき)
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	//スキンメッシュでなければ
	if (pMeshContainer->pSkinInfo == NULL)
	{
		return S_OK;
	}
	//ボーンの数取得
	dwBoneNum = pMeshContainer->pSkinInfo->GetNumBones();
	//各ボーンのワールド行列格納用領域を確保
	SAFE_DELETE(pMeshContainer->ppBoneMatrixPtrs);
	pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[dwBoneNum];
	//ボーンの数だけループ
	for (DWORD i = 0; i < dwBoneNum; i++)
	{
		//子フレーム(ボーン)のアドレスを検索してpFrameに格納
		pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(i));
		//子フレームがなければ処理を終了する
		if (pFrame == NULL)
		{
			return E_FAIL;
		}
		//各ボーンのワールド行列格納用変数に最終行列を格納
		pMeshContainer->ppBoneMatrixPtrs[i] = &pFrame->CombinedTransformationMatrix;
	}
	return S_OK;
}

//=============================================================================
// ボーン行列初期化関数
//=============================================================================
//HRESULT AllocateAllBoneMatrices( THING* pThing,LPD3DXFRAME pFrame )
HRESULT CSkinMesh::AllocateAllBoneMatrices(LPD3DXFRAME pFrameRoot, LPD3DXFRAME pFrame)
{
	//階層の走査(メモリを確保したメッシュコンテナ領域を探す処理)
	if (pFrame->pMeshContainer != NULL)
	{
		//ボーン行列の初期化処理
		if (FAILED(AllocateBoneMatrix(pFrameRoot, pFrame->pMeshContainer)))
		{
			return E_FAIL;
		}
	}
	//再起判断処理
	if (pFrame->pFrameSibling != NULL)
	{
		if (FAILED(AllocateAllBoneMatrices(pFrameRoot, pFrame->pFrameSibling)))
		{
			return E_FAIL;
		}
	}
	if (pFrame->pFrameFirstChild != NULL)
	{
		if (FAILED(AllocateAllBoneMatrices(pFrameRoot, pFrame->pFrameFirstChild)))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

//=============================================================================
// メッシュレンダリング関数
//=============================================================================
//VOID RenderMeshContainer(LPDIRECT3DDEVICE9 pDevice,MYMESHCONTAINER* pMeshContainer, MYFRAME* pFrame)
//フレーム内のそれぞれのメッシュをレンダリングする
VOID CSkinMesh::RenderMeshContainer(LPDIRECT3DDEVICE9 pDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainerBase, D3DXFRAME_DERIVED* pFrameBase)
{
	//DWORD i, k;
	//DWORD dwBlendMatrixNum;
	//DWORD dwPrevBoneID;
	//LPD3DXBONECOMBINATION pBoneCombination;
	//UINT iMatrixIndex;
	//D3DXMATRIX mStack;

	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	pDevice->GetDeviceCaps(&d3dCaps);

	D3DDEVICE_CREATION_PARAMETERS cp;
	pDevice->GetCreationParameters(&cp);
	g_dwBehaviorFlags = cp.BehaviorFlags;

	D3DXMATRIX  mtxView, mtxProjection;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);

	//スキンメッシュの描画
	if (pMeshContainer->pSkinInfo != NULL)
	{
		if (pMeshContainer->UseSoftwareVP)
		{
			// If hw or pure hw vertex processing is forced, we can't render the
			// mesh, so just exit out.  Typical applications should create
			// a device with appropriate vertex processing capability for this
			// skinning method.
			if (g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
				return;

			pDevice->SetSoftwareVertexProcessing(TRUE);
		}

		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			// first calculate all the world matrices
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					//D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
					//	pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
					//D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &matTemp, &mtxView);
					D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
						pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
				}
			}

			// テクスチャある場合
			if (pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId])
			{
				// テクニック t0 を使用
				if (FAILED(pEffect->SetTechnique("t0")))
				{
					// エラー
					MessageBox(NULL, "テクニックのセットに失敗しました。", "t0", MB_OK);
				}

				// テクスチャをセット
				if (FAILED(pEffect->SetTexture("tex", pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId])))
				{
					// エラー
					MessageBox(NULL, "テクスチャのセットに失敗しました。", "tex", MB_OK);
				}
			}
			else
			{
				// テクニック t1 を使用
				if (FAILED(pEffect->SetTechnique("t1")))
				{
					// エラー
					MessageBox(NULL, "テクニックのセットに失敗しました。", "t1", MB_OK);
				}
			}

			{	// ライトON
				Camera* pCamera = CameraManager::GetCameraNow();
				D3DXVECTOR4 eyeTmp = D3DXVECTOR4(pCamera->GetEye(), 0.0f);
				if (FAILED(pEffect->SetVector("eye", &eyeTmp)))
				{
					// エラー
					MessageBox(NULL, "カメラEye情報のセットに失敗しました。", "eye", MB_OK);
				}
				// ライト情報を取得
				Light* pLight = LightManager::GetLightAdr(LightManager::Main);
				// ライト情報をセット
				if (FAILED(pEffect->SetValue("lt", &pLight->value, sizeof(Light::LIGHTVALUE))))
				{
					// エラー
					MessageBox(NULL, "ライト情報のセットに失敗しました。", "lt", MB_OK);
				}
			}

			pEffect->SetMatrixArray("mWorldMatrixArray", g_pBoneMatrices,
				pMeshContainer->NumPaletteEntries);

			// Sum of all ambient and emissive contribution
			//D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
			//D3DXCOLOR color2(.25, .25, .25, 1.0);
			//D3DXCOLOR ambEmm;
			//D3DXColorModulate(&ambEmm, &color1, &color2);
			//ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

			//// set material color properties 
			//pEffect->SetVector("MaterialDiffuse",
			//	(D3DXVECTOR4*)&(
			//		pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
			//pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

			pEffect->SetValue("mat", &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D, sizeof(D3DMATERIAL9));


			// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
			//pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

			// Set CurNumBones to select the correct vertex shader for the number of bones
			pEffect->SetInt("CurNumBones", pMeshContainer->NumInfl - 1);

			pEffect->SetMatrix("mViewProj", &mtxProjection);
			pEffect->SetMatrix("mView", &mtxView);

			// 結果を確定させる
			pEffect->CommitChanges();

			// Start the effect now all parameters have been updated
			UINT numPasses;
			pEffect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
			for (UINT iPass = 0; iPass < numPasses; iPass++)
			{
				pEffect->BeginPass(iPass);

				// draw the subset with the current world matrix palette and material state
				pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);

				pEffect->EndPass();
			}

			pEffect->End();

			// 固定機能に戻す
			pDevice->SetVertexShader(NULL);
			pDevice->SetPixelShader(NULL);
		}

		// remember to reset back to hw vertex processing if software was required
		if (pMeshContainer->UseSoftwareVP)
		{
			pDevice->SetSoftwareVertexProcessing(FALSE);
		}
	}


	////#ifdef _DEBUG
	////		PrintDebugProc("Container  [%d]\n", m_dwContainerCount);
	////#endif
	//		//ボーンテーブルからバッファの先頭アドレスを取得
	//		pBoneCombination = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
	//		//dwPrevBoneIDにUINT_MAXの値(0xffffffff)を格納
	//		dwPrevBoneID = UINT_MAX;
	//		//スキニング計算
	//		for (i = 0; i < pMeshContainer->NumAttributeGroups; i++)
	//		{
	//			dwBlendMatrixNum = 0;
	//			//影響している行列数取得
	//			for (k = 0; k< pMeshContainer->NumInfl; k++)
	//			{
	//				//UINT_MAX(-1)
	//				if (pBoneCombination[i].BoneId[k] != UINT_MAX)
	//				{
	//					//現在影響を受けているボーンの数
	//					dwBlendMatrixNum = k;
	//				}
	//			}
	//			//ジオメトリブレンディングを使用するために行列の個数を指定
	//			pDevice->SetRenderState(D3DRS_VERTEXBLEND, dwBlendMatrixNum);
	//			//影響している行列の検索
	//			for (k = 0; k < pMeshContainer->NumInfl; k++)
	//			{
	//				//iMatrixIndexに1度の呼び出しで描画出来る各ボーンを識別する値を格納
	//				//( このBoneID配列の長さはメッシュの種類によって異なる
	//				//( インデックスなしであれば　=　頂点ごとの重み であり
	//				// インデックスありであれば　=　ボーン行列パレットのエントリ数)
	//				//現在のボーン(i番目)からみてk番目のボーンid
	//				iMatrixIndex = pBoneCombination[i].BoneId[k];
	//				//行列の情報があれば
	//				if (iMatrixIndex != UINT_MAX)
	//				{
	//					//mStackにオフセット行列*ボーン行列を格納
	//					mStack = pMeshContainer->pBoneOffsetMatrices[iMatrixIndex] * (*pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
	//					//行列スタックに格納
	//					pDevice->SetTransform(D3DTS_WORLDMATRIX(k), &mStack);
	//				}
	//			}
	//
	//
	//
	//			D3DMATERIAL9 TmpMat = pMeshContainer->pMaterials[pBoneCombination[i].AttribId].MatD3D;
	//			TmpMat.Emissive.a = TmpMat.Diffuse.a = TmpMat.Ambient.a = 1.0f;
	//			pDevice->SetMaterial(&TmpMat);
	//			pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneCombination[i].AttribId]);
	//			//dwPrevBoneIDに属性テーブルの識別子を格納
	//			dwPrevBoneID = pBoneCombination[i].AttribId;
	//			//メッシュの描画
	//			//pMeshContainer->MeshData.pMesh->DrawSubset(i);
	//			if (FAILED(pMeshContainer->MeshData.pMesh->DrawSubset(i)))
	//			{
	//				MessageBox(NULL, "描画に失敗しました。", "SkinMeshX", MB_OK);
	//			}
	//
	////#ifdef _DEBUG
	//			//PrintDebugProc("Bone  [Con:%d  ID:%d  Name:%s]\n", 
	//			//	m_dwContainerCount,
	//			//	m_dwBoneCount,
	//			//	pMeshContainer->pSkinInfo->GetBoneName(i));
	////#endif
	//			m_dwBoneCount++;
	//		}
	////#ifdef _DEBUG
	////		PrintDebugProc("BoneMax  [%d]\n", m_dwBoneCount);
	////#endif
		//}
		//通常メッシュの場合
	else
	{
		MessageBox(NULL, "スキンメッシュXファイルの描画に失敗しました。", NULL, MB_OK);
		exit(EOF);
	}
}

////メッシュの現在のMatrixデータ取得
//bool CSkinMesh::GetMatrix(D3DXMATRIX* out,int dwCon, int dwBone)
//{
//	MYFRAME* pFrame = (MYFRAME*)m_pFrameRoot;
//	MYMESHCONTAINER* pMeshContainer = (MYMESHCONTAINER*)pFrame->pMeshContainer;
//
//	for (unsigned int i = 0; i < dwCon; i++)
//	{
//		if (pMeshContainer != NULL)
//		{
//			//次のメッシュコンテナへアクティブを移す
//			pMeshContainer = (MYMESHCONTAINER*)pMeshContainer->pNextMeshContainer;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	if (dwBone <= pMeshContainer->dwBoneNum)
//	{
//		UINT iMatrixIndex;
//		LPD3DXBONECOMBINATION pBoneCombination;
//		//ボーンテーブルからバッファの先頭アドレスを取得
//		pBoneCombination = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneBuffer->GetBufferPointer());
//		iMatrixIndex = pBoneCombination[dwBone].BoneId[0];
//
//		//行列の情報があれば
//		if (iMatrixIndex != UINT_MAX)
//		{
//			//mStackにオフセット行列*ボーン行列を格納
//			*out = pMeshContainer->pBoneOffsetMatrices[iMatrixIndex] * (*pMeshContainer->ppBoneMatrix[iMatrixIndex]);
//		}
//		return true;
//	}
//	return false;
//}

//=============================================================================
// フレームレンダリング関数
//=============================================================================
//VOID DrawFrame(LPDIRECT3DDEVICE9 pDevice,LPD3DXFRAME pFrameBase)
//フレームをレンダリングする。
VOID CSkinMesh::DrawFrame(LPDIRECT3DDEVICE9 pDevice, LPD3DXFRAME pFrameBase)
{
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;
	while (pMeshContainer != NULL)
	{
		//SHADER_KIND a = GetpShader()->GetShaderKind();
		////シェーダを使用しているのなら専用の描画関数に飛ばす
		//if( GetpShader() != NULL && GetpShader()->GetShaderKind() == SHADER_KIND_LAMBERT ){
		// ShaderDraw( pDevice, ControlNum, pMeshContainer, pFrame ); 
		//}else{
		RenderMeshContainer(pDevice, pMeshContainer, pFrame);
		// }
		//次のメッシュコンテナへアクティブを移す
		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
		m_dwContainerCount++;
	}
	if (pFrame->pFrameSibling != NULL)
	{
		DrawFrame(pDevice, pFrame->pFrameSibling);
	}
	if (pFrame->pFrameFirstChild != NULL)
	{
		DrawFrame(pDevice, pFrame->pFrameFirstChild);
	}
}

//=============================================================================
// ワールド更新関数
//=============================================================================
//VOID UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
//フレーム内のメッシュ毎にワールド変換行列を更新する
VOID CSkinMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	if (pParentMatrix != NULL)
	{
		//CombinedTransformationMatrixに最終行列を格納
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
	}
	else
	{
		//CombinedTransformationMatrixに最終行列を格納
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}
	if (pFrame->pFrameSibling != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
	}
	if (pFrame->pFrameFirstChild != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
	}
}

//=============================================================================
// フレーム解放関数
//=============================================================================
//全ての階層フレームを解放する
VOID CSkinMesh::FreeAnim(LPD3DXFRAME pFrame)
{
	if (pFrame->pMeshContainer != NULL) {
		m_cHierarchy.DestroyMeshContainer(pFrame->pMeshContainer);
		pFrame->pMeshContainer = NULL;
	}
	if (pFrame->pFrameSibling != NULL)
		FreeAnim(pFrame->pFrameSibling);
	if (pFrame->pFrameFirstChild != NULL)
		FreeAnim(pFrame->pFrameFirstChild);
}

//=============================================================================
// 初期化関数
//=============================================================================
HRESULT CSkinMesh::Init(LPDIRECT3DDEVICE9 lpD3DDevice, LPSTR pMeshPass) {
	CHAR TmpMeshPass[256];
	strcpy_s(TmpMeshPass, sizeof(TmpMeshPass) - 1, pMeshPass);

	int nMax = 0;
	while (TmpMeshPass[nMax] != NULL)
	{
		nMax++;
	}

	int nSearch = 0;
	for (int i = nMax - 1; i > 0; i--)
	{
		if (TmpMeshPass[i] == '/')
		{
			nSearch = i;
			for (i = 0; i < nSearch + 1; i++)
			{
				g_cDirPassSkin[i] = TmpMeshPass[i];
			}
			g_cDirPassSkin[nSearch + 1] = NULL;
			break;
		}
	}

	// Xファイルからアニメーションメッシュを読み込み作成する
	if (FAILED(
		D3DXLoadMeshHierarchyFromX(
			TmpMeshPass,
			D3DXMESH_MANAGED,
			lpD3DDevice,
			&m_cHierarchy,
			NULL,
			&m_pFrameRoot,
			&m_pAnimController)))
	{
		MessageBox(NULL, "アニメーションXファイルの読み込みに失敗しました", TmpMeshPass, MB_OK);
		return E_FAIL;
	}

	strcpy_s(g_cDirPassSkin, sizeof(g_cDirPassSkin) - 1, "\0");

	// シェーダのアドレスを取得
	pEffect = ShaderManager::GetEffect(ShaderManager::SKINMESH);


	//ボーン行列初期化
	AllocateAllBoneMatrices(m_pFrameRoot, m_pFrameRoot);
	//アニメーショントラックの取得
	for (DWORD i = 0; i < m_pAnimController->GetNumAnimationSets(); i++)
	{
		//アニメーション取得
		m_pAnimController->GetAnimationSet(i, &(m_pAnimSet[i]));
		m_bLoopFlag[i] = true;
	}

	m_fAnimeTime = 0.0f;
	m_fLoopTime = m_pAnimSet[m_CurrentTrack]->GetPeriod();

	//すべてのフレーム参照変数の生成
	m_FrameArray.clear();
	m_IntoMeshFrameArray.clear();
	CreateFrameArray(m_pFrameRoot);

	D3DXFRAME_DERIVED *pFrame = NULL;


	//フレーム配列にオフセット情報作成
	for (DWORD i = 0; i < m_IntoMeshFrameArray.size(); i++) {
		D3DXMESHCONTAINER_DERIVED* pMyMeshContainer = (D3DXMESHCONTAINER_DERIVED*)m_IntoMeshFrameArray[i]->pMeshContainer;
		while (pMyMeshContainer) {
			//スキン情報
			if (pMyMeshContainer->pSkinInfo) {
				DWORD cBones = pMyMeshContainer->pSkinInfo->GetNumBones();
				for (DWORD iBone = 0; iBone < cBones; iBone++)
				{
					pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMyMeshContainer->pSkinInfo->GetBoneName(iBone));
					if (pFrame == NULL)
					{
						return E_FAIL;
					}
					pMyMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;


					////フレーム内から同じ名前のフレームを検索
					//for (DWORD Idx = 0; Idx<m_FrameArray.size(); Idx++) {
					//	if (strcmp(pMyMeshContainer->pSkinInfo->GetBoneName(iBone), m_FrameArray[Idx]->Name) == 0) {
					//		pMyMeshContainer->BoneFrameArray.push_back(m_FrameArray[Idx]);
					//		//Offset行列
					//		m_FrameArray[Idx]->OffsetMat = *(pMyMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
					//		m_FrameArray[Idx]->OffsetID = Idx;
					//		break;
					//	}
					//}
				}
			}
			//次へ
			pMyMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMyMeshContainer->pNextMeshContainer;
		}
	}
	return S_OK;
}

//=============================================================================
// フレームポインタ格納処理関数
//=============================================================================
VOID CSkinMesh::CreateFrameArray(LPD3DXFRAME _pFrame) {
	if (_pFrame == NULL) { return; }
	//フレームアドレス格納
	D3DXFRAME_DERIVED* pMyFrame = (D3DXFRAME_DERIVED*)_pFrame;
	m_FrameArray.push_back(pMyFrame);
	//メッシュコンテナがある場合はIntoMeshFrameArrayにアドレスを格納
	if (pMyFrame->pMeshContainer != NULL) {
		m_IntoMeshFrameArray.push_back(pMyFrame);
	}
	//子フレーム検索
	if (pMyFrame->pFrameFirstChild != NULL) {
		CreateFrameArray(pMyFrame->pFrameFirstChild);
	}
	//兄弟フレーム検索
	if (pMyFrame->pFrameSibling != NULL) {
		CreateFrameArray(pMyFrame->pFrameSibling);
	}
}

//=============================================================================
// 更新処理
//=============================================================================
VOID CSkinMesh::Update(void)
{
	// モーションブレンド確認
	m_fCurWeight += m_fShiftTime;

	if (m_fCurWeight <= 1.0f)
	{
		// ブレンド中
		m_pAnimController->SetTrackWeight(0, m_fCurWeight);			// 現在のアニメーション
		m_pAnimController->SetTrackWeight(1, 1.0f - m_fCurWeight);	// 前のアニメーション
	}
	else
	{
		// ブレンド終了。通常アニメーションをするTrack0のウェイトを最大値に固定
		m_pAnimController->SetTrackWeight(0, 1.0f);					// 現在のアニメーション
		m_pAnimController->SetTrackEnable(1, false);				// 前のアニメーションを無効にする
	}

	//現在のアニメーション番号を適応
	m_pAnimController->SetTrackAnimationSet(0, m_pAnimSet[m_CurrentTrack]);
	//0(再生中の)トラックからトラックデスクをセットする
	m_pAnimController->SetTrackDesc(0, &(m_CurrentTrackDesc));

	// アニメーション時間を加算
	m_fAnimeTime += m_AnimSpeed;

	// ループフラグが true の場合はアニメーション時間データの更新
	if (m_bLoopFlag[m_CurrentTrack])
		m_pAnimController->AdvanceTime(m_AnimSpeed, NULL);
	// false の場合は1ループ目まで更新
	else
	{
		// 1ループの時間を上回るまで更新
		if (m_fLoopTime > m_fAnimeTime)
			m_pAnimController->AdvanceTime(m_AnimSpeed, NULL);
	}

	//アニメーション時間を更新
	//m_AnimeTime++;
}

//=============================================================================
// スキンメッシュ描画関数
//=============================================================================
VOID CSkinMesh::Draw(LPDIRECT3DDEVICE9 lpD3DDevice, D3DXMATRIX _World)
{
		//マトリックス行列反映
	m_World = _World;
	// メッシュコンテナカウンタを初期化
	m_dwContainerCount = 0;

	//アニメーションデータを更新
	UpdateFrameMatrices(m_pFrameRoot, &m_World);
	//アニメーション描画
	DrawFrame(lpD3DDevice, m_pFrameRoot);
	//0(再生中の)トラックから更新したトラックデスクを取得する
	m_pAnimController->GetTrackDesc(0, &m_CurrentTrackDesc);
}

//=============================================================================
// アニメーション変更関数
//=============================================================================
//オブジェクトのアニメーション変更( 変更するアニメーション番号 )
VOID CSkinMesh::ChangeAnim(DWORD _NewAnimNum, FLOAT fShift)
{
	if (m_CurrentTrack == _NewAnimNum)
	{	// 異なるアニメーションであるかチェック
		return;
	}

	// 現在のアニメーションセットの設定値を取得
	D3DXTRACK_DESC TD;   // トラックの能力
	m_pAnimController->GetTrackDesc(0, &TD);
	// 現在のアニメーションをトラック1へ移行
	m_pAnimController->SetTrackAnimationSet(1, m_pAnimSet[m_CurrentTrack]);
	m_pAnimController->SetTrackDesc(1, &TD);

	// 現在のアニメーションを保管
	m_OldTrack = m_CurrentTrack;
	// 新規アニメーションに変更
	m_CurrentTrack = _NewAnimNum;

	//アニメーションタイムを初期化
	m_AnimeTime = 0;
	// アニメーションを最初の位置から再生させる
	m_pAnimController->GetTrackDesc(0, &m_CurrentTrackDesc);
	m_CurrentTrackDesc.Position = 0;
	m_pAnimController->SetTrackDesc(0, &m_CurrentTrackDesc);

	// ウェイトタイムを初期化
	m_fCurWeight = 0.0f;
	// 次のアニメーションへシフトするのにかかる時間を設定
	m_fShiftTime = fShift;

	// トラックの合成を許可
	m_pAnimController->SetTrackEnable(0, true);
	m_pAnimController->SetTrackEnable(1, true);

	m_fAnimeTime = 0.0f;
	m_fLoopTime = m_pAnimSet[m_CurrentTrack]->GetPeriod();
}

//=============================================================================
// 対象ボーン検索関数
//=============================================================================
D3DXFRAME_DERIVED* CSkinMesh::SearchBoneFrame(LPSTR _BoneName, D3DXFRAME* _pFrame) {
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)_pFrame;
	if (pFrame->Name != NULL)
	{
		if (strcmp(pFrame->Name, _BoneName) == 0) {
			return pFrame;
		}
	}
	if (_pFrame->pFrameSibling != NULL)
	{
		pFrame = SearchBoneFrame(_BoneName, _pFrame->pFrameSibling);
		if (pFrame != NULL) {
			return pFrame;
		}
	}
	if (_pFrame->pFrameFirstChild != NULL)
	{
		pFrame = SearchBoneFrame(_BoneName, _pFrame->pFrameFirstChild);
		if (pFrame != NULL) {
			return pFrame;
		}
	}
	return NULL;
}

////=============================================================================
//// ボーン数検索関数
////=============================================================================
//D3DXFRAME_DERIVED* CSkinMesh::SearchBoneFrameNum(int* _BoneNum, D3DXFRAME* _pFrame)
//{
//	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)_pFrame;
//
//	// ボーンの数をカウントアップ
//	*_BoneNum += 1;
//
//	if (_pFrame->pFrameSibling != NULL)
//	{
//		pFrame = SearchBoneFrameNum(_BoneNum, _pFrame->pFrameSibling);
//		if (pFrame != NULL) {
//			return pFrame;
//		}
//	}
//	if (_pFrame->pFrameFirstChild != NULL)
//	{
//		pFrame = SearchBoneFrameNum(_BoneNum, _pFrame->pFrameFirstChild);
//		if (pFrame != NULL) {
//			return pFrame;
//		}
//	}
//	return NULL;
//}
//
////=============================================================================
//// 全ボーンのマトリクスポインタの取得関数
////=============================================================================
//D3DXFRAME_DERIVED* CSkinMesh::SearchBoneFrameAllAdr(int* _BoneNum, D3DXMATRIX* _pMtxBoneArray, D3DXFRAME* _pFrame)
//{
//	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)_pFrame;
//
//	// ボーンの数をカウントアップ
//	_pMtxBoneArray[*_BoneNum] = pFrame->CombinedTransformationMatrix;
//	*_BoneNum += 1;
//
//	if (_pFrame->pFrameSibling != NULL)
//	{
//		pFrame = SearchBoneFrameAllAdr(_BoneNum, _pMtxBoneArray,_pFrame->pFrameSibling);
//		if (pFrame != NULL) {
//			return pFrame;
//		}
//	}
//	if (_pFrame->pFrameFirstChild != NULL)
//	{
//		pFrame = SearchBoneFrameAllAdr(_BoneNum, _pMtxBoneArray, _pFrame->pFrameFirstChild);
//		if (pFrame != NULL) {
//			return pFrame;
//		}
//	}
//	return NULL;
//}

//=============================================================================
// ボーンのマトリクス取得関数
//=============================================================================
D3DXMATRIX CSkinMesh::GetBoneMatrix(LPSTR _BoneName)
{
	D3DXFRAME_DERIVED* pFrame = SearchBoneFrame(_BoneName, m_pFrameRoot);
	//ボーンが見つかれば
	if (pFrame != NULL) {
		//ボーン行列を返す
		return pFrame->CombinedTransformationMatrix;
	}
	//ボーンが見つからなければ
	else {
		//単位行列を返す
		D3DXMATRIX TmpMatrix;
		D3DXMatrixIdentity(&TmpMatrix);
		return TmpMatrix;
	}
}

//=============================================================================
// ボーンのマトリクスポインタ取得関数
//=============================================================================
D3DXMATRIX* CSkinMesh::GetBoneMatrixAdr(LPSTR _BoneName)
{
	/////////////////////////////////////
	//注意）RokDeBone用に設定(対象ボーンの一つ先の行列をとってくる)
	D3DXFRAME_DERIVED* pFrame = SearchBoneFrame(_BoneName, m_pFrameRoot);
	//ボーンが見つかれば
	if (pFrame != NULL) {
		return &pFrame->CombinedTransformationMatrix;
	}
	//ボーンが見つからなければ
	else {
		//NULLを返す
		return NULL;
	}
}

////=============================================================================
//// ボーンのマトリクスポインタ取得関数
////=============================================================================
//VOID CSkinMesh::GetBoneMtxAll(D3DXMATRIX** pMtxBoneArray)
//{
//	int nBoneNum = 0;
//	SearchBoneFrameNum(&nBoneNum, m_pFrameRoot);
//
//	*pMtxBoneArray = new D3DXMATRIX[nBoneNum];
//
//	nBoneNum = 0;
//
//	SearchBoneFrameAllAdr(&nBoneNum, *pMtxBoneArray, m_pFrameRoot);
//}

//--------------------------------------------------------------------------------------
// Called either by CreateMeshContainer when loading a skin mesh, or when 
// changing methods.  This function uses the pSkinInfo of the mesh 
// container to generate the desired drawable mesh and bone combination 
// table.
//--------------------------------------------------------------------------------------
HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer)
{
	HRESULT hr = S_OK;
	D3DCAPS9 d3dCaps;
	pd3dDevice->GetDeviceCaps(&d3dCaps);

	if (pMeshContainer->pSkinInfo == NULL)
		return hr;

	g_bUseSoftwareVP = false;

	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

	// if non-indexed skinning mode selected, use ConvertToBlendedMesh to generate drawable mesh
	if (g_SkinningMethod == D3DNONINDEXED)
	{

		hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
		(
			pMeshContainer->pOrigMesh,
			D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh
		);
		if (FAILED(hr))
			goto e_Exit;


		// If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
		// Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
		// drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
		LPD3DXBONECOMBINATION rgBoneCombinations = reinterpret_cast<LPD3DXBONECOMBINATION>(
			pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// look for any set of bone combinations that do not fit the caps
		for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups;
			pMeshContainer->iAttributeSW++)
		{
			DWORD cInfl = 0;

			for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
			{
				if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
				{
					++cInfl;
				}
			}

			if (cInfl > d3dCaps.MaxVertexBlendMatrices)
			{
				break;
			}
		}

		// if there is both HW and SW, add the Software Processing flag
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			LPD3DXMESH pMeshTmp;

			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING |
				pMeshContainer->MeshData.pMesh->GetOptions(),
				pMeshContainer->MeshData.pMesh->GetFVF(),
				pd3dDevice, &pMeshTmp);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMeshTmp;
			pMeshTmp = NULL;
		}
	}
	// if indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if (g_SkinningMethod == D3DINDEXED)
	{
		DWORD NumMaxFaceInfl;
		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

		LPDIRECT3DINDEXBUFFER9 pIB;
		hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB,
			pMeshContainer->pOrigMesh->GetNumFaces(),
			&NumMaxFaceInfl);
		pIB->Release();
		if (FAILED(hr))
			goto e_Exit;

		// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
		// can be handled
		NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

		if (d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl)
		{
			// HW does not support indexed vertex blending. Use SW instead
			pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
			pMeshContainer->UseSoftwareVP = true;
			g_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}
		else
		{
			// using hardware - determine palette size from caps and number of bones
			// If normals are present in the vertex data that needs to be blended for lighting, then 
			// the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
			pMeshContainer->NumPaletteEntries = min((d3dCaps.MaxVertexBlendMatrixIndex + 1) / 2,
				pMeshContainer->pSkinInfo->GetNumBones());
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}

		pMeshContainer->NumPaletteEntries = NumMaxFaceInfl;

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		(
			pMeshContainer->pOrigMesh,
			Flags,
			pMeshContainer->NumPaletteEntries,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;
	}
	// if vertex shader indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
	else if ((g_SkinningMethod == D3DINDEXEDVS) || (g_SkinningMethod == D3DINDEXEDHLSLVS))
	{
		// Get palette size
		// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
		// (96 - 9) /3 i.e. Maximum constant count - used constants 
		UINT MaxMatrices = 26;
		pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
		if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
		{
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}
		else
		{
			pMeshContainer->UseSoftwareVP = true;
			g_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		(
			pMeshContainer->pOrigMesh,
			Flags,
			pMeshContainer->NumPaletteEntries,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->NumInfl,
			&pMeshContainer->NumAttributeGroups,
			&pMeshContainer->pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;


		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL |
			D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF,
				pd3dDevice, &pMesh);
			if (!FAILED(hr))
			{
				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}

		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
		//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
		//          this is more of a "cast" operation
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
			pDeclCur++;
		}

		hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if (g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
		{
			g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			delete[] g_pBoneMatrices;
			g_pBoneMatrices = new D3DXMATRIXA16[g_NumBoneMatricesMax];
			if (g_pBoneMatrices == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}

	}
	// if software skinning selected, use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
	else if (g_SkinningMethod == SOFTWARE)
	{
		hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
			pd3dDevice, &pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
		if (FAILED(hr))
			goto e_Exit;

		delete[] pMeshContainer->pAttributeTable;
		pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
		if (pMeshContainer->pAttributeTable == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
		if (FAILED(hr))
			goto e_Exit;

		// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
		if (g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
		{
			g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			// Allocate space for blend matrices
			delete[] g_pBoneMatrices;
			g_pBoneMatrices = new D3DXMATRIXA16[g_NumBoneMatricesMax];
			if (g_pBoneMatrices == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}
		}
	}
	else  // invalid g_SkinningMethod value
	{
		// return failure due to invalid skinning method value
		hr = E_INVALIDARG;
		goto e_Exit;
	}

e_Exit:
	return hr;
}

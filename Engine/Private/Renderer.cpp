#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

_uint		g_iShadowWindowSizeX = 8192;
_uint		g_iShadowWindowSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fViewPortWidth = ViewportDesc.Width;
	m_fViewPortHeight = ViewportDesc.Height;

	for (int i = 0; i < 32; ++ i) {

		m_Randoms[i].x = (static_cast<_float>(std::rand()) / (RAND_MAX / (_float)2.f) - (_float)2.f / 2.f);
		m_Randoms[i].y = (static_cast<_float>(std::rand()) / (RAND_MAX / (_float)2.f) - (_float)2.f / 2.f);
		m_Randoms[i].z = (static_cast<_float>(std::rand()) / (RAND_MAX / (_float)2.f) - (_float)2.f / 2.f);
	}

	/* 디퍼드 쉐이딩을 위한 렌더타겟들을 생성하자. */
	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iShadowWindowSizeX, g_iShadowWindowSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Bright */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bright"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Blur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* MRT_GameObjects : 객체들의 특정 정보를 받아오기위한 렌더타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* MRT_LightAcc : 빛들의 연산결과 정보를 받아오기위한 렌더타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_Bright : 글로우를 먹이기 위한 객체. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bright"), TEXT("Target_Bright"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;

	/* MRT_ShadowObject : 객체들의 특정 정보를 받아오기위한 렌더타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObject"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/RandomNormal.png"));
	if (nullptr == m_pTexture)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	m_WorldMatrix._41 = 0.f;
	m_WorldMatrix._42 = 0.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iShadowWindowSizeX;
	TextureDesc.Height = g_iShadowWindowSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth"), ViewportDesc.Width - 150.f, 150.0f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Bright"), 500.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Blur"), 500.f, 300.f, 200.f,  200.f)))
		return E_FAIL;

#endif


	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (FAILED(m_pGameInstance->Clear()))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;

	// 여기서 렌더 브라이트
	// + 블러


	// 그림자 연산
	if (FAILED(Render_Shadow()))
		return E_FAIL;

#pragma region 빛 연산
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Result()))
		return E_FAIL;
#pragma endregion

	if (FAILED(Render_Bright()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;
	if (FAILED(Render_UI_Late()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponents(CComponent* pRenderComponent)
{
	m_DebugComponents.emplace_back(pRenderComponent);

	Safe_AddRef(pRenderComponent);

	return S_OK;
}
#endif

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if(nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iShadowWindowSizeX;
	ViewPortDesc.Height = (_float)g_iShadowWindowSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObject"), m_pLightDepthDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOW].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* 렌더타겟을 교체한다. */
	/* 이 그룹에 있는 객체들을 다 빛연산이 필요하다. => 빛연산을 후처리로 할꺼다. */
	/* 후처리를 위해서는 빛연산을 위한 데이터가 필요하다. => 빛 : 빛매니져, ☆노멀,재질☆ : 이새끼를 받아오고 싶어서!!!!! 렌더타겟에 ㅈ2ㅓ장해서 받아올라고!! */
	/* Diffuse를 0번째에 셋, Normal를 1번째에 셋 */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return pSour->Get_ViewZ() > pDest->Get_ViewZ();
	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();	
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI_Late()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI_LATE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI_LATE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Bright()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bright"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_BRIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Bright();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BRIGHT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_Width", &m_fViewPortWidth, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_Height", &m_fViewPortHeight, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Bright"), "g_Texture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();

	m_pShader->Begin(4);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Bright"), "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Blur"), "g_BlurTexture")))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_SampleOffsets", &m_Randoms[0], sizeof(_float3) * 32)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_RandomNormalTexture"), 0))
		return E_FAIL;


	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Result()
{
	/* 백버퍼에다가 디퍼드 방식으로 연산된 최종 결과물을 찍어준다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;

	_float4x4 ViewMatrix = m_pGameInstance->Get_Shadow_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Shadow_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &ProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr != pDebugCom)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_ShadowObject"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Bright"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Blur"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

#endif

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderList)		
			Safe_Release(pRenderObject);
		RenderList.clear();		
	}

#ifdef _DEBUG
	for (auto& pDebugComponents : m_DebugComponents)
		Safe_Release(pDebugComponents);
	m_DebugComponents.clear();
#endif

	Safe_Release(m_pLightDepthDSV);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

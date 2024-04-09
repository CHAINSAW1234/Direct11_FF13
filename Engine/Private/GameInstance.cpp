#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Renderer.h"
#include "Picking.h"
#include "Light_Manager.h"
#include "Font_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()	
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화한다 .*/
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if(nullptr == m_pGraphic_Device)
		return E_FAIL;	

	m_pInput_Device = CInput_Device::Create(hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	/*m_pFont_Manager = CFont_Manager::Create(*ppGraphic_Device);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
*/
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* 사운드 디바이스를 초기화한다 .*/

	/* 오브젝트 매니져의 공간 예약을 한다. */
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	/* 컴포넌트 매니져의 공간 예약을 한다. */
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	
	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create();
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine)
		return;

	m_pInput_Device->Tick();

	m_pLevel_Manager->Open_Level();
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);
	m_pPipeLine->Tick();

	m_pPicking->Update(m_pPipeLine);

	m_pObject_Manager->Late_Tick(fTimeDelta);
	
}

HRESULT CGameInstance::Begin_Draw(const _float4& vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}


HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* 화면에 그려져야할 객체들을 그리낟. == 오브젝트 매니져에 들어가있을꺼야 .*/
	/* 오브젝트 매니져에 렌더함수를 만들어서 호출하면 객체들을 다 그린다. */

	/* But. CRenderer객체의 렌더함수를 호출하여 객체를 그리낟. */
	m_pRenderer->Render();

	m_pLevel_Manager->Render();

	return S_OK;
}
HRESULT CGameInstance::Clear(_uint iClearLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	/* 지정된 레벨용 자원(텍스쳐, 사운드, 객체등등) 을 삭제한다. */

	/* 사본 게임오브젝트. */
	m_pObject_Manager->Clear(iClearLevelIndex);

	/* 컴포넌트 원형 */
	m_pComponent_Manager->Clear(iClearLevelIndex);

	return S_OK;
}

_byte CGameInstance::Get_KeyState(_uint eState, _ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_KeyState(eState, byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);	
}

HRESULT CGameInstance::Open_Level(_uint iNewLevelID, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Request_Open_Level(iNewLevelID, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

void CGameInstance::Start()
{
	m_pObject_Manager->Start();
}


CGameObject* CGameInstance::Add_Clone_With_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_Clone_With_Object(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

const CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

size_t CGameInstance::Get_LayerCnt(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return 0;

	return m_pObject_Manager->Get_LayerCnt(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);	
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Transform_PickingToLocalSpace(const CTransform* pTransform, _Out_ _float4* pRayDir, _Out_ _float4* pRayPos)
{
	return m_pPicking->Transform_PickingToLocalSpace(pTransform, pRayDir, pRayPos);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition_Vector();
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition_Float4();
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRadian);
}


void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();

	Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pRenderer);	
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pFont_Manager);
}

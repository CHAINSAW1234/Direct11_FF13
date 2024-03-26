#include "stdafx.h"
#include "UI_Chain.h"
#include "Monster.h"
#include "Player_Battle.h"

CUI_Chain::CUI_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Chain::CUI_Chain(const CUI_Chain& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Chain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Chain::Initialize(void* pArg)
{
	//if (nullptr == pArg)
	//	return E_FAIL;

	//UI_DESC* pUI_Desc = (UI_DESC*)pArg;
	//m_pPlayerInfo = (CPlayer_Battle*)pUI_Desc->pObserver_Hander;
	//if (nullptr == m_pPlayerInfo)	// 이부분 주의
	//	return E_FAIL;
	//Safe_AddRef(m_pPlayerInfo);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pPlayerInfo->RegisterObserver(this);

	return S_OK;
}

void CUI_Chain::Tick(_float fTimeDelta)
{
	m_fMaskMovement += fTimeDelta / 10;
	Check_Target();
	Update_Ratio();
	Update_Position(fTimeDelta);
}

HRESULT CUI_Chain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (m_isBreak) {
		if (FAILED(m_pShaderCom->Begin(2)))	// inner
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))	// border
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Begin(2)))	// inner1	: fRatio, fCurRatio 반영	// pass 바꿀것
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))	// Border
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_Chain::Start()
{
	m_fSizeX = 200;
	m_fSizeY = 10;
	m_fMoveTimeDelta = 1.f;
	m_vTargetPosition = { 350.f, 200.f, 0.f };
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vTargetPosition), 1.f));
}

void CUI_Chain::OnNotify()
{
}

void CUI_Chain::Change_Target(CGameObject* pGameObject)
{
	m_isRender = true;
	Safe_Release(m_pTarget);
	m_pTarget = pGameObject;
	Safe_AddRef(m_pTarget);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ((CTransform*)(m_pTarget->Get_Component(g_strTransformTag)))->Get_State_Vector(CTransform::STATE_POSITION));
}

HRESULT CUI_Chain::Bind_ShaderResources()
{
	_float4 vColor = { 0.f,1.f,1.f,1.f };
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	m_pTextureBorderCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Border", 0);
	m_pTextureMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fCurRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fMaskMovement, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Inner */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Inner"),
		TEXT("Com_Texture_Inner"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_Border */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Chain_Border"),
		TEXT("Com_Texture_Border"), (CComponent**)&m_pTextureBorderCom)))
		return E_FAIL;

	/* For.Com_Texture_UV */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		TEXT("Com_Texture_UV"), (CComponent**)&m_pTextureMaskCom)))
		return E_FAIL;

	return S_OK;
}

void CUI_Chain::Set_Movement()
{
	m_fMoveTimeDelta = 0.f;
}

void CUI_Chain::Update_Position(_float fTimeDelta)
{
	if (m_isChasing) {
		if (nullptr == m_pTarget)
			return;
		_float4 vPosition = ((CTransform*)(m_pTarget->Get_Component(g_strTransformTag)))->Get_State_Float4(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);	// 수정 필요함

	}
	else {
		Move(fTimeDelta);
	}
}

void CUI_Chain::Move(_float fTimeDelta)
{
	if (m_fMoveTimeDelta >= 1.0f) {
		return;
	}

	m_fMoveTimeDelta += fTimeDelta;
	if (m_fMoveTimeDelta >= 1.0f) {
		m_fMoveTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fMoveTimeDelta), 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);
}

void CUI_Chain::Update_Ratio()
{
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
	if (nullptr == pMonster) {
		m_isBreak = false;
		m_fRatio = m_fCurRatio = 0.f;
		m_fChain = m_fCurChain = 100.f;
		m_fStagger = INFINITY;
		return;
	}

	m_isBreak = pMonster->Get_Break();
	if(m_isBreak)
		m_fBreakTimeDelta = pMonster->Get_BreakTime();
	else {
		m_fChain = pMonster->Get_Chain();
		m_fCurChain = pMonster->Get_CurrentChain();
		m_fStagger = pMonster->Get_Stagger();
	}

	if (m_isBreak) {
		m_fChain = pMonster->Get_Chain();
		m_fRatio = m_fCurRatio = 20.f - m_fBreakTimeDelta / 20.f;
	}
	else {
		m_fRatio = m_fChain / m_fStagger;
		m_fCurRatio = m_fCurChain / m_fStagger;
	}

}

void CUI_Chain::Check_Target()
{
	if (nullptr == m_pTarget)
		return;

	if (true == m_pTarget->Get_Dead()) {
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
		m_isRender = false;
	}

}

CUI_Chain* CUI_Chain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chain* pInstance = new CUI_Chain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI* CUI_Chain::Clone(void* pArg)
{
	CUI_Chain* pInstance = new CUI_Chain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Chain::Free()
{
	__super::Free();

	Safe_Release(m_pTextureBorderCom);
	Safe_Release(m_pTextureMaskCom);
	//Safe_Release(m_pPlayerInfo);
	Safe_Release(m_pTarget);
}

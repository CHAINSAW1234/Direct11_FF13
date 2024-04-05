#include "stdafx.h"
#include "MapObject.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMapObject::CMapObject(const CMapObject& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CMapObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    MAPOBJECT_DESC* pMapObjectDesc = (MAPOBJECT_DESC*)pArg;
    m_strModelTag = pMapObjectDesc->strModelTag;

    m_eLevel = g_Level;

    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CMapObject::Tick(_float fTimeDelta)
{
}

HRESULT CMapObject::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    return S_OK;

}

HRESULT CMapObject::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i) {
        m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    if(nullptr != m_pNavigationCom)
        m_pNavigationCom->Render();

    return S_OK;
}

_bool CMapObject::Compute_Picking(_Out_ _float4* vOutPos)
{
    return m_pModelCom->Compute_Picking(m_pTransformCom, vOutPos);
}

void CMapObject::Tick_Navigation()
{
    _matrix vMatrix = m_pTransformCom->Get_WorldMatrix();
    //_matrix vMatrix = XMMatrixIdentity();
    m_pNavigationCom->Tick(vMatrix);
}

HRESULT CMapObject::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, m_strModelTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    wstring strNaviTag;
    switch (m_eLevel) {
    case LEVEL_FIELD:
        strNaviTag = TEXT("Prototype_Component_Navigation_Field");
        break;
    case LEVEL_FIELD_BOSS:
        strNaviTag = TEXT("Prototype_Component_Navigation_Field_Boss");
        break;
    case LEVEL_BATTLE:
        strNaviTag = TEXT("Prototype_Component_Navigation_Battle");
        break;
    case LEVEL_BATTLE_BOSS:
        strNaviTag = TEXT("Prototype_Component_Navigation_Boss_Battle");
        break;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, strNaviTag,
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
        return S_OK;



    return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapObject* pInstance = new CMapObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CMapObject"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CMapObject"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
}

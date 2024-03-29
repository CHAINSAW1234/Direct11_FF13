#include "stdafx.h"
#include "Chr_Battle_Light.h"

#include "Chr_Battle_Light_State_Idle.h"
#include "Chr_Battle_Light_State_Attack.h"
#include "Chr_Battle_Light_State_Hit.h"
#include "Chr_Battle_Light_State_Dead.h"
#include "Chr_Battle_Light_State_Item.h"
#include "Chr_Battle_Light_State_Optima.h"
#include "Chr_Battle_Light_State_Finish.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Body.h"
#include "Weapon_Anim.h"
#include "Ability.h"

#include "ImGUI_Manager.h"

CChr_Battle_Light::CChr_Battle_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CChr_Battle{ pDevice, pContext }
{
}

CChr_Battle_Light::CChr_Battle_Light(const CChr_Battle_Light& rhs)
    : CChr_Battle{ rhs }
{
}

HRESULT CChr_Battle_Light::Initialize_Prototype()
{   
    m_strChrName = TEXT("라이트닝");
    return S_OK;
}

HRESULT CChr_Battle_Light::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

    return S_OK;
}

void CChr_Battle_Light::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Update_FSMState(fTimeDelta);

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_2))
        Min_Hp(50);

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_3))
        Add_Hp(50);


}

HRESULT CChr_Battle_Light::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    // 임시
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
    return S_OK;
}

HRESULT CChr_Battle_Light::Render()
{
    /*m_pImGUI_Manager->Tick(0);
    Show_ImGUI();
    m_pImGUI_Manager->Render();*/

    return S_OK;
}

void CChr_Battle_Light::Start()
{
    m_iMaxHp = m_iHp = 300;

    Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));
    m_pTransformCom->Look_At_ForLandObject(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION));
    Change_Animation_Weapon(WEAPON_OPEN_IDLE);
    Change_Animation_Weapon(CChr_Battle_Light::WEAPON_OPEN_IDLE);
    
}

HRESULT CChr_Battle_Light::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->Change_State(eState);

    return S_OK;
}

void CChr_Battle_Light::Change_Animation(ANIMATION_CHR_BATTLE_LIGHT iAnimationIndex, _bool isLoop)
{
    dynamic_cast<CBody*>(m_PartObjects[0])->Change_Animation(iAnimationIndex, isLoop);
}

void CChr_Battle_Light::Change_Animation_Weapon(ANIMATION_CHR_BATTLE_LIGHT_WEAPON iAnimationIndex)
{
    dynamic_cast<CWeapon_Anim*>(m_PartObjects[1])->Change_Animation(iAnimationIndex, false);
}



_int CChr_Battle_Light::Get_Command_Cost_Sum()
{
    _int iCost = { 0 };
    for (size_t i = 0; i < m_pCommands->size(); ++i) {
        iCost += m_pCommands->at(i).second;
    }
    return iCost;
}

CRole::SKILL CChr_Battle_Light::Get_Current_Command()
{
    if (nullptr == m_pCommands || m_pCommands->empty())
        return CRole::SKILL_END;

    return m_pCommands->front().first;
}

void CChr_Battle_Light::Use_Command()
{
    if (nullptr != m_pCommands && !m_pCommands->empty()) {
        m_fATB -= Get_Current_Command_Cost();
        m_pCommands->pop_front();
    }
}

void CChr_Battle_Light::Cancel_Command()
{
    if (nullptr != m_pCommands && !m_pCommands->empty())
        m_pCommands->pop_back();
}

void CChr_Battle_Light::Set_Command(deque<pair<CRole::SKILL, _int>>* pCommand)
{
    Safe_Delete(m_pCommands);

    m_pCommands = pCommand;
}

void CChr_Battle_Light::Use_Item()
{
    m_eItem = CInventory::ITEM_END;
}

void CChr_Battle_Light::Change_Role(CAbility::ROLE eRole)
{
    __super::Change_Role(eRole);
    Change_State(OPTIMA);
}

HRESULT CChr_Battle_Light::Add_Components()
{
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CChr_Battle_Light::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(IDLE, CChr_Battle_Light_State_Idle::Create(this));
    m_pFSMCom->Add_State(ATTACK, CChr_Battle_Light_State_Attack::Create(this));
    m_pFSMCom->Add_State(ITEM, CChr_Battle_Light_State_Item::Create(this));
    m_pFSMCom->Add_State(HIT, CChr_Battle_Light_State_Hit::Create(this));
    m_pFSMCom->Add_State(DEAD, CChr_Battle_Light_State_Dead::Create(this));
    m_pFSMCom->Add_State(OPTIMA, CChr_Battle_Light_State_Optima::Create(this));
    m_pFSMCom->Add_State(FINISH, CChr_Battle_Light_State_Finish::Create(this));

    Change_State(IDLE);
    return S_OK;
}

HRESULT CChr_Battle_Light::Add_Ability()
{
    m_pAbility = CAbility::Create();

    m_pAbility->Add_Role(CAbility::ATTACKER);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::ATTACK, 1);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::AREA_BLAST, 2);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::RUIN, 1);

    m_pAbility->Add_Role(CAbility::BLASTER);
    m_pAbility->Add_Skill(CAbility::BLASTER, CRole::THUNDER, 1);

    m_pAbility->Set_CurrentRole(CAbility::ATTACKER);

    return S_OK;
}

void CChr_Battle_Light::Update_FSMState(_float fTimeDelta)
{
    /*if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON)) {
        Change_State(ATTACK);
    }*/

    if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON)) {
        Change_State(HIT);
    }

    //if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_I)) {
    //    Change_State(ITEM);
    //}

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_O)) {
        Change_Animation(DEAD_START, false);
        Change_State(DEAD);
    }

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_U)) {
        Change_State(FINISH);
    }
}

void CChr_Battle_Light::Show_ImGUI()
{
    _float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
    _float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
    _float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
    _float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
    _float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

    string str;
    switch (m_eState) {
    case IDLE:
        str = "IDLE";
        break;
    case ATTACK:
        str = "ATTACK";
        break;
    case ITEM:
        str = "ITEM";
        break;
    //case HIT:
    //    str = "HIT";
    //    break;
    case DEAD:
        str = "DEAD";
        break;
    }

    ImGui::Begin("Chr_Battle Tool");
    if (ImGui::TreeNode("Transform")) {
        ImGui::InputFloat4("Right", vRight);
        ImGui::InputFloat4("Up", vUp);
        ImGui::InputFloat4("Look", vLook);
        ImGui::InputFloat4("Pos", vPos);

        ImGui::TreePop();
    }
    ImGui::Text("Current State : ");
    ImGui::SameLine();
    ImGui::Text(str.c_str());
    ImGui::Text("Animation Index : %d", Get_CurrentAnimationIndex());
    ImGui::Text("Animation Frame : %f", Get_CurrentTrackPosition());

    if (nullptr != m_pCommands) {
        ImGui::Text("Queue size : %d", m_pCommands->size());
    }

    ImGui::End();
}

void CChr_Battle_Light::Determine_Action_Based_On_Command()
{
    // IDLE일때만 사용하시오
    if (nullptr != m_pCommands && !m_pCommands->empty() && m_fATB >= Get_Command_Cost_Sum()) {
        Change_State(ATTACK);
    }
    else if (CInventory::ITEM_END !=  m_eItem) {
        Change_State(ITEM);
    }

}

HRESULT CChr_Battle_Light::Add_PartObjects()
{
    /* For.Part_Body */
    CPartObject* pBodyObject = { nullptr };
    CBody::BODY_DESC BodyDesc{};

    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    BodyDesc.strModelTag = TEXT("Prototype_Component_Model_Light_Battle");


    pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body"), &BodyDesc));
    if (nullptr == pBodyObject)
        return E_FAIL;

    m_PartObjects.push_back(pBodyObject);

    /* For.Part_Weapon */
    CPartObject* pWeaponObject = { nullptr };
    CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

    CModel* pModel = (CModel*)pBodyObject->Get_Component(TEXT("Com_Model"));

    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    WeaponDesc.pSocket = pModel->Get_BonePtr("R_weapon");
    WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Light_Weapon");

    pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
    if (nullptr == pWeaponObject)
        return E_FAIL;

    m_PartObjects.push_back(pWeaponObject);

    return S_OK;
}

CChr_Battle_Light* CChr_Battle_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChr_Battle_Light::Clone(void* pArg)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChr_Battle_Light::Free()
{
    __super::Free();

    Safe_Delete(m_pCommands);

    CImGUI_Manager::Destroy_Instance();
}

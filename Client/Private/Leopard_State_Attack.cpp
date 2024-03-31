#include "stdafx.h"
#include "Leopard_State_Attack.h"
#include "Leopard.h"

CLeopard_State_Attack::CLeopard_State_Attack(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Attack::OnStateEnter()
{
	m_eState = RUN;
	m_pLeopard->Reset_AttackTime();
	m_pLeopard->Change_Animation(CLeopard::RUN_START, false);
}

void CLeopard_State_Attack::OnStateUpdate(_float fTimeDelta)
{
	switch (m_eState) {
	case RUN:
		Run(fTimeDelta);
		break;
	case ATTACK:
		Attack();
		break;
	}
}

void CLeopard_State_Attack::OnStateExit()
{
}

void CLeopard_State_Attack::Run(_float fTimeDelta)
{
	_float fDegree = m_pLeopard->Cal_Degree_Target();

	_vector vTargetPosition = XMLoadFloat4(&m_pLeopard->Get_TargetPosition());
	_float fTargetPositionY = vTargetPosition.m128_f32[1];
	vTargetPosition.m128_f32[1] = 0.f;

	_vector vCurrentPosition = m_pLeopard->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	_float fCurrentPositionY = vTargetPosition.m128_f32[1];
	vCurrentPosition.m128_f32[1] = 0.f;

	_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];


	if (abs(fDegree) > 5) {
		m_pLeopard->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fDegree / (fDegree));
	}
	else {
		m_pLeopard->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pLeopard->Get_TargetPosition()));
	}

	if (fDist <= 1) {
		m_eState = ATTACK;
		m_pLeopard->Change_Animation(CLeopard::ATTACK, false);
	}

	m_pLeopard->Get_Transform()->Go_Straight(fTimeDelta * 3);

	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_Animation(CLeopard::RUN_IDLE, true);
}

void CLeopard_State_Attack::Attack()
{
	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_State(CLeopard::STATE_IDLE);

}

CLeopard_State_Attack* CLeopard_State_Attack::Create(CLeopard* pLeopard)
{
	CLeopard_State_Attack* pInstance = new CLeopard_State_Attack(pLeopard);

	return pInstance;
}

void CLeopard_State_Attack::Free()
{
	__super::Free();
}

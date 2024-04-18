#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Vanila_State_Heal final : public CFSM_State
{
private:
	CChr_Battle_Vanila_State_Heal(class CChr_Battle_Vanila* pChr_Battle_Vanila);
	virtual ~CChr_Battle_Vanila_State_Heal() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Heal();
private:
	class CChr_Battle_Vanila*	m_pChr_Battle_Vanila = { nullptr };
	class CUI_Skill*			m_pUI_Skill = { nullptr };
	_bool						m_isCommandFinish = { false };
public:
	static	CChr_Battle_Vanila_State_Heal* Create(class CChr_Battle_Vanila* pChr_Battle_Vanila);
	virtual void Free() override;

};

END
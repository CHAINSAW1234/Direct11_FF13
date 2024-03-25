#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Battle_Sazh_State_Optima final : public CFSM_State
{
private:
	CChr_Battle_Sazh_State_Optima(class CChr_Battle_Sazh* pChr_Battle_Sazh);
	virtual ~CChr_Battle_Sazh_State_Optima() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };

public:
	static	CChr_Battle_Sazh_State_Optima* Create(class CChr_Battle_Sazh* pChr_Battle_Sazh);
	virtual void Free() override;

};
END
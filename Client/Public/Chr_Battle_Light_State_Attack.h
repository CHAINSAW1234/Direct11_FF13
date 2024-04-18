#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_State_Attack final : public CFSM_State
{
	enum STATE { RUN, UP, DOWN, ATTACK, SKILL, FINISH, STATE_END };
	enum LAND { GROUND, AIR, LAND_END };
private:
	CChr_Battle_Light_State_Attack(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_State_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Run(_float fTimeDelta);
	void Up(_float fTimeDelta);
	void Down(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Skill(_float fTimeDelta);
	void Finish(_float fTimeDelta);


	_float Get_Dist_Y();
	
private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };
	STATE	m_eState = { STATE_END };
	LAND	m_eLand = { LAND_END };

	_float	m_fDegree = { 0.f };
	_bool	m_isCommandFinish = { false };
	_bool	m_isCommandUse = { false };

public:
	static	CChr_Battle_Light_State_Attack* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
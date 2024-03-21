#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Wait final : public CFSM_State
{
private:
	CUI_Battle_Stage_Wait(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Wait() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

	virtual void Start() override;

private:
	void	Update_Cursor();
	void	Update_KeyInput();


private:
	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	_int				m_iCursor = { 0 };

public:
	static	CUI_Battle_Stage_Wait* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END
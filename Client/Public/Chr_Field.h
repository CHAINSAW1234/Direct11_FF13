#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
END


BEGIN(Client)

class CChr_Field final : public CGameObject
{
public:
	enum STATE { IDLE, WALK, MOVE, ITEM, BATTLE_BEGIN, STATE_END };
	enum ANIMATION_CHR_FIELD {
		BATTLE_BEGIN_IDLE, BATTLE_BEGIN_SURPRISE,
		IDLE_LEFT_INARM, IDLE_LEFT_INARM_TO_NOR, IDLE_LEFT_INARM_TO_RIGHT,
		IDLE_NOR, IDLE_NOR_TO_LEFT_INARM, IDLE_RIGHT_INARM, IDLE_RIGHT_INARM_TO_LEFT,
		IDLE_TURN_LEFT, IDLE_TURN_RIGHT,
		ITEM_END, ITEM_IDLE, ITEM_START,
		MOVE_IDLE, MOVE_START, MOVE_START_WITH_TURN_LEFT, MOVE_START_WITH_TURN_RIGHT,
		MOVE_STOP_LEFT, MOVE_STOP_RIGHT, MOVE_TURN_LEFT, MOVE_TURN_RIGHT,
		WALK_IDLE, WALK_START_WITH_TURN_LEFT, WALK_START_WITH_TURN_RIGHT,
		WALK_STOP_LEFT, WALK_STOP_RIGHT, ANIMATION_END };

private:
	CChr_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Field(const CChr_Field& rhs);
	virtual ~CChr_Field() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CFSM*		m_pFSMCom = { nullptr };

	STATE		m_eState = { STATE_END };
	_bool		m_isControl = { true };				// 조작 가능 여부 -> 아이템 조작, 전투시 false 처리

	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

public: 
	CTransform* Get_Transform() { return m_pTransformCom; }
	_uint		Get_CurrentAnimationIndex() { return m_pModelCom->Get_CurrentAnimationIndex(); }
	_float		Get_CurrentTrackPosition() { return m_pModelCom->Get_CurrentTrackPosition(); }
	_bool		Is_Animation_Finished() { return m_pModelCom->isFinished(); }

public:
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop);

	_float4 Cal_Target_Direction();			// 카메라의 방향과 키입력을 바탕으로 이동 방향을 결정해서 리턴
	_float4	Get_Look();						// Player의 Look vector를 Y값을 지우고 리턴

private:
	HRESULT Add_Components();
	HRESULT Add_Component_FSM();
	HRESULT Bind_ShaderResources();

	void	Update_FSMState(_float fTimeDelta);

	void	Show_ImGUI();

public:
	static CChr_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
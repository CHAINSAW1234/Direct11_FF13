#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
END

BEGIN(Client)
class CMonster abstract : public CGameObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	/*============================GET============================*/
	wstring	Get_Name() { return m_strChrName; }
	_int	Get_Hp() { return m_iHp; }
	_int	Get_MaxHp() { return m_iMaxHp; }
	_bool	Get_Break() { return m_isBreak; }
	_float  Get_BreakTime() { return m_fBreakTimeDelta; }
	_float  Get_Chain() { return m_fChain; }
	_float  Get_CurrentChain() { return m_fCurChain; }
	_float  Get_Stagger() { return m_fStagger; }

	/*============================SET============================*/
	void Set_Target(CGameObject* pTargetObject);

	/*============================기타============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Add_Chain(_float fChain);

protected:
	void Update_Chain(_float fTimeDelta);

protected:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CFSM*		m_pFSMCom = { nullptr };

	wstring		m_strChrName = {};
	_int		m_iMaxHp = { 1 };
	_int		m_iHp = { 1 };

	// 체인의 지속시간은 20초로 고정합시다
	_bool		m_isBreak = { false };						// 브레이크 상태인지에 대한 처리
	_float		m_fBreakTimeDelta = { 0.f };
	_float		m_fMagnification = { 1.f };					// 체인 감소 배율
	_float		m_fChain = { 100.f };
	_float		m_fCurChain = { 100.f };					// 실시간으로 감소하는 Chain 값 : 이 값이 100이 되면 m_fChain을 100으로 초기화
	_float		m_fStagger = { 100.f };						// Chain이 이 값보다 높아지면 Break상태로 변환
	_float4		m_vStartPosition = { 0.f,0.f,0.f,1.f };		// 흠

	CGameObject* m_pTargetObject = { nullptr };

protected:
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END
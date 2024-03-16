#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct PartObject_Desc : public GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4x4			m_WorldMatrix;					// �ڽ��� ���� ���
	const _float4x4*	m_pParentMatrix = { nullptr };	// �� ������ �����ϰ� �ִ� GameObject == Parent�� ���� ����� �����ͷ� ����

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
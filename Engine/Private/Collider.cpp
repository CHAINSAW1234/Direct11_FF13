#include "..\Public\Collider.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_SPHERE.h"

#include "GameInstance.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent{ rhs }
	, m_eType { rhs.m_eType }
#ifdef _DEBUG
	, m_pInputLayout { rhs.m_pInputLayout}
	, m_pEffect { rhs.m_pEffect }
	, m_pBatch { rhs.m_pBatch }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout);
#endif





	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{	
	switch (m_eType)
	{
	case TYPE_AABB:
		//m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_OBB:
		//m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_SPHERE:
		//m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		break;
	}
	return S_OK;
}

HRESULT CCollider::Render()
{
	m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	/*m_pEffect->SetWorld();
	m_pEffect->SetView();
	m_pEffect->SetProjection();*/

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed To Created : CCollider"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CComponent * CCollider::Clone(void * pArg)
{
	CComponent*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCollider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}
	
#endif

	Safe_Release(m_pBounding);
}
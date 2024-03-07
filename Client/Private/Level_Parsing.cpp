#include "stdafx.h"
#include "Level_Parsing.h"

#include "GameInstance.h"
#include "MapObject.h"

CLevel_Parsing::CLevel_Parsing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Parsing::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Models()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Parsing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_S))
		Parse_Models();

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_T))
		Test_Parsed_Models();
	// �׽�Ʈ�� ���ؼ� �����̵��ϴ� �ڵ� �߰��ϱ�?
}

HRESULT CLevel_Parsing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("�Ľ� �����Դϴ�."));

	return S_OK;
}

HRESULT CLevel_Parsing::Ready_Models()
{
	SetWindowText(g_hWnd, TEXT("�� �ε���."));

	// 1. MapObjects
	for (size_t i = 0; i < 15; ++i) {
		string path = "../Bin/Resources/Models/MapObject/MapObject/map" + to_string(i) + ".fbx";

		CModel* pModel = { nullptr };
		pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, path, XMMatrixIdentity());
		
		if (nullptr == pModel)
			return E_FAIL;
		m_Models.push_back(pModel);
	}
	// 
	// 
	// 2. Animmodel;
	//_matrix		TransformMatrix = XMMatrixIdentity();

	///* Prototype_Component_Model_Fiona */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	//string path = "../Bin/Resources/Models/Fiona/Fiona.fbx";

	//CModel* pModel = { nullptr };
	//pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, path, TransformMatrix);

	//if (nullptr == pModel)
	//	return E_FAIL;
	//m_Models.push_back(pModel);

	return S_OK;
}

HRESULT CLevel_Parsing::Parse_Models()
{
	for (size_t i = 0; i < m_Models.size(); ++i) {
		string path = "../Bin/Resources/Models/MapObject/MapObject/map" + to_string(i) + ".bin";
		if (FAILED(m_Models[i]->Save_Model(path)))
			return E_FAIL;
	}
	//string path = "../Bin/Resources/Models/Fiona/Fiona.bin";
	//if (FAILED(m_Models[0]->Save_Model(path)))
	//	return E_FAIL;

	MSG_BOX(TEXT("PARSING END"));
	return S_OK;
}

HRESULT CLevel_Parsing::Test_Parsed_Models()
{
	// 1. MapObjects
	for (size_t i = 0; i < 15; ++i) {
		string path = "../Bin/Resources/Models/MapObject/MapObject/map" + to_string(i) + ".bin";

		CModel* pModel = { nullptr };
		pModel = CModel::Create(m_pDevice, m_pContext, path);

		if (nullptr == pModel)
			return E_FAIL;
		m_Models.push_back(pModel);
	}

	return S_OK;
}

CLevel_Parsing* CLevel_Parsing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Parsing* pInstance = new CLevel_Parsing(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Parsing"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Parsing::Free()
{
	__super::Free();

	for (auto& pModel : m_Models) {
		Safe_Release(pModel);
	}
	m_Models.clear();
}
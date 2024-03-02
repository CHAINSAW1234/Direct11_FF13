#pragma once

#include "Base.h"

/* ���� ������ ǥ���ϱ����� Ÿ�� ������ �����Ѵ�. */
/* aiNode, aiBone, aiAnimNode */

#pragma region Explain
/*
	1. aiNode : 

	2. aiBone :

	3. aiAnimNode : 

*/
#pragma region
BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones);

	_bool Compare_Name(const _char* pBoneName) {
		return !strcmp(m_szName, pBoneName);
	}
	
private:
	_char				m_szName[MAX_PATH] = { "" };	// �� ���� �̸�

	/* �� �� ���� ������� */
	_float4x4			m_TransformationMatrix;			// �� ���� ���� ��ġ. �θ� ����� ���ؼ� ���� ��ġ�� ����

	/* ���� �������. */
	_float4x4			m_CombinedTransformationMatrix; // �� ���� ���� ��ġ

	_int				m_iParentBoneIndex = { -1 };	// * model

public:
	static CBone* Create(const aiNode* pAINode, _int  iParentIndex);
	virtual void Free() override;
};

END
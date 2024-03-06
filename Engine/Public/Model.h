#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	TYPE Get_Type() const { return m_eModelType; }
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	const vector<class CMesh*> Get_Meshes() { return m_Meshes; }
	_uint Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_bool isFinished() {
		return m_Animations[m_iCurrentAnimIndex]->isFinished();
	}
public:
	void Set_Animation(_uint iAnimIndex, _bool isLoop) {
		m_isLoop = isLoop;


		// 이하 statement는 선형 보간을 위함
		m_iNextAnimIndex = iAnimIndex;
		m_isChange_Animation = true;
		m_fTime_Inear_Interpolation = 0;

	}

	void Set_TransformMatrix(_float4x4 TransformMatrix) {
		// 자주 사용하지 말것
		m_TransformMatrix = TransformMatrix;
		if (m_eModelType == TYPE_NONANIM) {
			// 버퍼 전체 교체;
		}
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

public:
	_bool	Compute_Picking(const class CTransform* pTransform, _Out_ _float4* vOutPos = nullptr);

	HRESULT	Save_Model(string filepath);
	
private:
	HRESULT Play_Animation_Linear_Interpolation(_float fTimeDelta);
	HRESULT	Load_Model(string filepath);

private:
	TYPE						m_eModelType = { TYPE_END };	// 애니메이션 모델과 논애니메이션 모델 구분 필요함
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

	// 이하 변수들은 aiScene의 정보를 멤버 변수로 가져와서 저장하는 변수들임
	// 따라서 Initialize이후 m_pAIScene 사용 안함 
private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	// 오프셋, 회전행렬, 스케일등을 위한 행렬
	// 모델에 따라 다르므로, 로더에서 처리
	_float4x4					m_TransformMatrix;

	// 이 모델이 사용하는 모든 뼈를 저장하고 있음
	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	// 셰이더에 전달할 m_OffsetMatrices
	// 셰이더 바인드 이전에 메쉬로 부터 받아옴
	_float4x4					m_MeshBoneMatrices[512];

	// 이전 애니메이션 인덱스를 저장
	_uint						m_iNextAnimIndex = { INFINITE };
	_float						m_fTime_Inear_Interpolation = { 0.f};
	_bool						m_isChange_Animation = { false };
	
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex = -1);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
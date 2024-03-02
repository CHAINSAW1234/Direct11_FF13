#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = pAIAnimation->mDuration;

	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;


	/* �� �ִϸ��̼��� ��� ���� ��Ʈ���ؾ��ϴ°�? */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<CBone*>& Bones)
{
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	for (auto& pChannel : m_Channels)
	{
		/* �� ���� ��������� ���� CBone�� TransformationMatrix�� �ٲ��. */
		pChannel->Invalidate_TransformationMatrix(Bones, m_fTrackPosition);
	}

}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
#pragma once
#include "UI.h"


BEGIN(Client)
class CPlayer_Battle;
class CChr_Battle_Light;
class CChr_Battle;				// ���߿� �̰ŷ� ����

class CUI_ATB final : public CUI
{
private:
	CUI_ATB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ATB(const CUI_ATB& rhs);
	virtual ~CUI_ATB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual void OnNotify()					 	 override;

private:
	HRESULT Add_Components();

private:
	// �̹����� 3�� �Ǵ� �� �̻� �ʿ��� ���� ����
	CTexture* m_pTextureUVCom = { nullptr };		// �̹����� �����̴°� ó�� ���̱� ���� ����ũ �ؽ�ó
	CTexture* m_pTextureBorderCom = { nullptr };	// �׵θ��� �����ϴ� �ؽ�ó��
													// UI���� �����ִ� �ؽ�ó�� ���η� �����ؼ� RGB Blue�� ������ ����
	CPlayer_Battle* m_pPlayerInfo = { nullptr };	// �÷��̾��� ������

	_float m_fRatio = { 0.f };						// ���� ����

public:
	static CUI_ATB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END
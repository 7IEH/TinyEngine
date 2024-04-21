#pragma once
#include "CAsset.h"

enum class PHYSICMATERIAL_COMBINEMODE
{
    Average,
    Minimum,
    Maximum,
    Mutiply
};

class CPhysicMaterial : public CAsset
{
private:
    float m_DynamicFriction;                      // �����̰� ���� �� ���Ǵ� ����, [0 ~ 1]
    float m_StaticFriction;                       // ǥ�� ���� ������ ���� ���� �� ���Ǵ� ����, [0 ~ 1]
    float m_Bounciness;                           // ǥ���� �󸶳� ź���� �ִ���, [0 ~ 1]
    PHYSICMATERIAL_COMBINEMODE m_FrictionCombine; // �浹�ϴ� �� ������Ʈ�� ������ �������� ���
    PHYSICMATERIAL_COMBINEMODE m_BounceCombine;   // �浹�ϴ� �� ������Ʈ�� ź���� �������� ���

public:
    float GetDynamicFriction() const { return m_DynamicFriction; }
    void SetDynamicFriction(float _Friction) { m_DynamicFriction = _Friction; }

    float GetStaticFriction() const { return m_StaticFriction; }
    void SetStaticFriction(float _Friction) { m_StaticFriction = _Friction; }

    float GetBounciness() const { return m_Bounciness; }
    void SetBounciness(float _Bounciness) { m_Bounciness = _Bounciness; };

    PHYSICMATERIAL_COMBINEMODE GetFrictionCombine() const { return m_FrictionCombine; }
    void SetFrictionCombine(PHYSICMATERIAL_COMBINEMODE _Mode) { m_FrictionCombine = _Mode; }

    PHYSICMATERIAL_COMBINEMODE GetBounceCombine() const { return m_BounceCombine; }
    void SetBounceCombine(PHYSICMATERIAL_COMBINEMODE _Mode) { m_BounceCombine = _Mode; }

public:
    CLONE(CPhysicMaterial);

    virtual int Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CPhysicMaterial(bool _bEngineAsset = false);
    virtual ~CPhysicMaterial();

    friend class CPhysicsMgr;
};

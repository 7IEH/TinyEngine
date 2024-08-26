#pragma once
#include "CKirbyAbility.h"
class CKirbyAbility_Fire : public CKirbyAbility
{
private:
    bool m_bFrmEnter;
    float m_SavedSpeed;
    float m_SaveRotSpeed;

    float m_ProjectileAcc;
    float m_ProjectileCooltime;

public:

    // ���� ����
    virtual void AttackCharge1Start() override;
    virtual void AttackCharge1StartEnter() override;
    virtual void AttackCharge1StartExit() override;

    // ���ڸ� ����
    virtual void AttackCharge1() override;
    virtual void AttackCharge1Enter() override;
    virtual void AttackCharge1Exit() override;

    // ���� ��
    virtual void AttackCharge1End() override;
    virtual void AttackCharge1EndEnter() override;
    virtual void AttackCharge1EndExit() override;

    // �޸��鼭 ����
    virtual void AttackCharge1Run() override;
    virtual void AttackCharge1RunEnter() override;
    virtual void AttackCharge1RunExit() override;

    virtual void ChangeAbility() override;
    virtual void ChangeAbilityEnter() override;
    virtual void ChangeAbilityExit() override;

public:
    CLONE(CKirbyAbility_Fire)
    CKirbyAbility_Fire();
    CKirbyAbility_Fire(const CKirbyAbility_Fire& _Origin);
    virtual ~CKirbyAbility_Fire();
};

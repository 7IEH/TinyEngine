#pragma once
#include "CKirbyAbility.h"

class CKirbyAbility_Sleep : public CKirbyAbility
{
private:
    CGameObject* m_SnotBubble;
    float m_PrevSpeed;
    const float m_Speed;

public:
    // �⺻ ���� ����
    virtual void IdleEnter();

    virtual void IdleStartEnter();
    virtual void IdleStartExit();

    virtual void RunEnter();
    virtual void RunExit();

    virtual void JumpFallEnter();
    virtual void JumpFallExit();

    // ����
    virtual void Attack();
    virtual void AttackEnter();
    virtual void AttackExit();

    virtual void AttackStartEnter();
    virtual void AttackStartExit();

    virtual void AttackEndEnter();
    virtual void AttackEndExit();

    // ���� ����
    virtual void ChangeAbility();
    virtual void ChangeAbilityEnter();
    virtual void ChangeAbilityExit();

public:
    CLONE(CKirbyAbility_Sleep)
    CKirbyAbility_Sleep();
    CKirbyAbility_Sleep(const CKirbyAbility_Sleep& _Origin);
    virtual ~CKirbyAbility_Sleep();
};

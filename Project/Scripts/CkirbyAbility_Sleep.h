#pragma once
#include "CKirbyAbility.h"

class CkirbyAbility_Sleep : public CKirbyAbility
{
public:
    // �⺻ ���� ����
    virtual void IdleEnter();
    virtual void IdleStartEnter();
    virtual void RunEnter();
    virtual void JumpFallEnter();

    // ����
    virtual void AttackEnter();
    virtual void AttackStartEnter();
    virtual void AttackEndEnter();

public:
    CLONE_DISABLE(CkirbyAbility_Sleep)
    CkirbyAbility_Sleep();
    virtual ~CkirbyAbility_Sleep();
};

#pragma once
#include "CKirbyAbility.h"
#include "CPlayerMgr.h"

class CKirbyAbility_Normal : public CKirbyAbility
{
public:
    // ���
    virtual void Attack();
    virtual void AttackEnter();
    virtual void AttackExit();

    // ����1
    virtual void ChargeAttack1();
    virtual void ChargeAttack1Enter();
    virtual void ChargeAttack1Exit();

    // ����2
    virtual void ChargeAttack2();
    virtual void ChargeAttack2Enter();
    virtual void ChargeAttack2Exit();

public:
    CLONE_DISABLE(CKirbyAbility_Normal)
    CKirbyAbility_Normal();
    virtual ~CKirbyAbility_Normal();
};

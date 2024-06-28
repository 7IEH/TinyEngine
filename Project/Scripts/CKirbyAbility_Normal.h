#pragma once
#include "CKirbyAbility.h"
#include "CPlayerMgr.h"

class CKirbyAbility_Normal : public CKirbyAbility
{
public:
    // RUN
    virtual void RunStart();
    virtual void RunStartEnter();
    virtual void RunStartExit();

    virtual void Run();
    virtual void RunEnter();
    virtual void RunExit();

    // ���
    virtual void Attack();
    virtual void AttackEnter();
    virtual void AttackExit();

    // ����1
    virtual void AttackCharge1();
    virtual void AttackCharge1Enter();
    virtual void AttackCharge1Exit();

    // ����2
    virtual void AttackCharge2();
    virtual void AttackCharge2Enter();
    virtual void AttackCharge2Exit();

    virtual void JumpAttack(){};
    virtual void JumpAttackEnter(){};
    virtual void JumpAttackExit(){};

public:
    CLONE_DISABLE(CKirbyAbility_Normal)
    CKirbyAbility_Normal();
    virtual ~CKirbyAbility_Normal();
};

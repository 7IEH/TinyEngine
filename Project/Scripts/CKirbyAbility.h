#pragma once

class CKirbyAbility
{
    // Kirby ���¿� ���õ� �Լ�
public:
    virtual void Run() = 0;
    virtual void RunEnter() = 0;
    virtual void RunExit() = 0;

    virtual void Jump() = 0;
    virtual void Hovering() = 0;
    virtual void Evasiveness() = 0;
    virtual void Sliding() = 0;

    virtual void Attack() = 0;
    virtual void ChargeAttack1() = 0;
    virtual void ChargeAttack2() = 0;
    virtual void JumpAttack() = 0;

    virtual void Throw();

public:
    virtual CKirbyAbility* Clone() = 0;

public:
    CKirbyAbility();
    virtual ~CKirbyAbility();
};

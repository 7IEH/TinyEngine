#pragma once

// ====================================================================================
// USAGE: Kirby�� Ability �������� States�� ��� �����ص� States Group Class
// ====================================================================================
// - Ability �������� ���(�ش� class�� ���Ե� �Լ�)�� State���� call�ϴ� ������� ����
// - Ability�� ������� ������ �� �ִ� ����� State���� ���� ����
// - Ability�� Object: ���� ���� �����ϴ� ����� �ٸ��Ƿ� ���� �ٸ� parent class�� ���

class CKirbyAbility
{
protected:
    // ��� �޽�: hat, weapon
    void ChangeState(const wstring& _strStateName);

public:
    // �ʼ�: Change, Idle, Attack, Jump Attack
    // ����: ChargeAttack1, ChargeAttack2

    //virtual void Idle();

    virtual void Run(){};
    virtual void RunEnter(){};
    virtual void RunExit(){};

    //virtual void Jump();

    //virtual void Hovering();

    //virtual void Evasiveness();

    //virtual void Sliding();

    virtual void Attack() = 0;
    virtual void AttackEnter() = 0;
    virtual void AttackExit() = 0;

    virtual void AttackCharge1(){};
    virtual void AttackCharge1Enter(){};
    virtual void AttackCharge1Exit(){};

    virtual void AttackCharge2(){};
    virtual void AttackCharge2Enter(){};
    virtual void AttackCharge2Exit(){};

    virtual void JumpAttack() = 0;
    virtual void JumpAttackEnter() = 0;
    virtual void JumpAttackExit() = 0;

    //virtual void Throw();

    //virtual void Change();

public:
    virtual CKirbyAbility* Clone() = 0;

public:
    CKirbyAbility();
    virtual ~CKirbyAbility();
};

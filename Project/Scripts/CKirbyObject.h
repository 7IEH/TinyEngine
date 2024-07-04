#pragma once

// ====================================================================================
// USAGE: Kirby�� Object �������� States�� ��� �����ص� States Group Class
// ====================================================================================
// - Object �������� ���(�ش� class�� ���Ե� �Լ�)�� State���� call�ϴ� ������� ����
// - Object�� ������� ������ �� �ִ� ����� State���� ���� ����
// - Ability�� Object: ���� ���� �����ϴ� ����� �ٸ��Ƿ� ���� �ٸ� parent class�� ���

class CKirbyObject
{
protected:
    // ��� �޽�: mesh
    void ChangeState(const wstring& _strStateName);

public:
    // �ʼ�: Deform, Attack
    // ����: Move

    virtual void Idle(){};
    virtual void IdleEnter(){};
    virtual void IdleExit(){};

    virtual void Run(){};
    virtual void RunEnter(){};
    virtual void RunExit(){};

    virtual void RunStart(){};
    virtual void RunStartEnter(){};
    virtual void RunStartExit(){};

    virtual void Jump(){};
    virtual void JumpEnter(){};
    virtual void JumpExit(){};

    virtual void JumpStart(){};
    virtual void JumpStartEnter(){};
    virtual void JumpStartExit(){};

    virtual void JumpEnd(){};
    virtual void JumpEndEnter(){};
    virtual void JumpEndExit(){};

    virtual void Landing(){};
    virtual void LandingEnter(){};
    virtual void LandingExit(){};

    virtual void LandingEnd(){};
    virtual void LandingEndEnter(){};
    virtual void LandingEndExit(){};

    // virtual void Hovering();

    // virtual void Evasiveness();

    // virtual void Sliding();

    virtual void Attack() = 0;
    virtual void AttackEnter() = 0;
    virtual void AttackExit() = 0;

    virtual void AttackEnd(){};
    virtual void AttackEndEnter(){};
    virtual void AttackEndExit(){};

    virtual void AttackCharge1(){};
    virtual void AttackCharge1Enter(){};
    virtual void AttackCharge1Exit(){};

    virtual void AttackCharge1Start(){};
    virtual void AttackCharge1StartEnter(){};
    virtual void AttackCharge1StartExit(){};

    virtual void AttackCharge1End(){};
    virtual void AttackCharge1EndEnter(){};
    virtual void AttackCharge1EndExit(){};

    virtual void AttackCharge2(){};
    virtual void AttackCharge2Enter(){};
    virtual void AttackCharge2Exit(){};

    virtual void AttackCharge2Start(){};
    virtual void AttackCharge2StartEnter(){};
    virtual void AttackCharge2StartExit(){};

    virtual void JumpAttack() = 0;
    virtual void JumpAttackEnter() = 0;
    virtual void JumpAttackExit() = 0;

    // virtual void Throw();

    // virtual void Deform();

public:
    virtual CKirbyObject* Clone() = 0;

public:
    CKirbyObject();
    virtual ~CKirbyObject();
};

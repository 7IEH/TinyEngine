#pragma once
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
// ====================================================================================
// USAGE: Kirby�� Ability �������� States�� ��� �����ص� States Group Class
// ====================================================================================
// - Ability �������� ���(�ش� class�� ���Ե� �Լ�)�� State���� call�ϴ� ������� ����
// - Ability�� ������� ������ �� �ִ� ����� State���� ���� ����
// - Ability�� Object: ���� ���� �����ϴ� ����� �ٸ��Ƿ� ���� �ٸ� parent class�� ���

class CKirbyAbility
{
protected:
    float           m_Charge1Time;
    float           m_Charge2Time;
    float           m_ComboSuccessTime;

protected:
    // ��� �޽�: hat, weapon
    void ChangeState(const wstring& _strStateName);

public:
    // �ʼ�: Change, Idle, Attack, Jump Attack
    // ����: ChargeAttack1, ChargeAttack2

    virtual void Idle(){};
    virtual void IdleEnter();
    virtual void IdleExit(){};

    virtual void Run(){};
    virtual void RunEnter();
    virtual void RunExit(){};

    virtual void RunStart(){};
    virtual void RunStartEnter();
    virtual void RunStartExit(){};

    virtual void Jump(){};
    virtual void JumpEnter();
    virtual void JumpExit(){};

    virtual void JumpStart(){};
    virtual void JumpStartEnter();
    virtual void JumpStartExit(){};

    virtual void JumpFall(){};
    virtual void JumpFallEnter();
    virtual void JumpFallExit(){};

    virtual void Landing(){};
    virtual void LandingEnter();
    virtual void LandingExit();

    virtual void LandingEnd(){};
    virtual void LandingEndEnter();
    virtual void LandingEndExit();

    //virtual void Hovering();

    //virtual void Evasiveness();

    virtual void Attack() = 0;
    virtual void AttackEnter() = 0;
    virtual void AttackExit() = 0;

    virtual void AttackEnd(){};
    virtual void AttackEndEnter(){};
    virtual void AttackEndExit(){};

    virtual void AttackCombo1(){};
    virtual void AttackCombo1Enter(){};
    virtual void AttackCombo1Exit(){};

    virtual void AttackCombo2(){};
    virtual void AttackCombo2Enter(){};
    virtual void AttackCombo2Exit(){};

    virtual void AttackCombo2End(){};
    virtual void AttackCombo2EndEnter(){};
    virtual void AttackCombo2EndExit(){};

    virtual void AttackCharge1(){};
    virtual void AttackCharge1Enter(){};
    virtual void AttackCharge1Exit(){};

    virtual void AttackCharge1Start(){};
    virtual void AttackCharge1StartEnter(){};
    virtual void AttackCharge1StartExit(){};

    virtual void AttackCharge1End(){};
    virtual void AttackCharge1EndEnter(){};
    virtual void AttackCharge1EndExit(){};

    virtual void AttackCharge1Run(){};
    virtual void AttackCharge1RunEnter(){};
    virtual void AttackCharge1RunExit(){};

    virtual void AttackCharge2(){};
    virtual void AttackCharge2Enter(){};
    virtual void AttackCharge2Exit(){};

    virtual void AttackCharge2Start(){};
    virtual void AttackCharge2StartEnter(){};
    virtual void AttackCharge2StartExit(){};

    virtual void AttackCharge2Run(){};
    virtual void AttackCharge2RunEnter(){};
    virtual void AttackCharge2RunExit(){};

    virtual void AttackCharge3(){};
    virtual void AttackCharge3Enter(){};
    virtual void AttackCharge3Exit(){};

    virtual void AttackCharge3Start(){};
    virtual void AttackCharge3StartEnter(){};
    virtual void AttackCharge3StartExit(){};

    virtual void AttackCharge3End(){};
    virtual void AttackCharge3EndEnter(){};
    virtual void AttackCharge3EndExit(){};

    virtual void JumpAttack(){};
    virtual void JumpAttackEnter(){};
    virtual void JumpAttackExit(){};

    //virtual void Throw();

    //virtual void Change();

    // ����
    virtual void ChangeAbility(){};
    virtual void ChangeAbilityEnter(){};
    virtual void ChangeAbilityExit(){};

public:
    float GetCharge1Time() const { return m_Charge1Time; }
    float GetCharge2Time() const { return m_Charge1Time + m_Charge2Time; }
    float GetComboSuccessTime() const { return m_ComboSuccessTime; }

public:
    virtual CKirbyAbility* Clone() = 0;

public:
    CKirbyAbility();
    virtual ~CKirbyAbility();
};

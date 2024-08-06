#pragma once
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyMoveController.h"
// ====================================================================================
// USAGE: Kirby�� Ability �������� States�� ��� �����ص� States Group Class
// ====================================================================================
// - Ability �������� ���(�ش� class�� ���Ե� �Լ�)�� State���� call�ϴ� ������� ����
// - Ability�� ������� ������ �� �ִ� ����� State���� ���� ����
// - Ability�� Object: ���� ���� �����ϴ� ����� �ٸ��Ƿ� ���� �ٸ� parent class�� ���

class CKirbyAbility
{
protected:
    Ptr<CPrefab>    m_Hat;
    Ptr<CPrefab>    m_Weapon;
    vector<wstring> m_vecWaitingAnim;
    float           m_Charge1Time;
    float           m_Charge2Time;
    float           m_ComboSuccessTime;

protected:
    void ChangeState(const wstring& _strStateName);

public:
    virtual void Idle(){};
    virtual void IdleEnter();
    virtual void IdleExit();

    virtual void IdleStart(){};
    virtual void IdleStartEnter(){};
    virtual void IdleStartExit(){};

    virtual void Run(){};
    virtual void RunEnter();
    virtual void RunExit(){};

    virtual void RunStart(){};
    virtual void RunStartEnter();
    virtual void RunStartExit(){};

    virtual void RunEnd(){};
    virtual void RunEndEnter(){};
    virtual void RunEndExit(){};

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

    virtual void Slide(){};
    virtual void SlideEnter();
    virtual void SlideExit(){};

    virtual void SlideStart(){};
    virtual void SlideStartEnter();
    virtual void SlideStartExit(){};

    virtual void SlideEnd(){};
    virtual void SlideEndEnter();
    virtual void SlideEndExit(){};

    virtual void SlideAttack(){};
    virtual void SlideAttackEnter(){};
    virtual void SlideAttackExit(){};

    virtual void Attack(){};
    virtual void AttackEnter(){};
    virtual void AttackExit(){};

    virtual void AttackStart(){};
    virtual void AttackStartEnter(){};
    virtual void AttackStartExit(){};

    virtual void AttackEnd(){};
    virtual void AttackEndEnter(){};
    virtual void AttackEndExit(){};

    virtual void AttackCombo1(){};
    virtual void AttackCombo1Enter(){};
    virtual void AttackCombo1Exit(){};

    virtual void AttackCombo1End(){};
    virtual void AttackCombo1EndEnter(){};
    virtual void AttackCombo1EndExit(){};

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

    virtual void JumpAttackStart(){};
    virtual void JumpAttackStartEnter(){};
    virtual void JumpAttackStartExit(){};

    virtual void Guard(){};
    virtual void GuardEnter();
    virtual void GuardExit(){};

    virtual void GuardRun(){};
    virtual void GuardRunEnter(){};
    virtual void GuardRunExit(){};

    //virtual void DropAbility();
    //virtual void DropAbilityEnter();
    //virtual void DropAbilityExit();

    // ����
    virtual void ChangeAbility() = 0;
    virtual void ChangeAbilityEnter() = 0;
    virtual void ChangeAbilityExit() = 0;

public:
    float GetCharge1Time() const { return m_Charge1Time; }
    float GetCharge2Time() const { return m_Charge1Time + m_Charge2Time; }
    float GetComboSuccessTime() const { return m_ComboSuccessTime; }

public:
    virtual CKirbyAbility* Clone() = 0;

public:
    CKirbyAbility();
    CKirbyAbility(const CKirbyAbility& _Origin);
    virtual ~CKirbyAbility();
};

#pragma once
#include "CMonsterUnitScript.h"

enum class BLADEKNIGHT_STATE
{
    Attack,
    AttackStart,
    Damage,
    DoubleAttack,
    Fall,
    Find,
    FindWait,
    FindWaitSub,
    Landing,
    Move,
    Retreat,
    Thrust,
    ThrustEnd,
    ThrustLoop,
    ThrustStart,
    ThrustStartWait,
    // ThrustWait,
    TornadoAttack,
    TornadoAttackCharge,
    TornadoAttackCharge2,
    // TornadoAttackChargeMax,
    Wait,
    Eaten,
};

class CBladeKnightSwordScript;

class CBladeKnightScript : public CMonsterUnitScript
{
private:
    BLADEKNIGHT_STATE m_State;
    CBladeKnightSwordScript* m_Sword;
    float m_PassedTime;
    bool m_bStepFlag;

    // Script Parameter
    float m_StepPower;
    float m_AttackRange;
    bool m_bTraceUnit;

private:
    virtual void begin() override;
    virtual void tick() override;

public:
    void ChangeState(BLADEKNIGHT_STATE _NextState);

private:
    void SetSword();
    void SetSwordEnable(bool _bEnable);

private:
    void EnterState();
    void ExitState();

    void Attack();
    void AttackStart();
    void Damage();
    void DoubleAttack();
    void Fall();
    void Find();
    void FindWait();
    void FindWaitSub();
    void Landing();
    void Move();
    void Retreat();
    void Thrust();
    void ThrustEnd();
    void ThrustLoop();
    void ThrustStart();
    void ThrustStartWait();
    void ThrustWait();
    void TornadoAttack();
    void TornadoAttackCharge();
    void TornadoAttackCharge2();
    void TornadoAttackChargeMax();
    void Wait();
    void Eaten();

private:
    virtual void OnTriggerEnter(CCollider* _OtherCollider) override;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CBladeKnightScript);

public:
    CBladeKnightScript();
    CBladeKnightScript(const CBladeKnightScript& origin);
    virtual ~CBladeKnightScript();
};

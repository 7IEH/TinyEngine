#pragma once
#include "CMonsterUnitScript.h"

enum class BLADEKNIGHTSWORD_STATE
{
    Thrust,
    ThrustEnd,
    ThrustLoop,
    ThrustStart,
    ThrustStartWait,
    // ThrustWait,
};

class CBladeKnightSwordScript : public CScript
{
private:
    BLADEKNIGHTSWORD_STATE m_State;

private:
    virtual void begin() override;
    virtual void tick() override;

public:
    void ChangeState(BLADEKNIGHTSWORD_STATE _NextState);

private:
    void EnterState();
    void ExitState();

    void Thrust();
    void ThrustEnd();
    void ThrustLoop();
    void ThrustStart();
    void ThrustStartWait();
    void ThrustWait();

private:
    void SlashEffect(Vec3 _vPos);

private:
    virtual void OnTriggerEnter(CCollider* _OtherCollider);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CBladeKnightSwordScript);

public:
    CBladeKnightSwordScript();
    CBladeKnightSwordScript(const CBladeKnightSwordScript& origin);
    virtual ~CBladeKnightSwordScript();
};

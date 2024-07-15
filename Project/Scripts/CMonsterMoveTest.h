#pragma once
#include <Engine/CScript.h>
class CMonsterMoveTest : public CScript
{
private:
    float m_fSpeed;

public:
    virtual void tick() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override{};
    virtual void LoadFromLevelFile(FILE* _File) override{};

public:
    CLONE(CMonsterMoveTest)
    CMonsterMoveTest();
    virtual ~CMonsterMoveTest();
};

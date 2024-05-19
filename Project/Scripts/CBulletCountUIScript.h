#pragma once
#include <Engine\\CScript.h>

class CBulletCountUIScript : public CScript
{
private:
    CGameObject* m_Player;


public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override{};
    virtual void LoadFromLevelFile(FILE* _File) override{};

    CLONE(CBulletCountUIScript);

public:
    CBulletCountUIScript();
    virtual ~CBulletCountUIScript();
};

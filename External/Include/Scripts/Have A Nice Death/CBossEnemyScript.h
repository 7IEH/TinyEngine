#pragma once
#include "CEnemyScript.h"

enum class BOSS_TYPE
{
    LIFE,
    EVILPUMPKIN,
};

class CBossEnemyScript : public CEnemyScript
{
protected:
    void SpawnBossUI(BOSS_TYPE _Type);
    void DestroyBossUI();
    void DamageLifeBarUpdate();
    void EndBossBattle();

private:
    virtual void OnDetectTargetEnter(CGameObject* _TargetObj) override{}; // �θ� OnDetectTargetEnter ��� X
    virtual void OnDetectTargetExit(CGameObject* _TargetObj) override{};  // �θ� OnDetectTargetExit ��� X

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CBossEnemyScript* Clone() = 0;

public:
    CBossEnemyScript(UINT _ScriptType);
    virtual ~CBossEnemyScript();
};

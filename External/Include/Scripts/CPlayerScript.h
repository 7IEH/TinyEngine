#pragma once
#include <Engine\\CScript.h>

class CPlayerScript : public CScript
{
private:
    Vec3 m_MoveVelocity;
    float m_Speed;
    float m_ForcePower;
    float m_JumpPower;
    float m_RayCastDist;

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    virtual void OnControllerColliderHit(struct ControllerColliderHit Hit);

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CPlayerScript);

public:
    CPlayerScript();
    CPlayerScript(const CPlayerScript& origin); // ��� ���� AddScriptParam() ��Ͻ� �ּҸ� �����ϱ� ������ ��������ڸ� �����������
    virtual ~CPlayerScript();
};

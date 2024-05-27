#pragma once
#include <Engine\\CScript.h>

class CPlayerCameraScript : public CScript
{
private:
    CGameObject* m_Player;
    Vec3 m_OffsetPos;
    float m_Speed;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CPlayerCameraScript);

public:
    CPlayerCameraScript();
    CPlayerCameraScript(const CPlayerCameraScript& origin); // ��� ���� AddScriptParam() ��Ͻ� �ּҸ� �����ϱ� ������ ��������ڸ� �����������
    virtual ~CPlayerCameraScript();
};

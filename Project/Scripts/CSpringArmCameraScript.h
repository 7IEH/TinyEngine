#pragma once
#include <Engine\\CScript.h>

// ===================================================
// Spring Arm Camera Script
// ī�޶� Ÿ���� �����ϰ�, �þ߰� ������ ��Ȳ�� ó��
// ===================================================
// Usage: Camera�� Target�� �ڽ����� �ΰ�, Camera�� �ش� ��ũ��Ʈ�� ���� �ٿ� ���
// Result: ī�޶� ������Ʈ�� �θ� spring arm object�� ��ó�� ����, ī�޶��� local transform ����

class CSpringArmCameraScript : public CScript
{
private:
    // Camera
    Vec3            m_CameraOffset;
    Vec3            m_TargetOffset;
    Vec3            m_ArmRotation;
    float           m_ArmLength;

    // Lag
    float           m_LagSpeed;
    float           m_LagMaxDistance;
    bool            m_bUnableLag;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    CLONE(CSpringArmCameraScript);

public:
    CSpringArmCameraScript();
    CSpringArmCameraScript(const CSpringArmCameraScript& _Origin);
    virtual ~CSpringArmCameraScript();
};
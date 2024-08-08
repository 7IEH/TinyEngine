#pragma once

#include <Engine/CScript.h>

// Transform�� Scale,Pos ���
// Sclae Absolute ���(Transform)
// Front - Down, Back - Up
// Box Collider
// Collider�� center, scale �ǵ��� x

class CLadderScript : public CScript
{
private:
    Vec3 m_UpDir;
    Vec3 m_DownDir;
    Vec3 m_TopPos;
    Vec3 m_BottomPos;
    float m_Scale; // ��ٸ� ���� ����
    float m_PlayerCapsuleScale;
    float m_Pivot; // ��ٸ��� �߽ɺ��� �÷��̾������ �Ÿ�

public:
    virtual void begin() override;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

private:
    virtual void OnTriggerEnter(CCollider* _OtherCollider) override;
    virtual void OnTriggerStay(CCollider* _OtherCollider) override;
    virtual void OnTriggerExit(CCollider* _OtherCollider) override;

public:
    CLONE(CLadderScript);
    CLadderScript();
    CLadderScript(const CLadderScript& _Origin);
    virtual ~CLadderScript();
};

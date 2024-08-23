#pragma once

#include <Engine/CScript.h>

// rigidbody (trigger x) -> 13�� ���̾�(Fire Bullet)
// Monster_trigger, WorldStatic, FireProjectile Layer�� �浹

enum class Fire_Step
{
    LV0,
    LV1,
    LV2,
    END,
};

class CKirbyFireBullet : public CScript
{
private:
    UINT m_Step;                        // Fire �ܰ�(��, ��, ��)
    
    bool m_bIsFreeze;
    bool m_bDestroy;

    int m_AccFireNumber;                // Fire�� ������ ����
    int m_RequiredFireNumber;           // ���� �ܰ�� ���� ���� ���� ���� ����

    float m_Acc;                        // Fire�� ���ӽð�
    float m_Duration;                   // Fire�� �ִ� ���� �ð�
    float m_DestroyDuration;            // Object �ı������� ���� �ð�


    Vec3 m_Dir;

public:
    float GetAcc() { return m_Acc; }
    UINT GetSetp() { return m_Step; }

private:
    void StepUp();
    void StepDown();

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    virtual void OnCollisionEnter(CCollider* _OtherCollider) override;
    virtual void OnTriggerEnter(CCollider* _OtherCollider) override;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CKirbyFireBullet);
    CKirbyFireBullet();
    CKirbyFireBullet(const CKirbyFireBullet& _Origin);
    virtual ~CKirbyFireBullet();

};

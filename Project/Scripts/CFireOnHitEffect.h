#pragma once

#include <Engine/CScript.h>

enum class FireEffect_Step
{
    LV0,
    LV1,
    LV2,
    END,
};


class CFireOnHitEffect : public CScript
{
private:
    UINT m_Step; // Fire �ܰ�(��, ��, ��)

    bool m_bDestroy;

    int m_AccFireNumber;      // Fire�� ������ ����
    int m_RequiredFireNumber; // ���� �ܰ�� ���� ���� ���� ���� ����

    float m_Acc;             // Fire�� ���ӽð�
    float m_Duration;        // Fire�� �ִ� ���� �ð�
    float m_DestroyDuration; // Object �ı������� ���� �ð�

private:
    virtual void begin() override;
    virtual void tick() override;

private:
    virtual void OnTriggerEnter(CCollider* _OtherCollider) override;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CFireOnHitEffect);
    CFireOnHitEffect();
    CFireOnHitEffect(const CFireOnHitEffect& Origin);
    virtual ~CFireOnHitEffect();

};

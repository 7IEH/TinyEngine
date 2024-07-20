#pragma once
#include <Engine\\CScript.h>

class CKirbyHatBlade : public CScript
{
private:
    float       m_AttackCoolTime;
    float       m_Acc;
    bool        m_bCanAttack;

    float       m_VariationDuration;
    float       m_VariationAcc;

public:
    // Ŀ�� ���� ���� ���
    void Attack();

    // Blade�� �ٽ� �Ӹ��� ���ƿ��� ��� ���� ������ ���·� �ʱ�ȭ
    void Reset();

    // ���� ������ ��������
    bool CanAttack() { return m_bCanAttack; }

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual UINT SaveToLevelFile(FILE* _File) override{};
    virtual UINT LoadFromLevelFile(FILE* _File) override{};

public:
    CLONE(CKirbyHatBlade);
    CKirbyHatBlade();
    virtual ~CKirbyHatBlade();

};
